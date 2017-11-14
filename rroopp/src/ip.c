//
// Created by explorer on 17-10-23.
//

#include "ip.h"
uint16_t ip_checksum(const void *buf, int hdr_len) {
    uint32_t sum = 0;
    const uint16_t *ip1;

    ip1 = buf;
    while (hdr_len > 1) {
        sum += *ip1++;
        if (sum & 0x80000000)
            sum = (sum & 0xFFFF) + (sum >> 16);
        hdr_len -= 2;
    }

    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return (uint16_t) (~sum);
}
