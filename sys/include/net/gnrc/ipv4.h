#ifndef NET_GNRC_IPV4_H
#define NET_GNRC_IPV4_H

#include "kernel_types.h"
#include "net/gnrc.h"
#include "thread.h"

#include "net/ipv4.h"
#include "net/gnrc/ipv6/hdr.h"
// #include "net/gnrc/ipv4/ext.h"
// #include "net/gnrc/ipv6/nib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default stack size to use for the IPv6 thread
 */
#ifndef GNRC_IPV4_STACK_SIZE
#define GNRC_IPV4_STACK_SIZE        (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Default priority for the IPv6 thread
 */
#ifndef GNRC_IPV4_PRIO
#define GNRC_IPV4_PRIO              (THREAD_PRIORITY_MAIN - 2)
#endif

/**
 * @brief   Default message queue size to use for the IPv6 thread.
 */
#ifndef GNRC_IPV4_MSG_QUEUE_SIZE
#define GNRC_IPV4_MSG_QUEUE_SIZE    (8U)
#endif


/**
 * @brief   The PID to the IPv4 thread.
 *
 * @note    Use @ref gnrc_ipv4_init() to initialize. **Do not set by hand**.
 *
 * @details This variable is preferred for IPv4 internal communication *only*.
 */
extern kernel_pid_t gnrc_ipv4_pid;

/**
 * @brief   Initialization of the IPv4 thread.
 *
 * @return  The PID to the IPv4 thread, on success.
 * @return  a negative errno on error.
 * @return  -EOVERFLOW, if there are too many threads running already
 * @return  -EEXIST, if IPv4 was already initialized.
 */
kernel_pid_t gnrc_ipv4_init(void);

/**
 * @brief   Demultiplexes a packet according to @p nh.
 *
 * @internal
 *
 * **Do not use outside this module or its submodules!!!**
 * Public access needed for Extension Headers.
 *
 * About `current` and `pkt`:
 *
 *                     current     pkt
 *                     |           |
 *                     v           v
 * IPv6 <- IPv6_EXT <- IPv6_EXT <- UNDEF
 *
 * This situation may happen when the packet has a source routing extension
 * header (RFC 6554), and the packet is forwarded from an interface to another.
 *
 * @param[in] netif     The receiving interface.
 * @param[in] current   A snip to process.
 * @param[in] pkt       A packet.
 * @param[in] nh        A protocol number (see @ref net_protnum) of the current snip.
 */
/* void gnrc_ipv6_demux(gnrc_netif_t *netif, gnrc_pktsnip_t *current,
                     gnrc_pktsnip_t *pkt, uint8_t nh); */

/**
 * @brief   Get the IPv4 header from a given list of @ref gnrc_pktsnip_t
 *
 *          This function may be used with e.g. a pointer to a (full) UDP datagram.
 *
 * @param[in] pkt    The pointer to the first @ref gnrc_pktsnip_t of the
 *                   packet.
 *
 * @return A pointer to the @ref ipv6_hdr_t of the packet.
 * @return NULL if the packet does not contain an IPv4 header.
 */
ipv4_hdr_t *gnrc_ipv4_get_header(gnrc_pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_H */
/**
 * @}
 */
