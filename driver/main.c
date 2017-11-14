#include <stdio.h>
#include <math.h>
#include "bit.h"
#include <unistd.h>
#include "huffman.h"
#include <sys/stat.h>
#include <stdlib.h>
/* for test
14 16 23 28 28 29 28 28
12 15 20 22 26 27 28 27
20 22 28 32 30 30 28 30
31 32 34 33 32 31 30 32
30 34 33 36 34 32 32 34
32 36 15 34 32 30 29 31
34 33 20 32 30 28 26 28
35 32 22 26 26 26 25 27

225.00000	-9.46954	-4.66184	2.04176		6.50000		-2.38346	-5.37515	-6.86317
-14.48256	-19.11103	-12.49108	-8.06072	-5.69550	3.46656		7.13329		6.34616
-20.92540	-4.44911	1.46599		4.03180		0.53440		0.92037		0.37500		0.19495
2.46405		-3.16559	-0.91044	2.51832		2.45798		-1.32196	-5.37659	-4.05686
8.25000		4.38959		0.60685	-	3.71515		-2.75000	1.03762		4.07820		3.49678
4.78212		4.03389		-1.01514	0.27638		1.96884		0.76506		0.53238		-1.28170
1.09084		-0.25185	-0.37500	0.92927		1.94343		-0.23218	-1.71599	-3.01398
3.38235		-0.55549	-3.93310	-3.35045	-0.13518	1.46225		2.25150		0.82765
 */

#define CC(n) ((n)==0?0.7071067811865476:1)

int FDCT(unsigned char in[8][8], double out[8][8]);

int z_scan(int in[8][8], int out[63]);

int quantization(double in[8][8], int out[8][8], int isY);

void Huffman(int ZZ0, int ZZ[63], int isY);

int getFileData(unsigned char **data);

void compress(unsigned char *data);

void compressBlock(unsigned char in[8][8], int isY);


// TODO test
int PRED[3] = {0, 0, 0};

//int main() {
//    int i, j, ZZ0;
//    char in[8][8] = {14, 16, 23, 28, 28, 29, 28, 28, 12, 15, 20, 22, 26, 27, 28, 27, 20, 22, 28, 32, 30, 30, 28, 30, 31,
//                     32, 34, 33, 32, 31, 30, 32, 30, 34, 33, 36, 34, 32, 32, 34, 32, 36, 15, 34, 32, 30, 29, 31, 34, 33,
//                     20, 32, 30, 28, 26, 28, 35, 32, 22, 26, 26, 26, 25, 27};
//    double out[8][8];
//    int out2[8][8];
//    int out3[64];
//    FDCT(in, out);
//    for (i = 0; i < 8; i++) {
//        for (j = 0; j < 8; j++) {
//            printf("%lf ", out[i][j]);
//        }
//        putchar('\n');
//    }
//
//    quantization(out, out2, 0);
//
//    for (i = 0; i < 8; i++) {
//        for (j = 0; j < 8; j++) {
//            printf("%02d ", out2[i][j]);
//        }
//        putchar('\n');
//    }
//    printf("%d\n", ZZ0 = z_scan(out2, out3));
//    for (i = 0; i < 63; i++) {
//        printf("%d ", out3[i]);
//    }
//    Huffman(ZZ0, out3, 0);
//    int size;
//    unsigned char *buf;
//    buf = get_buffer(&size);
//    putchar('\n');
//    for (i = 0; i < size; i++) {
//        unsigned char c = buf[i];
//        for (j = 0; j < 8; j++) {
//            if (c & 0x80) {
//                putchar('1');
//            } else {
//                putchar('0');
//            }
//            c <<= 1;
//        }
//    }
//}

int main(void) {
    unsigned char *bmp;
    int size;
    unsigned char *buffer;
    size = getFileData(&bmp);
    compress(bmp + 0x36);
    buffer = get_buffer(&size);
    write(STDOUT_FILENO, bmp, 0x36);
    write(STDOUT_FILENO, buffer, size);
}

void compressBlock(unsigned char in[8][8], int isY) {
    double out[8][8];
    int out2[8][8];
    int out3[64];
    int ZZ0;

    FDCT(in, out);
    quantization(out, out2, isY);
    ZZ0 = z_scan(out2, out3);
    Huffman(ZZ0, out3, isY);
}

