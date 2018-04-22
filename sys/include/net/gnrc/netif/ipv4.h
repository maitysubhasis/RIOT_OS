/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netif
 * @{
 *
 * @file
 * @brief   IPv6 defintions for @ref net_gnrc_netif
 *
 * @author  Subhasis Maity<cs16m055@smail.iitm.ac.in>
 */
#ifndef NET_GNRC_NETIF_IPV4_H
#define NET_GNRC_NETIF_IPV4_H

#include <assert.h>

#include "evtimer_msg.h"
#include "net/ipv4/addr.h"
// #ifdef MODULE_GNRC_IPV6_NIB
// #include "net/gnrc/ipv6/nib/conf.h"
// #endif
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif/conf.h"
#ifdef MODULE_NETSTATS_IPV4
#include "net/netstats.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   IPv4 component for @ref gnrc_netif_t
 *
 * @note only available with @ref net_gnrc_ipv6.
 */
typedef struct {
#ifdef MODULE_NETSTATS_IPV4
/**
 * @brief IPv4 packet statistics
 *
 * @note    Only available with module `netstats_ipv4`.
 */
    netstats_t stats;
#endif
    
    /**
     * @brief   IPv4 unicast and anycast addresses of the interface
     *
     * @note    Only available with module @ref net_gnrc_ipv6 "gnrc_ipv4".
     */
    ipv4_addr_t addr;

    /**
     * @brief   Maximum transmission unit (MTU) for IPv4 packets
     *
     * @note    Only available with module @ref net_gnrc_ipv4 "gnrc_ipv4".
     */
    uint16_t mtu;
} gnrc_netif_ipv4_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_IPV6_H */
/** @} */
