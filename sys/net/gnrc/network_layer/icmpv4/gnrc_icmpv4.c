/*
 * Copyright (C) 2018 Amit Verma <cs.amit.v@gmail.com>
 */

/**
 * @ingroup     net_gnrc_icmpv4
 * @{
 *
 * @file
 *
 * @author      Amit Verma <cs.amit.v@gmail.com>
 */


#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>

#include "byteorder.h"
#include "net/ipv4/hdr.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv4/nib.h"
#include "net/protnum.h"
#include "od.h"
#include "utlist.h"

#include "net/gnrc/icmpv4.h"
#include "net/gnrc/icmpv4/echo.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"




gnrc_pktsnip_t *gnrc_icmpv4_build(gnrc_pktsnip_t *next, uint8_t type,
                                  uint8_t code, size_t size)
{
    gnrc_pktsnip_t *pkt;
    icmpv4_hdr_t *icmpv4;

      if ((pkt = gnrc_pktbuf_add(next, NULL, size, GNRC_NETTYPE_ICMPV4)) == NULL) {
        DEBUG("icmpv4: no space left in packet buffer\n");
        return NULL;
    }

    DEBUG("icmpv4: Building ICMPv4 message with type=%u, code=%u\n",
          type, code);
    icmpv6 = (icmpv4_hdr_t *)pkt->data;
    icmpv6->type = type;
    icmpv6->code = code;
    icmpv6->csum.u16 = 0; /*For computing the checksum, the checksum
      field should be zero.  This checksum may be replaced in the
      future. rfc792 page 2 */

    return pkt;
}


static inline uint16_t calc_icmp_csum(uint16_t *data,
                            int bytes)
{
  uint32_t sum;
  int i;
  sum = 0;
  for (i=0;i<bytes/2;i++) {
    sum += data[i];}
  sum = (sum & 0xFFFF) + (sum >> 16);
  sum = htons(0xFFFF - sum);
return sum;}