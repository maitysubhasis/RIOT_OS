#ifndef NET_GNRC_IPV4_HDR_H
#define NET_GNRC_IPV4_HDR_H

#include "net/gnrc/pkt.h"
#include "net/ipv4/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Builds an IPv4 header for sending and adds it to the packet buffer.
 *
 * @details Initializes version field with 6, ...
 *
 * @param[in] payload   Payload for the packet.
 * @param[in] src       Source address for the header. Can be NULL if not
 *                      known or required.
 * @param[in] dst       Destination address for the header. Can be NULL if not
 *                      known or required.
 *
 * @return  The an IPv4 header in packet buffer on success.
 * @return  NULL on error.
 */
gnrc_pktsnip_t *gnrc_ipv4_hdr_build(gnrc_pktsnip_t *payload, const ipv4_addr_t *src,
                                    const ipv4_addr_t *dst);

#ifdef __cplusplus
}
#endif

#endif
/**
 * 
 */