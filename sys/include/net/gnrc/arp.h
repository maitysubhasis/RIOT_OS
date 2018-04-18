#ifndef NET_GNRC_ARP_H
#define NET_GNRC_ARP_H

#include "kernel_types.h"
#include "net/gnrc.h"
#include "thread.h"
#include "net/arp/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GNRC_ARP_STACK_SIZE
#define GNRC_ARP_STACK_SIZE        (THREAD_STACKSIZE_DEFAULT)
#endif

#ifndef GNRC_ARP_PRIO
#define GNRC_ARP_PRIO              (THREAD_PRIORITY_MAIN - 5)
#endif

/**
 * @brief   Default message queue size to use for the IPv6 thread.
 */
#ifndef GNRC_ARP_MSG_QUEUE_SIZE
#define GNRC_ARP_MSG_QUEUE_SIZE    (8U)
#endif



extern kernel_pid_t gnrc_arp_pid;

kernel_pid_t gnrc_arp_init(void);

arp_hdr_t *gnrc_arp_get_header(gnrc_pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif

#endif