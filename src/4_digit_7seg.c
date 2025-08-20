
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <time.h>

#define SYSFS_GPIO_PATH "/sys/class/gpio"
#define BUFFER_SIZE     64


#define SEG_A   66
#define SEG_B   67
#define SEG_C   69
#define SEG_D   45
#define SEG_E   44
#define SEG_F   26
#define SEG_G   46
#define SEG_DP  68


#define DIG_1   48
#define DIG_2   49
#define DIG_3   112
#define DIG_4   115


#ifdef COMMON_CATHODE
    #define SEG_ON   1
    #define SEG_OFF  0
#else
    #define SEG_ON   0
    #define SEG_OFF  1
#endif


static int gpio_export(int pin) {
    int fd = open(SYSFS_GPIO_PATH "/export", O_WRONLY);
    if (fd < 0) return -1;
    char buf[BUFFER_SIZE];
    int len = snprintf(buf, sizeof(buf), "%d", pin);
    write(fd, buf, len);
    close(fd);
    return 0;
}


static int gpio_set_dir(int pin, int output) {
    char path[BUFFER_SIZE];
    snprintf(path, sizeof(path), SYSFS_GPIO_PATH "/gpio%d/direction", pin);
    int fd = open(path, O_WRONLY);
    if (fd < 0) return -1;
    if (output) write(fd, "out", 3);
    else write(fd, "in", 2);
    close(fd);
    return 0;
}


static int gpio_write(int pin, int value) {
    char path[BUFFER_SIZE];
    snprintf(path, sizeof(path), SYSFS_GPIO_PATH "/gpio%d/value", pin);
    int fd = open(path, O_WRONLY);
    if (fd < 0) return -1;
    if (value) write(fd, "1", 1);
    else write(fd, "0", 1);
    close(fd);
    return 0;
}


static int segments[] = { SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G };


static int digits[] = { DIG_1, DIG_2};


static int digit_map[10][7] = {
    {1,1,1,1,1,1,0}, // 0
    {0,1,1,0,0,0,0}, // 1
    {1,1,0,1,1,0,1}, // 2
    {1,1,1,1,0,0,1}, // 3
    {0,1,1,0,0,1,1}, // 4
    {1,0,1,1,0,1,1}, // 5
    {1,0,1,1,1,1,1}, // 6
    {1,1,1,0,0,0,0}, // 7
    {1,1,1,1,1,1,1}, // 8
    {1,1,1,1,0,1,1}  // 9
};


static void setup_gpio() {
    for (int i=0; i<7; i++) { gpio_export(segments[i]); gpio_set_dir(segments[i],1); }
    gpio_export(SEG_DP); gpio_set_dir(SEG_DP,1);

    for (int i=0; i<4; i++) { gpio_export(digits[i]); gpio_set_dir(digits[i],1); }
}


static void display_digit(int num) {
    for (int i=0; i<7; i++) {
        gpio_write(segments[i], digit_map[num][i] ? SEG_ON : SEG_OFF);
    }
}


static void display_number(int value) {
    for (int pos=1; pos>=0; pos--) {
        int digit_val = value % 10;
        value /= 10;

        gpio_write(digits[pos], 1);
        display_digit(digit_val);
        usleep(100);
        display_digit(10);
        gpio_write(digits[pos], 0);
    }
}

/* Count up */
static void run_upcounter(int delay) {
    int count = 0;
    while (1) {
        for (int i=0; i<delay; i++) display_number(count);
        count = (count+1) % 100; 
    }
}

/* Count down */
static void run_downcounter(int delay) {
    int count = 9999;
    while (1) {
        for (int i=0; i<delay; i++) display_number(count);
        count = (count==0) ? 99 : count-1;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <mode> <delay>\n", argv[0]);
        printf("Modes: up | down\n");
        return 1;
    }

    int delay = atoi(argv[2]);
    setup_gpio();

    if (strcmp(argv[1],"up")==0) run_upcounter(delay);
    else if (strcmp(argv[1],"down")==0) run_downcounter(delay);
    else printf("Invalid mode. Use up/down.\n");

    return 0;
}
