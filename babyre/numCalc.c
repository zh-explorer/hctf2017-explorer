//
// Created by explorer on 17/5/18.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "numCalc.h"

bigInteger *createBigInteger() {
    bigInteger *newNum = malloc(sizeof(bigInteger));
    memset(newNum, 0, sizeof(bigInteger));
    newNum->prev = (intergerNode *) newNum;
    newNum->next = (intergerNode *) newNum;
    return newNum;
}

intergerNode *createIntergerNode() {
    intergerNode *node = malloc(sizeof(intergerNode));
    memset(node, 0, sizeof(intergerNode));
    node->prev = node;
    node->next = node;
    return node;
}


/*
 * num1 = num1 * num2
 */
void bigMulNum(bigInteger *num1, unsigned int num2) {
    __uint64_t tmp;
    intergerNode *i = NULL;
    intergerNode *p = NULL;
    unsigned int carry = 0;
    for (i = num1->next; i != (intergerNode *) num1; i = i->next) {
        tmp = i->num;
        tmp *= num2;
        tmp += carry;
        i->num = (unsigned int) (tmp & 0xffffffff);
        carry = (unsigned int) (tmp >> 32);
    }
    if (carry) {
        p = createIntergerNode();
        add(num1->prev, p, num1);
        num1->size++;
        p->num = carry;

    }
}

/*
 * num1 = num1 + num2
 */
void bigAddNum(bigInteger *num1, unsigned int num2) {
    __uint64_t tmp;
    intergerNode *i = NULL;
    intergerNode *p = NULL;
    unsigned int carry = 0;
    i = num1->next;
    do {
        tmp = i->num;
        tmp += num2;
        tmp += carry;
        i->num = (unsigned int) (tmp & 0xffffffff);
        carry = (unsigned int) (tmp >> 32);
        if (!carry) {
            break;
        }
        i = i->next;
    } while (i != (intergerNode *) num1);
    if (carry) {
        p = createIntergerNode();
        add(num1->prev, p, num1);
        num1->size++;
        p->num = carry;
    }
}

/*
 * num1 > num2
 * num1 = num1 - num2
 */
void bigSubNum(bigInteger *num1, unsigned int num2) {
    int num;
    intergerNode *i = num1->next;
    intergerNode *p = NULL;
    unsigned int carry = 0;
    do {
        if (i->num > num2) {
            i->num -= num2 + carry;
            carry = 0;
        } else {
            i->num = (unsigned int) (0x100000000 + i->num - num2 - carry);  // should work will while i->num == num2
            carry = 1;
        }
        if (!carry) {
            break;
        }
        i = i->next;
    } while (1);
    if (num1->prev->num == 0 && num1->prev != num1->next) {
        p = num1->prev;
        del(num1->prev->prev, p, num1);
        num1->size--;
        free(p);
    }
    // not need deal with carry because num1 > num2
}

void numClean(bigInteger *num) {
    intergerNode *i, *p = NULL;
    for (i = num->prev; i->num == 0 && i->next != i->prev; i = i->prev) {
        if (p) {
            del(p->prev, p, p->next)
            num->size--;
            free(p);
        }
        p = i;
    }
    if(p){
        del(p->prev, p, p->next)
        num->size--;
        free(p);
    }
}

/*
 * num1 = num1 + num2;
 */
void bigAdd(bigInteger *num1, bigInteger *num2) {
    intergerNode *i1 = num1->next;
    intergerNode *i2 = num2->next;
    __uint64_t tmp;
    int carry = 0, i;
    intergerNode *p = NULL;
    if (num1->size < num2->size) {
        tmp = num2->size - num1->size;
        for (i = 0; i < tmp; i++) {
            p = createIntergerNode();
            add(num1->prev, p, num1);
            num1->size++;
        }
    }

    // add a node for carry
    p = createIntergerNode();
    add(num1->prev, p, num1);
    num1->size++;

    while (i1 != (intergerNode *) num1 && i2 != (intergerNode *) num2) {
        tmp = i1->num;
        tmp += i2->num;
        tmp += carry;
        i1->num = (unsigned int) (tmp & 0xffffffff);
        carry = (int) (tmp >> 32);
        i1 = i1->next;
        i2 = i2->next;
    }
    if (carry) {
        while (i1->num == 0xffffffff) {
            i1->num = 0;
            i1 = i1->next;
        }
        i1->num++;
    }
    numClean(num1);
}

