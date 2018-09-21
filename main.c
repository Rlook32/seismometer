#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "defines.h"

#define GRAVITY 979.8
#define ADDRESS 0x18
#define CTRL_REG1 0x57
#define INTERVAL 7000
#define X_OFFSET -26
#define Y_OFFSET -6
#define Z_OFFSET 982

char *i2cdevName = "/dev/i2c-1";
int fd = 0;

__s32 s18(__s32 value) {
    return -(value & 0b100000000000) | (value & 0b011111111111);
}

double getAccX() {
    __s16 xRaw = (i2c_smbus_read_byte_data(fd, 0x29) << 8 | i2c_smbus_read_byte_data(fd, 0x28)) >> 4;
    return (double) ((double) (s18(xRaw) - X_OFFSET)) / 1024 * GRAVITY;
}

double getAccY() {
    __s16 yRaw = (i2c_smbus_read_byte_data(fd, 0x2B) << 8 | i2c_smbus_read_byte_data(fd, 0x2A)) >> 4;
    return (double) ((double) (s18(yRaw) - Y_OFFSET)) / 1024 * GRAVITY;
}

double getAccZ() {
    __s32 zRaw = (i2c_smbus_read_byte_data(fd, 0x2D) << 8 | i2c_smbus_read_byte_data(fd, 0x2C)) >> 4;
    return (double) ((double) (s18(zRaw) - Z_OFFSET)) / 1024 * GRAVITY;
}

double getSynVecSize(double v1, double v2, double v3) {
    return (double) sqrt(pow(v1, 2) + pow(v2, 2) + pow(v3, 2));
}

double getSIS(double arr[]) {
    int aIndex = quick_select(arr, N, N - ((int) ceil(FREQ * 0.3)) - 1);
    return (double) 2 * log10((double) arr[aIndex]) + 0.94;
}


int init() {
    if ((fd = open(i2cdevName,O_RDWR)) < 0) return -1;
    if (ioctl(fd, I2C_SLAVE,ADDRESS) < 0) return -1;
    return 0;
}

void startMeasurement() {
    i2c_smbus_write_byte_data(fd, 0x20, CTRL_REG1);
}

void stopMeasurement() {
    i2c_smbus_write_byte_data(fd, 0x20, 0x00);
}

double X[N], Y[N], Z[N];

int main(int argc, char **argv) {
    int i;
    if (init() < 0) return -1;
    printf("start measuring\n");
    startMeasurement();
    for (i = 0; i < N; i++) {
        X[i] = getAccX();
        Y[i] = getAccY();
        Z[i] = getAccZ();
        usleep(INTERVAL);
    }
    stopMeasurement();
    printf("finished mesuring and now start filtering\n");
    filtering(X);
    filtering(Y);
    filtering(Z);
    printf("finished filtering and now start calculating SIS\n");
    for (i = 0; i < N; i++) {
        X[i] = getSynVecSize(X[i], Y[i], Z[i]);
    }
    printf("finished caluculating SIS\n");
    printf("SIS: %lf\n", getSIS(X));
    return 0;
}
