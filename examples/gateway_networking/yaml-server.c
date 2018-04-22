/**
 * @author      Subhasis Maity
 */

#include <inttypes.h>
#include <stdio.h>

#include <errno.h>
#include "byteorder.h"
#include "thread.h"
#include "msg.h"
#include "net/gnrc.h"
#include "net/icmpv6.h"
#include "net/ipv6/addr.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/ipv4.h"
#include "net/tcp.h"
#include "net/udp.h"
#include "net/sixlowpan.h"
#include "od.h"
#include "string.h"

#include "yaml-parser/yaml.h"
#include "udp.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#ifndef YAML_H
#define YAML_H

/**
 * @brief   PID of the pktdump thread
 */
kernel_pid_t gateway_pid = KERNEL_PID_UNDEF;

/**
 * @brief   Stack for the pktdump thread
 */
static char _stack[THREAD_STACKSIZE_MAIN];
static void *_eventloop(void *arg);

static char ipv4_addr_str[IPV4_ADDR_MAX_STR_LEN];
// static char ipv6_addr_str[IPV6_ADDR_MAX_STR_LEN];


kernel_pid_t gateway_init(void)
{
    if (gateway_pid == KERNEL_PID_UNDEF) {
        gateway_pid = thread_create(_stack, sizeof(_stack), THREAD_PRIORITY_MAIN - 1,
                             THREAD_CREATE_STACKTEST,
                             _eventloop, NULL, "gateway");
    }
    return gateway_pid;
}

gnrc_netif_t * get_wired_iface(void) 
{
    gnrc_netif_t *netif = NULL;
    uint16_t u16;

    while ((netif = gnrc_netif_iter(netif))) {
        if (gnrc_netapi_get(netif->pid, NETOPT_IS_WIRED, 0, &u16, sizeof(u16)) > 0) {
            return netif;
        }
    }
    return NULL;
}

gnrc_netif_t * get_wireless_iface(void) 
{
    gnrc_netif_t *netif = NULL;
    uint16_t u16;
    while ((netif = gnrc_netif_iter(netif))) {
        if (!(gnrc_netapi_get(netif->pid, NETOPT_IS_WIRED, 0, &u16, sizeof(u16)) > 0)) {
            return netif;
        }
    }
    return NULL;
}

int comp (char *a, char *b) {
    if (strncmp(a, b, strlen(b)) == 0) {
        return 1;
    }
    return 0;
}

void _recv(gnrc_pktsnip_t *pkt)
{
    int len = pkt->size;
    void * data = pkt->data;
    char * dp = NULL;
    char dbuff[2048];
    gnrc_netif_t * netif;
    // DEBUG("Received packet: %s\n", (char *)pkt->data);

    if (getVal(&dp, data, len, "proto") && comp(dp, "yaml")) {
        DEBUG("got yaml data, now process it.\n");
        getVal(&dp, data, len, "type");
        if (comp(dp, "req")) {
            DEBUG("Got req!\n");
            getVal(&dp, data, len, "src.type");
            if (comp(dp, "4")) {
                DEBUG("Got req from ipv4!\n");
                getVal(&dp, data, len, "src.addr");
                // send the data to mote
                getData(ipv4_addr_str, dp);
                netif = get_wired_iface();
                printf("%s\n", ipv4_addr_str);
                send_ipv4(ipv4_addr_str, "8806", "returning data", netif->pid);
            } else if (comp(dp, "6")) {
                DEBUG("Got req from ipv6!\n");
                // send the data to PC

            }
            // handle requests from PC
            // send the data to mote
            // get my ip
            // if my ip and dst ip are same send reply
            // send_ipv6();
        } else if (comp(dp, "res")) {
            // handle responses from motes
            getVal(&dp, data, len, "dst.type");
            if (comp(dp, "4")) {
                DEBUG("Got res from ipv4!\n");
                // send the data to mote
            } else if (comp(dp, "6")) {
                DEBUG("Got res from ipv6!\n");
                // send the data to PC
                getVal(&dp, data, len, "dst.addr");
                getData(ipv4_addr_str, dp);
                netif = get_wired_iface();
                send_ipv4(ipv4_addr_str, "8808", data, netif->pid);
            }
        }
    }
    (void)dbuff;
    gnrc_pktbuf_release(pkt);
}

static void *_eventloop(void *arg)
{
    (void)arg;
    msg_t msg, reply;
    msg_t msg_queue[(8U)];

    /* setup the message queue */
    msg_init_queue(msg_queue, (8U));

    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                puts("PKTDUMP: data received: --- ");
                _recv(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                puts("PKTDUMP: data to send:");
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                msg_reply(&msg, &reply);
                break;
            default:
                puts("PKTDUMP: received something unexpected");
                break;
        }
    }

    /* never reached */
    return NULL;
}

#endif