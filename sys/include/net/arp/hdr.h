#include "byteorder.h"
#include "net/ethernet/hdr.h"
#include "net/gnrc/ipv4.h"

#ifndef NET_ARP_HDR_H
#define NET_ARP_HDR_H

typedef struct __attribute__((packed)) {
	uint16_t htype;
	uint16_t ptype;
	uint8_t hlen; 						/**< hardware address length */
	uint8_t plen; 						/* protocol address length */
	uint16_t oper; 						/* Operation */
	uint8_t src[ETHERNET_ADDR_LEN];     /**< destination address */
	ipv4_addr_t spa;					/**< target hardware address */
    uint8_t tha[ETHERNET_ADDR_LEN];     /**< target hardware address */
	ipv4_addr_t tpa;					/**< target protocol address */
} arp_hdr_t;

#endif