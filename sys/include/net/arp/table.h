#include "byteorder.h"
#include "net/ethernet/hdr.h"
#include "net/gnrc/ipv4.h"
#include "net/gnrc/arp.h"

#ifndef NET_ARP_TABLE_H
#define NET_ARP_TABLE_H

static char addr_str[IPV4_ADDR_MAX_STR_LEN];

extern arp_table_t arp_table;

uint8_t * search_arp_table(ipv4_addr_t * ip_addr)
{
	for (uint8_t i=0; i < arp_table.size; i++) {
		if (ipv4_addr_equal(ip_addr, &(arp_table.table[i].ip_addr))) {
			return arp_table.table[i].l2_addr;
		}
	}
	return NULL;
}


void update_arp_table(ipv4_addr_t * ip_addr, uint8_t * l2_addr)
{
	ipv4_addr_to_str(addr_str, ip_addr, IPV4_ADDR_MAX_STR_LEN);
	uint8_t * l2_addr_found = search_arp_table(ip_addr);
	if (l2_addr_found == NULL && arp_table.size < GNRC_ARP_TABLE_SIZE) {
		arp_table.size++;
		memcpy(&(arp_table.table[arp_table.size].ip_addr), ip_addr, IPV4_ADDR_MAX_STR_LEN);
		memcpy(&(arp_table.table[arp_table.size].l2_addr), l2_addr, ETHERNET_ADDR_LEN);
	}
}

// void print_arp_table(void)
// {
// 	uint8_t * hwaddr;
// 	for (int i = 0; i < arp_table.size; i++) {
// 		hwaddr = arp_table.table[i].l2_addr;
// 		printf("ARP IP: %s: ", ipv4_addr_to_str(addr_str, &(arp_table.table[i].ip_addr),
// 			IPV4_ADDR_MAX_STR_LEN));
// 		printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
//               hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4],
//               hwaddr[5]);
// 	}
// }


#endif