void compress(unsigned char *data) {
    unsigned char Y[8][8], U[8][8], V[8][8];
    double tmp;
    int i, j, n, m, tmp2;
    for (n = 0; n < 128; n++) {
        for (m = 0; m < 128; m++) {
            for (i = 0; i < 8; i++) {
                for (j = 0; j < 8; j++) {

                    tmp = 0.299 * *(data + (n * 8 + i) * 0xc00 + (m * 8 + j) * 3 + 0) +
                          0.587 * *(data + (n * 8 + i) * 0xc00 + (m * 8 + j) * 3 + 1) +
                          0.114 * *(data + (n * 8 + i) * 0xc00 + (m * 8 + j) * 3 + 2);
                    tmp2 = (int) tmp;
                    if (tmp > 0 && tmp - tmp2 > 0.5) {
                        tmp2 += 1;
                    } else if (tmp < 0 && tmp - tmp2 < -0.5) {
                        tmp2 -= 1;
                    }
                    Y[i][j] = (unsigned char) (tmp2 - 128);

                    tmp = -0.1687 * *(data + (n * 8 + i) * 0xc00 + (m * 8 + j) * 3 + 0) -
                          0.3313 * *(data + (n * 8 + i) * 0xc00 + (m * 8 + j) * 3 + 1) +
                          0.5 * *(data + (n * 8 + i) * 0xc00 + (m * 8 + j) * 3 + 2);
                    tmp2 = (int) tmp;
                    if (tmp > 0 && tmp - tmp2 > 0.5) {
                        tmp2 += 1;
                    } else if (tmp < 0 && tmp - tmp2 < -0.5) {
                        tmp2 -= 1;
                    }
                    U[i][j] = (unsigned char) (tmp2);

                    tmp = 0.5 * *(data + (n * 8 + i) * 0xc00 + (m * 8 + j) * 3 + 0) -
                          0.4187 * *(data + (n * 8 + i) * 0xc00 + (m * 8 + j) * 3 + 1) +
                          -0.0813 * *(data + (n * 8 + i) * 0xc00 + (m * 8 + j) * 3 + 2);
                    tmp2 = (int) tmp;

                    if (tmp > 0 && tmp - tmp2 > 0.5) {
                        tmp2 += 1;
                    } else if (tmp < 0 && tmp - tmp2 < -0.5) {
                        tmp2 -= 1;
                    }
                    V[i][j] = (unsigned char) (tmp2);
                }
            }
            compressBlock(Y, 0);
            compressBlock(U, 1);
            compressBlock(V, 2);
        }
        PRED[0] = 0;
        PRED[1] = 0;
        PRED[2] = 0;
    }
}

int getFileData(unsigned char **data) {
    struct stat statbuf;
    size_t size;
    char *fileName = "/mnt/hgfs/preject/hctf/driver/timg.bmp";
    FILE *fp = fopen(fileName, "r");
    stat(fileName, &statbuf);
    size = (size_t) statbuf.st_size;
    if (size != 0x300036) {
        puts("error");
        exit(1);
    }
    *data = malloc(size);
    fread(*data, size, 1, fp);
    fclose(fp);
    return (int) size;

}

double cosTable[] = {1.0, 0.980785280403, 0.923879532511, 0.831469612303, 0.707106781187, 0.55557023302, 0.382683432365,
                     0.195090322016, 0.0, -0.195090322016, -0.382683432365, -0.55557023302, -0.707106781187,
                     -0.831469612303, -0.923879532511, -0.980785280403, 0.0, 0.0, -0.923879532511, 0.0, -0.707106781187,
                     -0.55557023302, -0.382683432365, 0.0, 0.0, 0.195090322016, 0.382683432365, 0.55557023302,
                     0.707106781187, 0.0, 0.923879532511, 0.0, 0.0, 0.980785280403, 0.0, 0.831469612303, 0.707106781187,
                     0.0, 0.0, 0.195090322016, 0.0, 0.0, -0.382683432365, 0.0, -0.707106781187, -0.831469612303, 0.0,
                     0.0, 0.0, -0.980785280403, 0.0, 0.0, -0.707106781187, 0.0, -0.382683432365, -0.195090322016, 0.0,
                     0.0, 0.0, 0.0, 0.707106781187, 0.0, 0.0, 0.980785280403, 0.0, 0.980785280403, 0.923879532511, 0.0,
                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.55557023302, 0.0, -0.831469612303, -0.923879532511, 0.0, 0.0,
                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.382683432365, 0.55557023302, 0.0, 0.0, 0.0, 0.0,
                     0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.195090322016};

#define PI 3.141592653589793

