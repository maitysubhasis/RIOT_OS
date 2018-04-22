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
#include "net/arp/table.h"

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
    uint8_t * l2_addr = NULL;
    
    assert(pkt != NULL);

    netif_hdr = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);
    if (netif_hdr != NULL) {
        l2_addr = gnrc_netif_hdr_get_src_addr(netif_hdr->data);
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
    
    // save the src arrr l2 addres in arp_table
    if (l2_addr != NULL) {
        update_arp_table(&(hdr->src), l2_addr);
        // uint8_t * found;
        // found = search_arp_table(&(hdr->src));
        // if (found != NULL) {
        //     memcpy(hwaddr, found, ETHERNET_ADDR_LEN);
        //      DEBUG("Saved l2 address: %02x:%02x:%02x:%02x:%02x:%02x\n",
        //       hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4],
        //       hwaddr[5]);
        // }
    }

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

    /* IPv4 internal demuxing (ICMP, UDP etc.) */
    // gnrc_ipv4_demux(hdr->protocol, next, pkt);
}

static void _send_to_iface(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    // printf("Sendind udp");
    // printf("MTU: %d\n", netif->ipv4.mtu);
    // printf("MTU: %d\n", gnrc_pkt_len(pkt->next));
    assert(netif != NULL);
    ((gnrc_netif_hdr_t *)pkt->data)->if_pid = netif->pid;
    if (gnrc_pkt_len(pkt->next) > netif->ipv4.mtu) {
        DEBUG("ipv4: packet too big\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

#ifdef MODULE_NETSTATS_IPV4
    netif->ipv4.stats.tx_success++;
    netif->ipv4.stats.tx_bytes += gnrc_pkt_len(pkt->next);
#endif
    if (gnrc_netapi_send(netif->pid, pkt) < 1) {
        DEBUG("ipv4: unable to send packet\n");
        gnrc_pktbuf_release(pkt);
    }
}

/**
 * Directly taken from ipv6 
 */
static gnrc_pktsnip_t *_create_netif_hdr(uint8_t *dst_l2addr,
                                         uint16_t dst_l2addr_len,
                                         gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *netif_hdr = gnrc_netif_hdr_build(NULL, 0, dst_l2addr, dst_l2addr_len);

    if (netif_hdr == NULL) {
        DEBUG("ipv6: error on interface header allocation, dropping packet\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }

    if (pkt->type == GNRC_NETTYPE_NETIF) {
        /* remove old netif header, since checking it for correctness would
         * cause to much overhead.
         * netif header might have been allocated by some higher layer either
         * to set a sending interface or some flags. Interface was already
         * copied using netif parameter, so we only need to copy the flags
         * (minus the broadcast/multicast flags) */
        DEBUG("ipv4: copy old interface header flags\n");
        gnrc_netif_hdr_t *netif_new = netif_hdr->data, *netif_old = pkt->data;
        netif_new->flags = netif_old->flags & \
                           ~(GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST);
        DEBUG("ipv4: removed old interface header\n");
        pkt = gnrc_pktbuf_remove_snip(pkt, pkt);
    }

    /* add netif_hdr to front of the pkt list */
    LL_PREPEND(pkt, netif_hdr);

    return pkt;
}

static void _send_unicast(gnrc_netif_t *netif, uint8_t *dst_l2addr, uint16_t dst_l2addr_len, gnrc_pktsnip_t *pkt)
{
    DEBUG("ipv4: add interface header to packet\n");
    if ((pkt = _create_netif_hdr(dst_l2addr, dst_l2addr_len, pkt)) == NULL) {
        return;
    }
    DEBUG("ipv4: send unicast over interface %" PRIkernel_pid "\n", netif->pid);

    /* and send to interface */
#ifdef MODULE_NETSTATS_IPV4
    netif->ipv4.stats.tx_unicast_count++;
#endif

    _send_to_iface(netif, pkt);
}

int gnrc_ipv4_calc_csum(gnrc_pktsnip_t *pkt)
{
    ipv4_hdr_t *hdr = pkt->data;
    uint8_t len = pkt->size;
    uint16_t * ptr = (uint16_t *)hdr;
    int times = len / 2 - 1;
    uint32_t csum = 0;

    // set the checksum to 0 first
    hdr->csum.u16 = 0;

    if (len % 2 == 1) {
        csum += *(ptr + times) & 0xff00;
        times--;
    }
    while(times >= 0) {
        csum += *(ptr + times--);
    }

    csum = ~((csum >> 16) + csum);
    hdr->csum.u16 = (uint16_t)csum;
    return 1;
}

static int _fill_ipv4_hdr(gnrc_netif_t *netif, gnrc_pktsnip_t *ipv4)
{
    int res;
    ipv4_hdr_t *hdr = ipv4->data;

    hdr->tl = byteorder_htons(gnrc_pkt_len(ipv4));
    hdr->ttl = 64;
    DEBUG("ipv4: set total length to %d\n", gnrc_pkt_len(ipv4));

    if (netif != NULL) {
        ipv4_addr_t * src_addr = &(netif->ipv4.addr);
        memcpy(&(hdr->src), src_addr, sizeof(ipv4_addr_t));
    }

    if ((res = gnrc_ipv4_calc_csum(ipv4)) < 0) {
        if (res != -ENOENT) {   /* if there is no checksum we are okay */
            DEBUG("ipv4: checksum calculation failed.\n");
            return res;
        }
    }

    return 0;
}

static void _send(gnrc_pktsnip_t *pkt, bool prep_hdr)
{
    gnrc_netif_t *netif = NULL;
    gnrc_pktsnip_t *ipv4; //, *payload;
    ipv4_hdr_t *hdr;
    uint8_t * l2addr;
    /* get IPv4 snip and (if present) generic interface header */
    if (pkt->type == GNRC_NETTYPE_NETIF) {
        /* If there is already a netif header (routing protocols and
         * neighbor discovery might add them to preset sending interface) */
        netif = gnrc_netif_get_by_pid(((gnrc_netif_hdr_t *)pkt->data)->if_pid);
        /* seize payload as temporary variable */
        ipv4 = gnrc_pktbuf_start_write(pkt); /* write protect for later removal
                                              * in _send_unicast() */
        if (ipv4 == NULL) {
            DEBUG("ipv4: unable to get write access to netif header, dropping packet\n");
            gnrc_pktbuf_release(pkt);
            return;
        }
        pkt = ipv4;  /* Reset pkt from temporary variable */

        ipv4 = pkt->next;
    }
    else {
        ipv4 = pkt;
    }
    /* seize payload as temporary variable */
    // payload = gnrc_pktbuf_start_write(ipv4);
    // if (payload == NULL) {
    //     DEBUG("ipv4: unable to get write access to IPv4 header, dropping packet\n");
    //     gnrc_pktbuf_release(pkt);
    //     return;
    // }
    // if (ipv4 != pkt) {      /* in case packet has netif header */
    //     pkt->next = payload;/* pkt is already write-protected so we can do that */
    // }
    // else {
    //     pkt = payload;      /* pkt is the IPv6 header so we just write-protected it */
    // }
    // ipv4 = payload;  /* Reset ipv4 from temporary variable */

    hdr = ipv4->data;
    // payload = ipv4->next;

    /**
     * TODO: 
     *  Add check for loopback address
     */

    if (prep_hdr) {
        if (_fill_ipv4_hdr(netif, ipv4) < 0) {
            /* error on filling up header */
            gnrc_pktbuf_release(pkt);
            return;
        }
    }
    // *** Now the netif
    if (netif == NULL) {
        gnrc_pktbuf_release(pkt);
        return;
    }

    l2addr = search_arp_table(&(hdr->dst));
    // print_arp_table();
    if (l2addr == NULL) {
        DEBUG("Dst l2_addr not found.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }
    
    _send_unicast(netif, l2addr, ETHERNET_ADDR_LEN,  pkt);
}

// void _send(gnrc_pktsnip_t pkt, bool blah)
// {
//     // check if dst address l2 address exists in arp_table
//     // else send arp request via msg_send_receive()
//     // wait for msg_reply()
//     // on reply check if the table is filled
//     // if filled send the pkt
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
                _send(msg.content.ptr, true);
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