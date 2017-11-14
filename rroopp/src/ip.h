//
// Created by explorer on 17-10-23.
//

#ifndef RROOPP_IP_H
#define RROOPP_IP_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
//! IP version 4.
#define IP_V4                   4
//! IP version 6.
#define IP_V6                   6

//! Maximum IP frame length.
#define IP_FRAME_LEN            65535
//! Minimum IP header length.
#define IP_HEAD_MIN_LEN         20

//! ICMP (Internet Control Message Protocol) packet type.
#define IPPROTO_ICMP            1
//! IGMP (Internet Group Message Protocol) packet type.
#define IPPROTO_IGMP            2
//! TCP (Transmition Control Protocol) packet type.
#define IPPROTO_TCP             6
//! UDP (User Datagram Protocol) packet type.
#define IPPROTO_UDP             17

// Fragment flags
//! More Fragments.
#define IP_FLAG_MF              4
//! Don't Fragment.
#define IP_FLAG_DF              2
//! The CE flag.
#define IP_FLAG_CE              1

//! The flag mask.
#define IP_FLAG_MASK            0xFFF8
//!

uint16_t ip_checksum(const void *buf, int hdr_len);

// forget the little endian
typedef struct ip {
#if __BYTE_ORDER__ == __LITTLE_ENDIAN__
    uint8_t ip_hdr_len:4;   //!< The header length.
        uint8_t ip_version:4;   //!< The IP version.
#else
    uint8_t ip_version:4;   //!< The IP version.
    uint8_t ip_hdr_len:4;   //!< The IP header length.
#endif
    //! Type of Service.
    uint8_t ip_tos;
    //! IP packet length (both data and header).
    uint16_t ip_len;

    //! Identification.
    uint16_t ip_id;

    //! Fragment offset.
    uint16_t ip_off;

    //! Time To Live.
    uint8_t ip_ttl;
    //! The type of the upper-level protocol.
    uint8_t ip_proto;
    //! IP header checksum.
    uint16_t ip_chk;

    //! IP source address (in network format).
    uint32_t ip_src;
    //! IP destination address (in network format).
    uint32_t ip_dst;

    uint32_t Options:24;
    uint8_t padding:8;
    char data[1];
} __attribute__ ((packed)) ip_t;

typedef struct udp {
    uint16_t udp_src_port;
    uint16_t udp_dst_port;
    uint16_t upd_len;
    uint16_t upd_checksum;
    char data[0x3000];
} __attribute__ ((packed)) udp_t;

#endif //RROOPP_IP_H
