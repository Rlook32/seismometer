#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#define PI2 6.28318530717958647692

#define R 18
#define N (1<<R)
#define N1 (1<<(R-1))
#define ll long long int

typedef double complex cmplx;

void printCmplx(cmplx c) {
    printf("%f+%fi", creal(c), cimag(c));
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

void brp(cmplx *A, int k) {
    int i, index;
    cmplx tmp;
    for (i = 0; i < (1<<k); i++) {
        index = (__builtin_bitreverse32(i))>>(32-k);
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

cmplx CC[N];

void convolve(int *A, int *B, int *C, int n){
    int i;
    initOmega();
    for (i = 0; i <= n; i++) {
        CC[i] = A[i] + B[i] * I;
    }
    fft(CC, R);
    CC[0] = conj(creal(CC[0]) * cimag(CC[0]));
    CC[N1] = conj(creal(CC[N1]) * cimag(CC[N1]));
    for (i = 1; i < (N1); i++) {
        CC[N-i] = (CC[i] + conj(CC[N-i])) * (CC[i] - conj(CC[N-i])) / 4 / I;
        CC[i] = conj(CC[N-i]);
    }
    fft(CC, R);
    for (i = 0; i <= 2 * n; i++) {
        C[i] = ((ll)round(creal(CC[i]))>>R);
    }
}
