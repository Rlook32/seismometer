#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define GRAVITY 979.8
#define ADDRESS 0x18
#define CTRL_REG1 0x67
#define INTERVAL 50000

char *i2cdevName = "/dev/i2c-1";
int fd = 0;

__s32 s18(__s32 value) {
    return -(value & 0b100000000000) | (value & 0b011111111111);
}

double getAccX() {
    __s16 xRaw = (i2c_smbus_read_byte_data(fd, 0x29) << 8 | i2c_smbus_read_byte_data(fd, 0x28)) >> 4;
    return ((double) s18(xRaw)) / 1024 * GRAVITY;
}

double getAccY() {
    __s16 yRaw = (i2c_smbus_read_byte_data(fd, 0x2B) << 8 | i2c_smbus_read_byte_data(fd, 0x2A)) >> 4;
    return ((double) s18(yRaw)) / 1024 * GRAVITY;
}

double getAccZ() {
    __s32 zRaw = (i2c_smbus_read_byte_data(fd, 0x2D) << 8 | i2c_smbus_read_byte_data(fd, 0x2C)) >> 4;
    return ((double) s18(zRaw)) / 1024 * GRAVITY;
}

double getSynVecSize(double v1, double v2, double v3) {
    return sqrt(pow(v1, 2) + pow(v2, 2) + pow(v3, 2));
}

int init() {
    if((fd = open(i2cdevName,O_RDWR)) < 0){
        fprintf(stderr,"Can not open i2c port\n");
        return -1;
    }
    if (ioctl(fd, I2C_SLAVE,ADDRESS) < 0) {
        fprintf(stderr,"Unable to get bus access to talk to slave\n");
        return -1;
    }
    return 0;
}

void startMeasurement() {
    i2c_smbus_write_byte_data(fd, 0x20, CTRL_REG1);
}

void stopMeasurement() {
    i2c_smbus_write_byte_data(fd, 0x20, 0x00);

}

int main(int argc, char **argv) {
    if (init() < 0) return -1;
    startMeasurement();
    __s32 sum = 0;
    for (int i = 0; i < 2000; i++) {
        printf("%d,", s18((i2c_smbus_read_byte_data(fd, 0x2D) << 8 | i2c_smbus_read_byte_data(fd, 0x2C)) >> 4));
        usleep(INTERVAL);
    }
    stopMeasurement();
    return 0;
}
