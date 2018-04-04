/*
 * Copyright (C) 2018 Amit Verma <cs.amit.v@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_icmp  ICMPV4
 * @ingroup     net_ipv4
 * @brief       Provides types related to ICMPv4
 * @see         <a href="https://tools.ietf.org/html/rfc792">
 *                  RFC 792
 *              </a>
 * @{
 *
 * @file
 * @brief   ICMPv4 type and function definitions
 *
 * @author  Amit Verma <cs.amit.v@gmail.com>
 */
#ifndef NET_ICMP_H
#define NET_ICMP_H

#include<stdint.h>

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @{
 * @name Error message types
 * @see <a href="https://www.iana.org/assignments/icmp-parameters/icmp-parameters.xhtml#icmp-parameters-types">
 *          IANA, ICMP "type" Numbers
 *      </a>
 */

#define ICMP_ER   (0)    /* echo reply */
#define ICMP_DST_UNR   (3) /**< Destination unreachable message */
#define ICMP_SQ   (4)    /* source quench */
#define ICMP_RD   (5)    /* redirect */
#define ICMP_ECHO (8)    /* echo */
#define ICMP_TE  (11)    /* time exceeded */
#define ICMP_PP  (12)    /* parameter problem */
#define ICMP_TS  (13)    /* timestamp */
#define ICMP_TSR (14)    /* timestamp reply */
#define ICMP_IRQ (15)    /* information request */
#define ICMP_IR  (16)    /* information reply */
#define ICMP_ADDRESS		17	/* Address Mask Request		*/
#define ICMP_ADDRESSREPLY	18	/* Address Mask Reply		*/
/**
 * @}
 */


/**
 * @{
 * @name Codes for destination unreachable messages
 *
 * @see <a href="https://tools.ietf.org/html/rfc792">
 *          RFC 792, Page 4
 *      </a>
 */
#define ICMP_ERROR_DST_UNR_NET_UNR   (0) /**< net unreachable */
#define ICMP_ERROR_DST_UNR_HOST_UNR     (1) /**< host unreachable */
#define ICMP_ERROR_DST_UNR_PROTO_UNR      (2) /**< beyond scope of source address */
#define ICMP_ERROR_DST_UNR_PORT_UNR       (3) /**< address unreachable */
#define ICMP_ERROR_DST_UNR_FRAG_NEED_DF_SET       (4) /**< port unreachable */
#define ICMP_ERROR_DST_UNR_SRC_ROUTE_FAILED     (5) /**< source address failed ingress/egress */
#define ICMP_NET_UNKNOWN	(6)  /**< Destination network unknown */
#define ICMP_HOST_UNKNOWN	(7)  /**< Destination host unknown */
#define ICMP_HOST_ISOLATED	(8) /**< Source host Isolated */
#define ICMP_NET_ANO		(9) /** < Communication with Destination Network is Administratively Prohibited */
#define ICMP_HOST_ANO		(10) /** < Communication with Destination Host is Administratively Prohibited */
#define ICMP_NET_UNR_TOS	(11) /** < Destination Network Unreachable for Type of Service */
#define ICMP_HOST_UNR_TOS	(12) /** < Destination Host Unreachable for Type of Service */
#define ICMP_PKT_FILTERED	(13)	/* Communication Administratively Prohibited */
#define ICMP_PREC_VIOLATION	(14)	/*  	Host Precedence Violation */
#define ICMP_PREC_CUTOFF	(15)	/* Precedence cut off */
                                     

/**
 * @}
 */

/**
 * @{
 * @name Codes for time exceeded messages
 *
 * @see <a href="https://tools.ietf.org/html/rfc792>
 *          RFC 792, Page 6
 *      </a>
 */
#define ICMP_ERROR_TTL_EXCEED    (0) /**< time to live exceeded in transit */
#define ICMP_ERROR_FRAG_EXCEED   (1) /**< fragment reassembly time exceeded */
/**
 * @}
 */

/**
 * @{
 * @name Codes for parameter problem messages
 *
 * @see <a href="https://tools.ietf.org/html/rfc792">
 *          RFC 792, Page 8
 *      </a>
 */
#define ICMP_ERROR_PARAM_PROB_POINTER_ERROR   (0) /**<  	Pointer indicates the error  */
#define ICMP_ERROR_PARAM_PROB_OPTION_MISS         (1) /**< Missing a Required Option */
#define ICMP_ERROR_PARAM_PROB_LEN        (2) /**< Bad Length */
/**
 * @}
 */


/**
 * @{
 * @name Codes for Redirect
 *
 * @see <a href="https://tools.ietf.org/html/rfc792">
 *          RFC 792, Page 12
 *      </a>
 */
#define ICMP_REDIR_NET		0	/* Redirect datagrams for the Network.	*/
#define ICMP_REDIR_HOST		1	/* Redirect datagrams for the Host.		*/
#define ICMP_REDIR_NET_TOS	2	/* Redirect datagrams for the Type of Service and Network.	*/
#define ICMP_REDIR_HOST_TOS	3	/* Redirect datagrams for the Type of Service and Host.	*/

/**
 * @}
 */




/**
 * @brief   Echo request and response message format.
 *
 * @see <a href="https://tools.ietf.org/html/rfc792#page-14">
 *          RFC 792, page 14-15
 *      </a>
 */
typedef struct __attribute__((packed)){
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    network_uint16_t id;    /**< identifier */
    network_uint16_t sn;    /**< sequence number */
} icmp_echo_t;



/**
 * @brief  Destination unreachable message format.
 *
 * @see <a href="https://tools.ietf.org/html/rfc792#page-4">
 *          RFC 792, page 4
 *      </a>
 */
typedef struct __attribute__((packed)){
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    network_uint32_t unused /** < unused field */
} icmp_dst_unr_t;





/**
 * @brief   Time exceeded message format.
 *
 * @see <a href="https://tools.ietf.org/html/rfc792#page-6">
 *          RFC 792, page 6
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               /**< message type */
    uint8_t code;               /**< message code */
    network_uint16_t csum;      /**< checksum */
    network_uint32_t unused;    /**< unused field */
} icmp_error_time_exc_t;


