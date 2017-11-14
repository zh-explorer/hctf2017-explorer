//gcc library.c ip.c -fpic -shared -fno-stack-protector -g -O2 -o a.so
#include <string.h>
#include "library.h"

char *readn(int *len);

void init(void);

typedef struct IPS {
    ip_t *buffer;
    struct IPS *next;
    int dataLen;
} IPS;

typedef enum {
    error,
    readpacks,
    checkhead,
    addtolist,
    decodeUDPStart,
    dropPack,
    addlist,
    checkcomplete,
    clearLast
} State;

IPS *head;

void LauncherMain(int argc, char **argv) {
    init();
    IPS *p, *p2;
    IPS *prev = NULL;
    State s = readpacks;
    int offset;
    int dataLength;
    int totalLen;
    udp_t udpPack;
    uint16_t checksum;

    int packLength = 0;
    ip_t *tmp = NULL;
    while (1) {
        switch (s) {
            case readpacks:
                tmp = (ip_t *) readn(&packLength);
                if (tmp == NULL) {
                    s = error;
                    break;
                }
                s = checkhead;
                break;
            case checkhead:
                if (tmp->ip_version != IP_V4) {
                    s = dropPack;
                    break;
                } else if (tmp->ip_hdr_len != 5) {
                    s = dropPack;
                    break;
                } else if (tmp->ip_ttl == 0) {
                    s = dropPack;
                    break;
                } else if (tmp->ip_proto != IPPROTO_UDP) {
                    s = dropPack;
                    break;
                }
                checksum = tmp->ip_chk;
                tmp->ip_chk = 0;
                if (checksum != ip_checksum(tmp, tmp->ip_hdr_len * 4)) {
                    s = dropPack;
                    break;
                }
                tmp->ip_chk = checksum;
                s = addtolist;
                break;
            case dropPack:
                s = readpacks;
                free(tmp);
                tmp = NULL;
                break;
            case addtolist:
                if (tmp->ip_off & IP_FLAG_DF) {
                    if (head == NULL) {
                        head = malloc(sizeof(IPS));
                        head->buffer = tmp;
                        head->next = NULL;
                        head->dataLen = packLength - tmp->ip_hdr_len * 4 - 4;
                        s = decodeUDPStart;
                        break;
                    } else {
                        s = dropPack;
                        break;
                    }
                } else {
                    s = addlist;
                    break;
                }
                break;
            case addlist:
                if (head == NULL) {
                    head = malloc(sizeof(IPS));
                    head->buffer = tmp;
                    head->next = NULL;
                    head->dataLen = packLength - tmp->ip_hdr_len * 4 - 4;
                } else {
                    if (head->buffer->ip_len != tmp->ip_len) {
                        s = dropPack;
                        break;
                    } else if (head->buffer->ip_id != tmp->ip_id) {
                        s = dropPack;
                        break;
                    } else if (head->buffer->ip_src != tmp->ip_src) {
                        s = dropPack;
                        break;
                    } else if (head->buffer->ip_dst != tmp->ip_dst) {
                        s = dropPack;
                        break;
                    }
                    offset = tmp->ip_off & IP_FLAG_MASK;
                    dataLength = packLength - tmp->ip_hdr_len * 4 - 4;
                    for (p = head, prev = NULL; p; p = p->next) {
                        if (p->buffer->ip_off & IP_FLAG_MASK > offset) {
                            if (prev &&
                                (prev->buffer->ip_off & IP_FLAG_MASK) + prev->dataLen > tmp->ip_off & IP_FLAG_MASK) {
                                goto brk;
                            }
                            if ((tmp->ip_off & IP_FLAG_MASK) + dataLength > p->buffer->ip_off & IP_FLAG_MASK) {
                                goto brk;
                            }
                            if (tmp->ip_off & IP_FLAG_MF == 0) {
                                goto brk;
                            }
                            p2 = malloc(sizeof(IPS));
                            p2->buffer = tmp;
                            p2->dataLen = dataLength;
                            p2->next = p;
                            prev->next = p2;
                            break;
                        }
                        prev = p;
                    }
                    if (p == NULL) {
                        if (prev->buffer->ip_off & IP_FLAG_MF == 0) {
                            goto brk;
                        }
                        if ((prev->buffer->ip_off & IP_FLAG_MASK) + prev->dataLen > tmp->ip_off & IP_FLAG_MASK) {
                            goto brk;
                        }
                        p2 = malloc(sizeof(IPS));
                        p2->buffer = tmp;
                        p2->dataLen = dataLength;
                        p2->next = NULL;
                        prev->next = p2;
                    }
                }
                s = checkcomplete;
                break;
            brk:
                s = dropPack;
                break;
            case checkcomplete:
                if ((head->buffer->ip_off & IP_FLAG_MASK) != 0) {
                    goto brk2;
                }
                totalLen = 0;
                for (p = head; p->next; p = p->next) {
                    if ((p->buffer->ip_off & IP_FLAG_MASK) + p->dataLen != (p->next->buffer->ip_off & IP_FLAG_MASK)) {
                        goto brk2;
                    }
                    totalLen += p->dataLen;
                    if (totalLen > p->buffer->ip_len) {
                        s = error;
                        goto err;
                    }
                }
                totalLen += p->dataLen;
                if (totalLen != p->buffer->ip_len) {
                    goto brk2;
                }
                if (p->buffer->ip_off & IP_FLAG_MF) {
                    s = error;
                    goto err;
                } else {
                    s = decodeUDPStart;
                    break;
                }
            brk2:
                s = readpacks;
            err:
                break;
            case decodeUDPStart:
                for (p = head; p; p = p->next) {
                    memcpy((char *)&udpPack + (p->buffer->ip_off & IP_FLAG_MASK), p->buffer->data, (size_t) p->dataLen);
                }
                s = clearLast;
                break;
            case clearLast:
                prev = head;
                for (p = head->next; p; p = p->next) {
                    free(prev->buffer);
                    free(prev);
                    prev = p;
                }
                free(prev->buffer);
                free(prev);
                head = NULL;
                s = readpacks;
                break;
            case error:
                asm("mov rax,0xdeadbeefdeadbeef");
                asm("mov rdi,rax");
                asm("mov rsi,rax");
                asm("mov rbx,rax");
                asm("mov rcx,rax");
                asm("mov rdx,rax");
                asm("mov r8,rax");
                asm("mov r9,rax");
                asm("mov r10,rax");
                asm("mov r11,rax");
                asm("mov r12,rax");
                asm("mov r13,rax");
                asm("mov r14,rax");
                asm("mov r15,rax");
                return;
        }
    }

}

void init(void) {
    setbuf(stdin, NULL);
    fclose(stdout);
    fclose(stderr);
}

char *readn(int *len) {
    int length;
    int get;
    char *buffer;
    if (read(STDIN_FILENO, &length, 4) != 4) {
        return NULL;
    }
    if (length > 0x1000 || length < IP_HEAD_MIN_LEN) {
        return NULL;
    }
    buffer = malloc((size_t) length);
    if (buffer == NULL) {
        return NULL;
    }

    get = (int) read(STDIN_FILENO, buffer, length);
    if (get != length) {
        free(buffer);
        return NULL;
    }
    if (len) {
        *len = length;
    }
    return buffer;
}
