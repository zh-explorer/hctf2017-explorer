//
// Created by explorer on 17-5-18.
//

#ifndef RSA_NUMCALC_H
#define RSA_NUMCALC_H
#define add(PREV, P, NEXT) { \
    (P)->prev = (PREV); \
    (P)->next = (NEXT); \
    (PREV)->next = (P); \
    (NEXT)->prev = (P); \
}

#define del(PREV, P, NEXT) {   \
    (PREV)->next = (NEXT);  \
    (NEXT)->prev = (PREV);  \
    (P)->next = NULL;       \
    (P)->prev = NULL;       \
}

typedef struct intergerNode {
    struct intergerNode *next;
    struct intergerNode *prev;
    unsigned int num;
} intergerNode;


typedef struct bigInteger {
    intergerNode *next;
    intergerNode *prev;
    unsigned int size;
} bigInteger;

bigInteger *createBigInteger();

intergerNode *createIntergerNode();

void bigMulNum(bigInteger *num1, unsigned int num2);

void bigAddNum(bigInteger *num1, unsigned int num2);

bigInteger *strToNum(char *strNumber);

void bigSubNum(bigInteger *num1, unsigned num2);

void bigAdd(bigInteger *num1, bigInteger *num2);

void bigSub(bigInteger *num1, bigInteger *num2);

bigInteger * numCopy(bigInteger * num);

void numDelete(bigInteger *num);

bigInteger *createZero();

int bigCmp(bigInteger *num1, bigInteger *num2);

void numClean(bigInteger *num);

void printNum(bigInteger *num);

bigInteger *arrayToNum(unsigned int *num, int size);
#endif //RSA_NUMCALC_H
