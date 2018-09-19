#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include "defines.h"

#define PI2 6.28318530717958647692
#define ll long long int

typedef double complex cmplx;

void printCmplx(cmplx c) {
    printf("%f+%fi", creal(c), cimag(c));
}

unsigned bitreverse32(unsigned x) {
   x = (x & 0x55555555) <<  1 | (x >>  1) & 0x55555555;
   x = (x & 0x33333333) <<  2 | (x >>  2) & 0x33333333;
   x = (x & 0x0F0F0F0F) <<  4 | (x >>  4) & 0x0F0F0F0F;
   x = (x << 24) | ((x & 0xFF00) << 8) |
       ((x >> 8) & 0xFF00) | (x >> 24);
   return x;
}

cmplx OMEGA[N];
void initOmega(){
    int i, j;
    OMEGA[0] = 1;
    for (i = 0; i < R; i++) {
        OMEGA[1<<i] = cexp(-1 * I * PI2 * (1<<i) / N);
        for (j = (1<<i) + 1; j < (1<<(i+1)); j++) {
            OMEGA[j] = OMEGA[1<<i] * OMEGA[j-(1<<i)];
        }
    }
}

double hcf(double freq) {
    double y2, cy = y2 = freq * freq / 100, res = 1;
    res += 0.694 * cy;
    cy *= y2;
    res += 0.241 * cy;
    cy *= y2;
    res += 0.0557 * cy;
    cy *= y2;
    res += 0.009664 * cy;
    cy *= y2;
    res += 0.00134 * cy;
    cy *= y2;
    res += 0.000155 * cy;
    return 1 / sqrt(res);
}

double FILTER[N1];
void initFilter() {
    int i;
    for (i = 0; i < N1; i++) {
        double freq = i * FREQ / N;
        FILTER[i] = sqrt(1 / freq) * hcf(freq) * sqrt(1-exp(-pow(freq*2, 3)));
    }
}

void brp(cmplx *A, int k) {
    int i, index;
    cmplx tmp;
    for (i = 0; i < (1<<k); i++) {
        index = (bitreverse32(i))>>(32-k);
        if (index <= i) continue;
        tmp = A[index];
        A[index] = A[i];
        A[i] = tmp;
    }
}

void fft(cmplx *A, int k){
    int i, j, l, half, con;
    cmplx tmp, *add, *addh;
    brp(A, k);
    for (i = 1; i <= k; i++) {
        half = 1<<(i-1);
        con = 1<<(k-i);
        add = A;
        addh = A + half;
        for (j = 0; j < con; j++) {
            for(l = 0; l < half; l++) {
                tmp = add[half+l] * OMEGA[l<<(k-i)];
                addh[l] = add[l] - tmp;
                add[l] += tmp;
            }
            add += (half<<1);
            addh += (half<<1);
        }
    }
}

cmplx CC[N1];

// n must be odd
void filtering(double *A){
    int i;
    cmplx tmp1, tmp2;
    initOmega();
    initFilter();
    printf("finished initialize\n");
    for (i = 0; i < N1; i++) {
        CC[i] = A[2*i] + A[2*i+1] * I;
    }
    printf("gfaw\n");
    fft(CC, R1);
    printf("awfhuioe\n");
    for (i = 0; i < N1; i++) {
        tmp1 = (CC[i] + conj(CC[N1-i])) / 2;
        tmp2 = (CC[i] - conj(CC[N1-i])) / -2 * I * OMEGA[i<<1];
        CC[i] = conj((tmp1 + tmp2) * FILTER[i]);
        CC[N1 + i] = conj((tmp1 - tmp2) * FILTER[N1-i-1]);
    }
    printf("fawef\n");
    fft(CC, R1);
    printf("afowe\n");
    for (i = 0; i < N1; i++) {
        A[2*i] = creal(CC[i]);
        A[2*i+1] = -cimag(CC[i]);
    }
}
