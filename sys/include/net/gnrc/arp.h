#ifndef NET_GNRC_ARP_H
#define NET_GNRC_ARP_H

#include "kernel_types.h"
#include "net/gnrc.h"
#include "thread.h"
#include "net/arp/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	ipv4_addr_t ip_addr;
	uint8_t l2_addr[GNRC_NETIF_L2ADDR_MAXLEN];
} arp_table_entry_t;

#define GNRC_ARP_TABLE_SIZE        (5U)

typedef struct {
	uint8_t size;
	arp_table_entry_t table[GNRC_ARP_TABLE_SIZE];
} arp_table_t;

extern arp_table_t arp_table;


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

void update_arp_table(ipv4_addr_t * ip_addr, uint8_t * l2_addr);

arp_hdr_t *gnrc_arp_get_header(gnrc_pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif

#endif