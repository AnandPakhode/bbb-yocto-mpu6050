#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define address 0x68
#define wake 0x6B
#define accel_sensitivity 0x1C
#define gyro_sensitivity 0x1B
#define who_am_i 0x75
#define accel_start 0x3B
#define gyrp_start 0x43

int i2c_fd;
int xaccel_off, yaccel_off, zaccel_off, xgyro_off, ygyro_off, zgyro_off;
const int accel_max_deadzone = 8;
const int gyro_max_deadzone = 1;
int xacc_adjust, yacc_adjust, zacc_adjust, xgyro_adjust, ygyro_adjust, zgyro_adjust;

void setup(const char *dev_file){
    if((i2c_fd = open(dev_file, O_RDWR)) < 0 ){
        printf("failed to open devicefile \n");
        return;
    }

    if(ioctl(i2c_fd, I2C_SLAVE, address)<0){
        printf("cannot able to access the bus \n");
        return;
    }

    uint8_t wakeup[2] = {wake, 0x00};
    write(i2c_fd, wakeup, 2);
}

void write_reg(uint8_t reg, uint8_t data){
    uint8_t buffer[] = {reg, data};
    write(i2c_fd, buffer, 2);
}

void read_reg(uint8_t reg, uint8_t *data, int size){
    write(i2c_fd, &reg, 1);
    read(i2c_fd, data, size);
}

void read_mpu(int *ax, int *ay, int *az, int *gx, int *gy, int *gz){
    uint8_t data[14];
    read_reg(accel_start, data, 14);

    *ax = ((int16_t)(data[0] << 8 | data[1])) + (int16_t)xacc_adjust;
    *ay = ((int16_t)(data[2] << 8 | data[3])) + (int16_t)yacc_adjust;
    *az = ((int16_t)(data[4] << 8 | data[5])) + (int16_t)zacc_adjust;

    *gx = ((int16_t)(data[8] << 8 | data[9])) + (int16_t)xgyro_adjust;
    *gy = ((int16_t)(data[10] << 8 | data[11])) + (int16_t)ygyro_adjust;
    *gz = ((int16_t)(data[12] << 8 | data[13])) + (int16_t)zgyro_adjust;
}

void cal_offset(){
    int sample = 1000;
    int ax, ay, az, gx, gy, gz;
    for(int i =0; i<sample+100; i++){
        if(i>99){
            read_mpu(&ax, &ay, &az, &gx, &gy, &gz);
            xaccel_off += ax + xacc_adjust;
            yaccel_off += ay + yacc_adjust;
            zaccel_off += az + zacc_adjust;
            xgyro_off += gx + xgyro_adjust;
            ygyro_off += gy + ygyro_adjust;
            zgyro_off += gz + zgyro_adjust;
        }
        usleep(20000);;
    }
    xaccel_off = xaccel_off / sample;
    yaccel_off = yaccel_off / sample;
    zaccel_off = (zaccel_off/ sample) - 16384;

    xgyro_off = xgyro_off / sample;
    ygyro_off = ygyro_off / sample;
    zgyro_off = zgyro_off / sample;

}
void callibration(){
    int ready = 0;
    int loop =0;
    printf("calibrating mpu ......\n");
    int ax, ay, az, gx, gy, gz;
    //first process
    xaccel_off = yaccel_off = zaccel_off = xgyro_off = ygyro_off = zgyro_off = 0;
    cal_offset();

    //second process
    xacc_adjust = -xaccel_off / 8 ; 
    yacc_adjust = -yaccel_off/ 6 ;
    zacc_adjust = -zaccel_off / 8 ;

    xgyro_adjust = -xgyro_off / 4 ;
    ygyro_adjust = -ygyro_off / 2 ;
    zgyro_adjust = -zgyro_off / 4 ;

    while(1){
        
        cal_offset();
        if(abs(xaccel_off) <= accel_max_deadzone) ready++;
        else xacc_adjust = xacc_adjust - xaccel_off / accel_max_deadzone;

        if(abs(yaccel_off) <= accel_max_deadzone) ready++;
        else yacc_adjust = yacc_adjust - yaccel_off / accel_max_deadzone;

        if(abs(zaccel_off) <= accel_max_deadzone) ready++;
        else zacc_adjust = zacc_adjust - zaccel_off / accel_max_deadzone;

        if(abs(xgyro_off) <= gyro_max_deadzone) ready++;
        else xgyro_adjust = xgyro_adjust - xgyro_off / (gyro_max_deadzone + 1);

        if(abs(ygyro_off) <= gyro_max_deadzone) ready++;
        else ygyro_adjust = ygyro_adjust - ygyro_off / (gyro_max_deadzone + 1);

        if(abs(zgyro_off) <= gyro_max_deadzone) ready++;
        else zgyro_adjust = zgyro_adjust - zgyro_off / (gyro_max_deadzone + 1);

        printf(" %d, %d, %d, %d, %d, %d \n", xaccel_off, yaccel_off, zaccel_off, xgyro_off, ygyro_off, zgyro_off);

        loop++;
        printf(" current loop is: %d \n", loop);
        if(ready == 6 || loop >=25){
            printf(" the adjusting values are \n");
            printf("  %d, %d, %d, %d, %d, %d \n", xacc_adjust, yacc_adjust, zacc_adjust, xgyro_adjust, ygyro_adjust, zgyro_adjust);
            break;
        }
    }
}

int main(){
    const char *dev_file = "/dev/i2c-2";
    int ax, ay, az, gx, gy, gz;
    setup(dev_file);
    write_reg(accel_sensitivity, 0x00);
    write_reg(gyro_sensitivity, 0x00);

    callibration();

    while(1){
        read_mpu(&ax, &ay, &az, &gx, &gy, &gz);
        printf("XACC = %d, YACC = %d, ZACC = %d \n", ax, ay , az);
        printf("XGYRO = %d, YGYRO = %d, ZGYRO = %d \n", gx, gy, gz);
        usleep(300000);
    }
    return 0;
}