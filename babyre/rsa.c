#include <stdio.h>
#include <stdlib.h>
#include "numCalc.h"

__uint64_t euclideanAlgorithm(__int64_t a, __int64_t b){
    __int64_t i;
    __int64_t x = 0, y = 1;
    __int64_t n = b;
    while (b){
        i = x;
        x = y - x*(a/b);
        y = i;
        i = b;
        b = a%b;
        a = i;
    }
    return (__uint64_t)y % n;
}

bigInteger * montgomery(bigInteger *a, bigInteger *b, bigInteger *N) {
    bigInteger *c = createZero();
    intergerNode *i, *tmp, *p;
    bigInteger *newb, *newN;
    unsigned int q,size, j;
    unsigned int m = (unsigned int) (0x100000000 - euclideanAlgorithm(N->next->num, 0x100000000));

    if (a->size < N->size) {
        size = N->size - a->size;
        for (j = 0; j < size; j++) {
            p = createIntergerNode();
            add(a->prev, p, a);
            a->size++;
        }
    }

    for(i=a->next; i != (intergerNode *) a; i = i->next){
        q = (i->num * b->next->num + c->next->num) *m;
        newb = numCopy(b);
        newN = numCopy(N);
        bigMulNum(newb, i->num);
        bigMulNum(newN, q);
        bigAdd(c, newb);
        bigAdd(c, newN);
        tmp = c->next;
        del(tmp->prev, tmp, tmp->next);
        c->size--;
        free(tmp);
        numDelete(newb);
        numDelete(newN);
    }
    numClean(c);
    if(bigCmp(c,N) == 1){
        bigSub(c,N);
    }
    return c;
}

bigInteger * createR(bigInteger *N){
    bigInteger * new = createZero();
    intergerNode *p, *c;
    new->next->num = 1;
    int i;
    __uint64_t tmp;
    int carry = 0;
    int size = N->size*32*2;
    for(i =0; i < size; i++){

        for(c = new->next, carry = 0; c != (intergerNode *) new; c = c->next){
            tmp = c->num;
            tmp <<= 1;
            c->num = (unsigned int) (tmp + carry);
            if(tmp&0x100000000){
                carry = 1;
            }else {
                carry = 0;
            };
        }
        if(carry){
            p = createIntergerNode();
            p->num = 1;
            add(new->prev, p, new);
            new->size++;
        }

        if(bigCmp(new, N) == 1){
            bigSub(new, N);
        }
    }

    return new;
}

bigInteger * modPow(bigInteger *x, bigInteger *y, bigInteger *N){
    numClean(x);
    numClean(y);
    numClean(N);
    unsigned int tmp = 0, j;
    bigInteger *r = createR(N);
    bigInteger *one = createZero();
    one->next->num = 1;
    bigInteger *montT = montgomery(one, r, N);
    bigInteger *montTmp;
    bigInteger *montX = montgomery(x, r, N);
    intergerNode *i;
    for(i = y->prev; i != (intergerNode *) y; i = i->prev){
        tmp = i->num;
        for(j=0;j<32;j++){
            montTmp = montT;
            montT = montgomery(montT, montT, N);
            numDelete(montTmp);
            if(tmp &0x80000000){
                montTmp = montT;
                montT = montgomery(montT, montX, N);
                numDelete(montTmp);
            }
            tmp <<= 1;
        }
    }

    numDelete(montX);
    montTmp = montgomery(montT, one, N);
    numDelete(montT);
    numDelete(one);
    numDelete(r);
    return montTmp;
}