int FDCT(unsigned char in[8][8], double out[8][8]) {
    int k, l, m, n;
    double tmp;
    double t1, t2;
    for (k = 0; k < 8; k++) {
        for (l = 0; l < 8; l++) {
            tmp = 0;
            for (m = 0; m < 8; m++) {
                for (n = 0; n < 8; n++) {
                    t1 = (2 * m + 1) * k;
                    t1 = t1 / 16;
                    t1 = t1 * PI;
                    t1 = cos(t1);

                    t2 = (2 * n + 1) * l;
                    t2 = t2 / 16;
                    t2 = t2 * PI;
                    t2 = cos(t2);
                    tmp += in[m][n] * t1 * t2;
                }
            }
            tmp = tmp * CC(k) * CC(l) / 4;
            out[k][l] = tmp;
        }
    }
    return 0;
}

int YQut[8][8] = {16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55, 14, 13, 16, 24, 40, 57, 69, 56, 14,
                  17, 22, 29, 51, 87, 80, 62, 18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 49,
                  64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99};
int UQut[8][8] = {17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99, 24, 26, 56, 99, 99, 99, 99, 99, 47,
                  66, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
                  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99};

int quantization(double in[8][8], int out[8][8], int isY) {
    int i, j;
    double tmp;
    int tmp2;
    int (*qutTable)[8];
    if (!isY) {
        qutTable = YQut;
    } else {
        qutTable = UQut;
    }

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            tmp = in[i][j] / (qutTable[i][j]);
            tmp2 = (int) tmp;

            if (tmp > 0 && tmp - tmp2 > 0.5) {
                tmp2 += 1;
            } else if (tmp < 0 && tmp - tmp2 < -0.5) {
                tmp2 -= 1;
            }
            out[i][j] = tmp2;
        }
    }
    return 0;
}

int z_scan(int in[8][8], int out[63]) {
    int i, j, t, flag = 1, c = 0;
    for (t = 1; t < 8; t++) {
        if (flag) {
            for (i = t; i >= 0; i--) {
                out[c++] = in[t - i][i];
            }
            flag = 0;
        } else {
            for (i = t; i >= 0; i--) {
                out[c++] = in[i][t - i];
            }
            flag = 1;
        }
    }
    for (t = 1, flag = 1; t < 8; t++) {
        if (flag) {
            for (i = t; i < 8; i++) {
                out[c++] = in[7 + t - i][i];
            }
            flag = 0;
        } else {
            for (i = t; i < 8; i++) {
                out[c++] = in[i][7 + t - i];
            }
            flag = 1;
        }
    }
    return in[0][0];
}


void Huffman(int ZZ0, int ZZ[63], int isY) {
    int diff = ZZ0 - PRED[isY];
    int tmp = diff > 0 ? diff : -diff;
    int i, n, size, last;

    for (i = 0; tmp; i++) {
        tmp = tmp >> 1;
    }
    if (!isY) {
        add_to_array(s_lumin_dc[i][1], s_lumin_dc[i][0]);
    } else {
        add_to_array(s_chrom_dc[i][1], s_chrom_dc[i][0]);
    }
    tmp = diff > 0 ? diff : ~(-diff);
    add_to_array(tmp, i);
    PRED[isY] = ZZ0;

    for (last = 62; last >= 0 && ZZ[last] == 0; last--);
    for (i = 0, n = 0; i <= last; i++) {
        if (ZZ[i] == 0) {
            n++;
            if (n == 16) {
                if (!isY) {
                    add_to_array(s_lumin_ac[16 * 15][1], s_lumin_ac[16 * 15][0]);
                } else {
                    add_to_array(s_chrom_ac[16 * 15][1], s_chrom_ac[16 * 15][0]);
                }
                n = 0;
            }
            continue;
        }
        tmp = ZZ[i] > 0 ? ZZ[i] : -ZZ[i];
        for (size = 0; tmp; size++) {
            tmp = tmp >> 1;
        }
        if (!isY) {
            add_to_array(s_lumin_ac[16 * n + size][1], s_lumin_ac[16 * n + size][0]);
        } else {
            add_to_array(s_chrom_ac[16 * n + size][1], s_chrom_ac[16 * n + size][0]);
        }
        tmp = ZZ[i] > 0 ? ZZ[i] : ~(-ZZ[i]);
        add_to_array(tmp, size);
        n = 0;
    }
    if (!isY) {
        add_to_array(s_lumin_ac[0][1], s_lumin_ac[0][0]);
    } else {
        add_to_array(s_chrom_ac[0][1], s_chrom_ac[0][0]);
    }
}


