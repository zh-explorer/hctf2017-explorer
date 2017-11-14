//
// Created by explorer on 17-5-20.
//

#ifndef RSA_RSA_H
#define RSA_RSA_H
__int64_t euclideanAlgorithm(__int64_t a, __int64_t b);
bigInteger * montgomery(bigInteger *a, bigInteger *b, bigInteger *N);
bigInteger * createR(bigInteger *N);
bigInteger * modPow(bigInteger *x, bigInteger *y, bigInteger *N);
#endif //RSA_RSA_H
