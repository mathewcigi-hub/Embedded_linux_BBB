/*
 =============================================================================
 File        : digital_clock.c
 Author      : Modified version (based on original by Kiran N)
 Version     : 2.0
 Description : Digital counter/clock for 4-digit 7-seg LED using BeagleBone GPIO
 =============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <time.h>

#define SYSFS_GPIO_PATH "/sys/class/gpio"
#define BUFFER_SIZE     64

/* Segment GPIOs */
#define SEG_A   66
#define SEG_B   67
#define SEG_C   69
#define SEG_D   45
#define SEG_E   44
#define SEG_F   26
#define SEG_G   46
#define SEG_DP  68

/* Digit GPIOs */
#define DIG_1   48
#define DIG_2   49
#define DIG_3   112
#define DIG_4   115

/* Common Anode / Cathode configuration */
#ifdef COMMON_CATHODE
    #define SEG_ON   1
    #define SEG_OFF  0
#else
    #define SEG_ON   0
    #define SEG_OFF  1
#endif

/* Helper: Export GPIO */
static int gpio_export(int pin) {
    int fd = open(SYSFS_GPIO_PATH "/export", O_WRONLY);
    if (fd < 0) return -1;
    char buf[BUFFER_SIZE];
    int len = snprintf(buf, sizeof(buf), "%d", pin);
    write(fd, buf, len);
    close(fd);
    return 0;
}

/* Helper: Set direction */
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

/* Helper: Write value */
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

/* Segment pins in order A–G */
static int segments[] = { SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G };

/* Digit select pins */
static int digits[] = { DIG_1, DIG_2, DIG_3, DIG_4 };

/* Lookup table for numbers 0–9 */
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

/* Initialize GPIO pins */
static void setup_gpio() {
    for (int i=0; i<7; i++) { gpio_export(segments[i]); gpio_set_dir(segments[i],1); }
    gpio_export(SEG_DP); gpio_set_dir(SEG_DP,1);

    for (int i=0; i<4; i++) { gpio_export(digits[i]); gpio_set_dir(digits[i],1); }
}

/* Display single digit */
static void display_digit(int num) {
    for (int i=0; i<7; i++) {
        gpio_write(segments[i], digit_map[num][i] ? SEG_ON : SEG_OFF);
    }
}

/* Multiplex display across 4 digits */
static void display_number(int value) {
    for (int pos=3; pos>=0; pos--) {
        int digit_val = value % 10;
        value /= 10;

        gpio_write(digits[pos], 1);          // turn on digit
        display_digit(digit_val);            // show number
        usleep(100);                         // persistence
        display_digit(10);                   // clear
        gpio_write(digits[pos], 0);          // turn off digit
    }
}

/* Count up */
static void run_upcounter(int delay) {
    int count = 0;
    while (1) {
        for (int i=0; i<delay; i++) display_number(count);
        count = (count+1) % 10000;
    }
}

/* Count down */
static void run_downcounter(int delay) {
    int count = 9999;
    while (1) {
        for (int i=0; i<delay; i++) display_number(count);
        count = (count==0) ? 9999 : count-1;
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
