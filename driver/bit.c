//
// Created by explorer on 17-10-26.
//

#include "bit.h"
#include <stdlib.h>
#include <string.h>

unsigned char *array_buffer;
int buffer_count;
int buffer_len;
int bit_tmp;
int tmp_len;
unsigned char bit_mask[] = {0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff};

void bit_add(int bit, int len);

void add_to_array(int bit, int len) {
    int i;
    if (len <= 8) {
        bit_add(bit, len);
        return;
    }
    if (len % 8) {
        bit_add(bit >> ((len / 8) * 8), len % 8);
    }


    for (i = len / 8 - 1; i >= 0; i--) {
        bit_add((bit >> (i * 8)) & 0xff, 8);
    }
}

void bit_add(int bit, int len) {
    if (tmp_len + len >= 8) {
        bit_tmp = bit_tmp << (8 - tmp_len);
        bit_tmp += (bit >> (len - (8 - tmp_len))) & bit_mask[8 - tmp_len];
        if (buffer_len < buffer_count + 1) {
            array_buffer = realloc(array_buffer, (size_t) (buffer_len + 0x100));
            buffer_len += 0x100;
        }
        array_buffer[buffer_count] = (unsigned char) bit_tmp;
        buffer_count++;
        bit_tmp = bit & bit_mask[len - (8 - tmp_len)];
        tmp_len = len - (8 - tmp_len);
    } else {
        bit_tmp = bit_tmp << len;
        bit_tmp += bit & bit_mask[len];
        tmp_len += len;
    }
}

unsigned char *get_buffer(int *size) {
    unsigned char *data = malloc((size_t) (buffer_count + (tmp_len == 0 ? 0 : 1)));
    memcpy(data, array_buffer, (size_t) buffer_count);
    *size = buffer_count;
    if (tmp_len) {
        data[buffer_count] = (unsigned char) (bit_tmp << (8 - tmp_len));
        *size += 1;
    }
    return data;
}