/*
 * num1 > num2
 * num1 = num1 - num2
 */
void bigSub(bigInteger *num1, bigInteger *num2) {
    intergerNode *i1 = num1->next;
    intergerNode *i2 = num2->next;
    intergerNode *i = NULL;
    int carry = 0;
    while (i1 != (intergerNode *) num1 && i2 != (intergerNode *) num2) {
        if (i1->num > i2->num) {
            i1->num -= i2->num + carry;
            carry = 0;
        } else {
            i1->num = (unsigned int) (0x100000000 + i1->num - i2->num - carry);
            carry = 1;
        }
        i1 = i1->next;
        i2 = i2->next;
    }
    if (carry == 1) {
        while (i1->num == 0) {
            i1->num = 0xffffffff;
            i1 = i1->next;
        }
        i1->num--;
    }
    numClean(num1);
}

bigInteger *numCopy(bigInteger *num) {
    bigInteger *new = createBigInteger();
    intergerNode *i, *p;
    intergerNode *prev = (intergerNode *) new;
    new->size = num->size;
    for (i = num->next; i != (intergerNode *) num; i = i->next) {
        p = createIntergerNode();
        p->num = i->num;
        add(prev, p, new);
        prev = p;
    }
    return new;
}

bigInteger *createZero() {
    bigInteger *new = createBigInteger();
    intergerNode *p = createIntergerNode();
    add(new, p, new);
    new->size++;
    return new;
}

void numDelete(bigInteger *num) {
    intergerNode *i, *p = NULL;
    for (i = num->next; i != (intergerNode *) num; i = i->next) {
        free(p);
        p = i;
    }
    free(p);
    free(num);
}

int bigCmp(bigInteger *num1, bigInteger *num2) {
    intergerNode *a, *b;
    int flag;
    if (num1->size != num2->size) {
        return num1->size > num2->size ? 1 : -1;
    }
    for (a = num1->prev, b = num2->prev; a != (intergerNode *) num1; a = a->prev, b = b->prev) {
        if (a->num != b->num) {
            return a->num > b->num ? 1 : -1;
        }
    }
    return 0;
}

bigInteger *arrayToNum(unsigned int *num, int size){
    int i;
    bigInteger *new;
    intergerNode *p;
    new = createBigInteger();
    new->next = (intergerNode *) new;
    new->prev = (intergerNode *) new;
    for(i=0;i<size;i++){
        p = createIntergerNode();

        p->num = num[i];

        p->prev = new->prev;
        p->next = (struct intergerNode *) new;
        new->prev->next = p;
        new->prev = p;
        new->size++;
    }
    return new;
}

//bigInteger *strToNum(char *strNumber) {
//    size_t len = strlen(strNumber);
//    int i;
//    char ch;
//    intergerNode *p = NULL;
//    bigInteger *num = createBigInteger();
//    p = createIntergerNode();
//    add(num, p, num);
//    num->size++;
//    for (i = 0; i < len; i++) {
//        ch = strNumber[i];
//        if (ch <= '9' && ch >= '0') { ;
//            bigMulNum(num, 10);
//            bigAddNum(num, (unsigned int) (ch - '0'));
//        } else {
//            break;
//        }
//    }
//    return num;
//}

void printNum(bigInteger *num){
    intergerNode * i;
	int c = 0;
    for(i=num->next; i != (intergerNode *) num; i = i->next){
        printf("0x%x\t", i->num);
		c++;
		if(c%4 == 0) {
			putchar('\n');
		}
    }
}
