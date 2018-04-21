#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>

#include "byteorder.h"
#include "cpu_conf.h"
#include "kernel_types.h"
#include "net/gnrc.h"
#include "thread.h"
#include "utlist.h"

#include "net/gnrc/ipv4.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
static char _stack[GNRC_IPV4_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[GNRC_IPV4_STACK_SIZE];
#endif

// this variable will be used for debugging 
static char addr_str[IPV4_ADDR_MAX_STR_LEN];

kernel_pid_t gnrc_ipv4_pid = KERNEL_PID_UNDEF;

static void _receive(gnrc_pktsnip_t *pkt);
// static void _send(gnrc_pktsnip_t *pkt, bool prep_hdr);
static void *_event_loop(void *args);

kernel_pid_t gnrc_ipv4_init(void)
{
	 if (gnrc_ipv4_pid == KERNEL_PID_UNDEF) {
        gnrc_ipv4_pid = thread_create(_stack, sizeof(_stack), GNRC_IPV4_PRIO,
                                      THREAD_CREATE_STACKTEST,
                                      _event_loop, NULL, "ipv4");
        // su: 
        // printf("\nIPv4 pid: %d\n", gnrc_ipv4_pid);
    }
    return gnrc_ipv4_pid;
}

/*
 *         current                 pkt
 *         |                       |
 *         v                       v
 * IPv4 <- IPv6_EXT <- IPv6_EXT <- UNDEF
 */
// void gnrc_ipv4_demux(gnrc_netif_t *netif, gnrc_pktsnip_t *current,
//                      gnrc_pktsnip_t *pkt, uint8_t nh)
// {
//     // Let's assume we can get the next packet somehow from here
//     // now woth the pkt we do demuxing
//     // this should send the packet to next handler
//     // (ex. ICMPv4 or UDp or TCP)
//     _dispatch_next_header()
// }

/* functions for receiving */
static inline bool _pkt_not_for_me(gnrc_netif_t *netif, ipv4_hdr_t *hdr)
{
    // get ip addr for corresponding netif
    gnrc_netif_ipv4_t * netif_ipv4 = &(netif->ipv4);
    ipv4_addr_t * ipv4_addr = &(netif_ipv4->addr);
    ipv4_addr_to_str(addr_str, ipv4_addr, IPV4_ADDR_MAX_STR_LEN);
    // DEBUG("My ip address: %s.\n", addr_str);
    ipv4_addr_to_str(addr_str, &(hdr->dst), IPV4_ADDR_MAX_STR_LEN);
    // DEBUG("Target ip address: %s.\n", addr_str);
    return !ipv4_addr_equal(ipv4_addr, &(hdr->dst));
}

static void _receive(gnrc_pktsnip_t *pkt)
{
    gnrc_netif_t *netif = NULL;
    gnrc_pktsnip_t *ipv4, *netif_hdr, * next;
    ipv4_hdr_t *hdr;

    assert(pkt != NULL);

    netif_hdr = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);

    if (netif_hdr != NULL) {
        // get the netif from the packet
        netif = gnrc_netif_get_by_pid(((gnrc_netif_hdr_t *)netif_hdr->data)->if_pid);
    }
    

    for (ipv4 = pkt; ipv4 != NULL; ipv4 = ipv4->next) { /* find IPv4 header if already marked */
        if ((ipv4->type == GNRC_NETTYPE_IPV4) && (ipv4->size == sizeof(ipv4_hdr_t)) &&
            (ipv4_hdr_is(ipv4->data))) {
            break;
        }
        next = pkt;
    }

    // if collected ipv4 is NULL try to 
    if (ipv4 == NULL) {
        if (!ipv4_hdr_is(pkt->data)) {
            DEBUG("ipv4: Received packet was not IPv4, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        /* seize ipv6 as a temporary variable */
        ipv4 = gnrc_pktbuf_start_write(pkt);

        if (ipv4 == NULL) {
            DEBUG("ipv4: unable to get write access to packet, drop it\n");
            gnrc_pktbuf_release(pkt);
            return;
        }

        pkt = ipv4;     /* reset pkt from temporary variable */

        ipv4 = gnrc_pktbuf_mark(pkt, sizeof(ipv4_hdr_t), GNRC_NETTYPE_IPV4);

        next = pkt;
        pkt->type = GNRC_NETTYPE_UNDEF; /* snip is no longer IPv6 */

        if (ipv4 == NULL) {
            DEBUG("ipv4: error marking IPv4 header, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
    }
    
    /* extract header */
    hdr = (ipv4_hdr_t *)ipv4->data;
    DEBUG("ipv4: Received (src = %s, ",
          ipv4_addr_to_str(addr_str, &(hdr->src), sizeof(addr_str)));
    DEBUG("dst = %s)\n",
          ipv4_addr_to_str(addr_str, &(hdr->dst), sizeof(addr_str)));
    
    // if packate is not for me try to route it 
    // if (_pkt_not_for_me(netif, hdr)) {
    //     printf("\npacket is not for me so dropping for now\n");
    //     DEBUG("ipv6: packet destination not this host\n");
    //     gnrc_pktbuf_release(pkt);
    //     return;
    // }

    (void)netif;
    // gnrc_pktsnip_t * tmp;

    if (next == NULL) {
        gnrc_pktbuf_release(pkt);
        return;
    }

    switch (hdr->protocol) {
        case PROTNUM_UDP:
            if (gnrc_netapi_dispatch_receive(GNRC_NETTYPE_UDP,
                                            GNRC_NETREG_DEMUX_CTX_ALL,
                                            pkt) == 0) {
                gnrc_pktbuf_release(pkt);
                DEBUG("Releasing ipv4 packet.\n");
            }
            return;
        default:
            gnrc_pktbuf_release(pkt);
            return;

    }

        // tmp = gnrc_pktbuf_start_write(pkt);

        // if (tmp == NULL) {
        //     DEBUG("udp: unable to get write access to packet, drop it\n");
        //     gnrc_pktbuf_release(pkt);
        //     return;
        // }

        // pkt = tmp;     /* reset pkt from temporary variable */

        // ipv4 = gnrc_pktbuf_mark(pkt, sizeof(ipv4_hdr_t), GNRC_NETTYPE_IPV4);

        // next = pkt;
        // pkt->type = GNRC_NETTYPE_UNDEF; /* snip is no longer IPv6 */

        // if (ipv4 == NULL) {
        //     DEBUG("ipv4: error marking IPv4 header, dropping packet\n");
        //     gnrc_pktbuf_release(pkt);
        //     return;
        // }

    // if (ipv4->next->type == GNRC_NETTYPE_IPV4) {
    //     // printf("IPv4 protocol %i\n", next->type);
    // }
    // (void)next;
    // next = ipv4->next;
    // DEBUG("Next packet type is %d\n", hdr->type);
    // the packet is for me so do necessary processing


    // // ipv4->next should give the udp header
    /* IPv4 internal demuxing (ICMP, UDP etc.) */
    // gnrc_ipv4_demux(hdr->protocol, next, pkt);
}

// static void _send_to_iface(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
// {
//     assert(netif != NULL);
//     ((gnrc_netif_hdr_t *)pkt->data)->if_pid = netif->pid;
//     if (gnrc_pkt_len(pkt->next) > netif->ipv4.mtu) {
//         DEBUG("ipv4: packet too big\n");
//         gnrc_pktbuf_release(pkt);
//         return;
//     }

// // #ifdef MODULE_NETSTATS_IPV4
// //     netif->ipv4.stats.tx_success++;
// //     netif->ipv4.stats.tx_bytes += gnrc_pkt_len(pkt->next);
// // #endif

//     if (gnrc_netapi_send(netif->pid, pkt) < 1) {
//         DEBUG("ipv4: unable to send packet\n");
//         gnrc_pktbuf_release(pkt);
//     }
// }

// static void _send(gnrc_pktsnip_t *pkt, bool prep_hdr)
// {
//     gnrc_netif_t *netif = NULL;
//     gnrc_pktsnip_t *ipv6, *payload;
//     ipv6_hdr_t *hdr;
// }

// void _send(gnrc_pktsnip_t pkt, true)
// {
    // check if dst address l2 address exists in arp_table
    // else send arp request via msg_send_receive()
    // wait for msg_reply()
    // on reply check if the table is filled
    // if filled send the pkt
// }

static void *_event_loop(void *args)
{
    msg_t msg, reply, msg_q[GNRC_IPV4_MSG_QUEUE_SIZE];
    gnrc_netreg_entry_t me_reg = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                            sched_active_pid);

    (void)args;
    msg_init_queue(msg_q, GNRC_IPV4_MSG_QUEUE_SIZE);

    // /* register interest in all IPv4 packets */
    gnrc_netreg_register(GNRC_NETTYPE_IPV4, &me_reg);

    // /* preinitialize ACK */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    /* start event loop */
    while (1) {
        msg_receive(&msg);

        DEBUG("Received message at IPv4 layer.\n");
        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("ipv4: GNRC_NETAPI_MSG_TYPE_RCV received\n");
                _receive(msg.content.ptr);
                break;

            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("ipv4: GNRC_NETAPI_MSG_TYPE_SND received\n");
                // _send(msg.content.ptr, true);
                break;

            case GNRC_NETAPI_MSG_TYPE_SET:
                DEBUG("ipv4: reply to unsupported get/set\n");
                reply.content.value = -ENOTSUP;
                msg_reply(&msg, &reply);
                break;
            default:
                break;
        }
    }

    return NULL;
}