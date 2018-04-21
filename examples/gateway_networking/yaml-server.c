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
#include "net/tcp.h"
#include "net/udp.h"
#include "net/sixlowpan.h"
#include "od.h"
#include "string.h"

#include "./yaml-parser/yaml.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#ifndef GATEWAY_H
#define GATEWAY_H

/**
 * @brief   PID of the pktdump thread
 */
kernel_pid_t gateway_pid = KERNEL_PID_UNDEF;

/**
 * @brief   Stack for the pktdump thread
 */
static char _stack[THREAD_STACKSIZE_MAIN];
static void *_eventloop(void *arg);

kernel_pid_t gateway_init(void)
{
    if (gateway_pid == KERNEL_PID_UNDEF) {
        gateway_pid = thread_create(_stack, sizeof(_stack), THREAD_PRIORITY_MAIN - 1,
                             THREAD_CREATE_STACKTEST,
                             _eventloop, NULL, "gateway");
    }
    return gateway_pid;
}

void _recv(gnrc_pktsnip_t *pkt)
{
    int len = pkt->size;
    int got = 0;
    char dbuff[2048];

    // DEBUG("Received packet: %s\n", (char *)pkt->data);

    got = getVal(dbuff, pkt->data, len, "proto");
    if (got && strcmp(dbuff, "yaml") == 0) {
        DEBUG("got yaml data, not process it.\n");
        if (getVal(dbuff, pkt->data, len, "type")) {
            if (strcmp(dbuff, "req") == 0) {
                // handle requests from PC
                // send the data to mote
                // get my ip
                // if my ip and dst ip are same send reply
                // send_ipv6();
            } else if (strcmp(dbuff, "res") == 0) {
                // handle responses from motes

            }
        }
    } else {
        gnrc_pktbuf_release(pkt);
    }

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