/* Under Process
 * Copyright (C) 2018 Amit Verma <cs.amit.v@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @{
 *
 * @file
 */

#include "net/gnrc.h"
#include "od.h"
#include "net/gnrc/icmpv4.h"
#include "net/gnrc/icmpv4/echo.h"
#include "net/gnrc/ipv4/hdr.h"
#include "utlist.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#if ENABLE_DEBUG
#include <inttypes.h> /*#defining PRIu8 as "%hhu" and PRIu16 as "%
hu" with no changes to the compiler itself.*/
#endif

gnrc_pktsnip_t *gnrc_icmpv4_echo_build(uint8_t type, uint16_t id, uint16_t seq,
                                        uint8_t *data, size_t data_len)
{
    gnrc_pktsnip_t *pkt;
    icmpv4_echo_t *echo;

    if((pkt = gnrc_icmpv4_build(NULL, type, 0, data_len + sizeof(icmpv6_echo_t))) == NULL) {
    return NULL;
}
DEBUG("icmpv4_echo: Building echo message with type=%" PRIu8 "id=%" PRIu16 ",
                                                     seq=%" PRIu16, type, id, seq);
 echo = (icmpv4_echo_t *)pkt->data;
 echo->id = byteorder_htons(id);
 echo->seq = byteorder_htons(seq);

 if(data !=NULL) {
     memcpy(echo+1,data,data_len);
#if defined(MODULE_OD) && ENABLE_DEBUG
        DEBUG(", payload:\n");
        od_hex_dump(data, data_len, OD_WIDTH_DEFAULT);
#endif
    }
    DEBUG("\n");

    return pkt;

 }






 void gnrc_icmpv4_echo_req_handle(gnrc_netif_t *netif, ipv6_hdr_t *ipv4_hdr,
                                 icmpv4_echo_t *echo, uint16_t len)
{
    uint8_t *payload = ((uint8_t *)echo) + sizeof(icmpv4_echo_t);
    gnrc_pktsnip_t *hdr, *pkt;

    if ((echo == NULL) || (len < sizeof(icmpv4_echo_t))) {
        DEBUG("icmpv4_echo: echo was NULL or len (%" PRIu16
              ") was < sizeof(icmpv4_echo_t)\n", len);
        return;
    }

    pkt = gnrc_icmpv4_echo_build(ICMPV4_ECHO_REP, byteorder_ntohs(echo->id),
                                 byteorder_ntohs(echo->seq), payload,
                                 len - sizeof(icmpv4_echo_t));

    if (pkt == NULL) {
        DEBUG("icmpv4_echo: no space left in packet buffer\n");
        return;
    }

    if (ipv4_addr_is_multicast(&ipv4_hdr->dst)) {
        hdr = gnrc_ipv4_hdr_build(pkt, NULL, &ipv4_hdr->src);
    }
    else {
        hdr = gnrc_ipv4_hdr_build(pkt, &ipv4_hdr->dst, &ipv4_hdr->src);
    }

    if (hdr == NULL) {
        DEBUG("icmpv4_echo: no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    pkt = hdr;
    hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);

    if (netif != NULL) {
        ((gnrc_netif_hdr_t *)hdr->data)->if_pid = netif->pid;
    }
    else {
        /* ipv6_hdr->dst is loopback address */
        ((gnrc_netif_hdr_t *)hdr->data)->if_pid = KERNEL_PID_UNDEF;
    }

    LL_PREPEND(pkt, hdr);

    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV4, GNRC_NETREG_DEMUX_CTX_ALL,
                                   pkt)) {
        DEBUG("icmpv4_echo: no receivers for IPv4 packets\n");
        gnrc_pktbuf_release(pkt);
    }
}
