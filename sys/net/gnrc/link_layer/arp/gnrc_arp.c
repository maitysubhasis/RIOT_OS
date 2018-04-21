/*
 * Copyright (C) 2018 Subhasis Maity
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @author  Subhasis Maity
 */
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

#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
// #include "net/gnrc/netif/ethernet.h"

#include "net/gnrc/arp.h"
#include "net/gnrc/pktdump.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
static char _stack[GNRC_ARP_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[GNRC_ARP_STACK_SIZE];
#endif

kernel_pid_t gnrc_arp_pid = KERNEL_PID_UNDEF;

static char addr_str[IPV4_ADDR_MAX_STR_LEN];

static void _receive(gnrc_pktsnip_t *pkt, kernel_pid_t sender_iface);
// static void _send(gnrc_pktsnip_t *pkt, bool prep_hdr);
static void *_event_loop(void *args);

kernel_pid_t gnrc_arp_init(void)
{
	 if (gnrc_arp_pid == KERNEL_PID_UNDEF) {
        gnrc_arp_pid = thread_create(_stack, sizeof(_stack), GNRC_ARP_PRIO,
                                      THREAD_CREATE_STACKTEST,
                                      _event_loop, NULL, "arp");
        // su: 
        // printf("\nARP pid: %d\n", gnrc_arp_pid);
    }
    return gnrc_arp_pid;
}

void preapare_for_reply(arp_hdr_t *arp)
{
	uint8_t tmp_l2[ETHERNET_ADDR_LEN];
	ipv4_addr_t tmp_addr;
	memcpy(&tmp_l2, &(arp->src), sizeof(tmp_l2));
	memcpy(&tmp_addr, &(arp->spa), sizeof(tmp_addr));
	// 
	memcpy(&(arp->src), &(arp->tha), sizeof(tmp_l2));
	memcpy(&(arp->spa), &(arp->tpa), sizeof(tmp_addr));
	memcpy(&(arp->tha), &tmp_l2, sizeof(tmp_l2));
	memcpy(&(arp->tpa), &tmp_addr, sizeof(tmp_addr));
    // arp->oper = 0x0002;
}

static void _receive(gnrc_pktsnip_t *pkt, kernel_pid_t sender_iface)
{
    // printf("NETTYPE_UNKNOWN (%i)\n", pkt->type);
    int res;
    uint8_t hwaddr[GNRC_NETIF_L2ADDR_MAXLEN];
    arp_hdr_t * arp = (arp_hdr_t *)  pkt->data;
    uint8_t oper = arp->oper && 0x0002;
    if (oper == 1) {
        res = gnrc_netapi_get(sender_iface, NETOPT_ADDRESS, 0, hwaddr, sizeof(hwaddr));
        if (res >= 0) {
            gnrc_pktsnip_t *netif_hdr = gnrc_netif_hdr_build(NULL, 0, arp->src, sizeof(arp->src));
            
            memcpy(arp->tha, hwaddr, sizeof(hwaddr));
            // swap target and source addresses
            preapare_for_reply(arp);

            /* add netif_hdr to front of the pkt list */
            LL_PREPEND(pkt, netif_hdr);

            if (ENABLE_DEBUG) {
                ipv4_addr_to_str(addr_str, &(arp->spa), IPV4_ADDR_MAX_STR_LEN);
                DEBUG("Source ip address: %s.\n", addr_str);
                DEBUG("gnrc_netif_ethernet: src l2 addr %02x:%02x:%02x:%02x:%02x:%02x\n",
                    arp->src[0], arp->src[1], arp->src[2], arp->src[3], arp->src[4], arp->src[5]);
                
                ipv4_addr_to_str(addr_str, &(arp->tpa), IPV4_ADDR_MAX_STR_LEN);
                DEBUG("Target ip address: %s.\n", addr_str);
                DEBUG("gnrc_netif_ethernet: target l2 addr %02x:%02x:%02x:%02x:%02x:%02x\n",
                arp->tha[0], arp->tha[1], arp->tha[2], arp->tha[3], arp->tha[4], arp->tha[5]);
            }
            // send arp reply
            if (gnrc_netapi_send(sender_iface, pkt) < 1) {
                DEBUG("arp: unable to send packet\n");
                gnrc_pktbuf_release(pkt);
            }
        }
    } else if (oper == 2) {
        // save the data in forwarding ttable
        // msg_reply();
    }
}

// static void _send(gnrc_pktsnip_t *pkt, kernel_pid_t sender_iface)
// {
    
// }

    static void *_event_loop(void *args)
{
    msg_t msg, reply, msg_q[GNRC_ARP_MSG_QUEUE_SIZE];
    gnrc_netreg_entry_t me_reg = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                            sched_active_pid);

    (void)args;
    (void)reply;
    msg_init_queue(msg_q, GNRC_ARP_MSG_QUEUE_SIZE);

    // /* register interest in all IPv4 packets */
    gnrc_netreg_register(GNRC_NETTYPE_ARP, &me_reg);

    // /* preinitialize ACK */
    // reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    /* start event loop */
    while (1) {
        msg_receive(&msg);

        // su: 
        DEBUG("Received message at arp layer.\n");
        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("arp: GNRC_NETAPI_MSG_TYPE_RCV received\n");
                _receive(msg.content.ptr, msg.sender_pid);
                break;

            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("arp: GNRC_NETAPI_MSG_TYPE_SND received\n");
                // _send(msg.content.ptr, true);
                break;

            case GNRC_NETAPI_MSG_TYPE_SET:
                DEBUG("arp: reply to unsupported get/set\n");
                reply.content.value = -ENOTSUP;
                // msg_reply(&msg, &reply);
                break;
            default:
                break;
        }
    }

    return NULL;
}