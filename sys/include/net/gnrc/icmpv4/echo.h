/*
 * Copyright (C) 2018 Amit Verma <cs.amit.v@gmail.com>
 *
 */

/**
 * @defgroup    net_gnrc_icmpv4_echo  ICMPv4 echo messages
 * @ingroup     net_gnrc_icmpv4
 * @brief       ICMPv4 echo request and reply
 * @{
 *
 * @file
 * @brief   ICMPv4 echo message definitions
 *
 * @author  Amit Verma <cs.amit.v@gmail.com>
 */

 #ifndef NET_GNRC_ICMPV4_ECHO_H
#define NET_GNRC_ICMPV4_ECHO_H

#include <inttypes.h>

#include "byteorder.h"
#include "net/gnrc/netif.h"
#include "net/ipv4/hdr.h"

#ifdef __cplusplus
extern "C" {
    #endif


 /**
 * @brief   Builds an ICMPv4 echo message of type @p type for sending.
 *
 * @param[in] type      Type of the echo message. Expected to be either
 *                      ICMP_ECHO or ICMP_ER.
 * @param[in] id        ID for the echo message in host byte-order
 * @param[in] seq       Sequence number for the echo message in host byte-order
 * @param[in] data      Payload for the echo message
 * @param[in] data_len  Length of @p data
 *
 * @return  The echo message on success
 * @return  NULL, on failure
 */  

gnrc_pktsnip_t *gnrc_icmpv4_echo_build(uint8_t type, uint16_t id, uint16_t seq,
                                       uint8_t *data, size_t data_len); 


/**
 * @brief   ICMPv4 echo request handler
 *
 * @param[in] netif     The interface the echo request was received on.
 * @param[in] ipv4_hdr  The IPv4 header of the echo request.
 * @param[in] echo      The Echo Request message.
 * @param[in] len       Length of the echo request message (ipv6_hdr_t::len
 *                      of @p ipv4_hdr minus length of extension headers).
 */
void gnrc_icmpv4_echo_req_handle(gnrc_netif_t *netif, ipv4_hdr_t *ipv4_hdr,
                                 icmpv4_echo_t *echo, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_ICMPV6_ECHO_H */
/** @} */