/**
 * @brief   Source Quench message format.
 *
 * @see <a href="https://tools.ietf.org/html/rfc792#page-10">
 *          RFC 792, page 10
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               /**< message type */
    uint8_t code;               /**< message code */
    network_uint16_t csum;      /**< checksum */
    network_uint32_t unused;    /**< unused field */
} icmp_error_sq_t;


/**
 * @brief   Redirect message format.
 *
 * @see <a href="https://tools.ietf.org/html/rfc792#page-12">
 *          RFC 792, page 12
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               /**< message type */
    uint8_t code;               /**< message code */
    network_uint16_t csum;      /**< checksum */
    network_uint32_t gateway_address;    /**< Gateway Internet Address  */
} icmp_error_redirect_t;



/**
 * @brief   Timestamp and reply message format.
 *
 * @see <a href="https://tools.ietf.org/html/rfc792#page-16">
 *          RFC 792, page 16
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               /**< message type */
    uint8_t code;               /**< message code */
    network_uint16_t csum;      /**< checksum */
    network_uint16_t id;        /**< indentifier */
    network_uint16_t sn;        /**< sequence number */
    network_uint32_t OT;        /**< Originate Timestamp */
    network_uint32_t RT;        /**< Receive Timestamp */
    network_uint32_t TT;        /**< Transmit Timestamp */
} icmp_error_timestamp_t;





/**
 * @brief   Parameter problem message format.
 *
 * @see <a href="https://tools.ietf.org/html/rfc792#page-8">
 *          RFC 792, Page 8
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    uint8_t ptr;   /**< pointer */
    network_uint24_t unused /**< unused */
} icmpv6_error_param_prob_t;



/**
 * @brief   Info request and reply message format.
 *
 * @see <a href="https://tools.ietf.org/html/rfc792#page-18">
 *          RFC 792, page 18
 *      </a>
 */
typedef struct __attribute__((packed)){
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    network_uint16_t id;    /**< identifier */
    network_uint16_t sn;    /**< sequence number */
} icmp_info_req_t;



#ifdef __cplusplus
}
#endif

#endif /* NET_ICMP_H */
/** @} */
