// config.h - Configuration constants for CLOCK
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <sys/rtc.h>

// Screen dimensions
#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   240

// Caterpillar position (bottom-right corner)
#define CAT_BASE_X      270
#define CAT_BASE_Y      208

// Animation timing
#define FRAME_DELAY_MS  200

// Weather system
#define MAX_PARTICLES   18
#define MAX_STARS       1
#define WEATHER_NONE    0
#define WEATHER_SNOW    1
#define WEATHER_RAIN    2

// Battery status thresholds
#define BATTERY_EMPTY   0
#define BATTERY_LOW     1
#define BATTERY_MEDIUM  2
#define BATTERY_HIGH    3
#define BATTERY_FULL    4

// Night time bounds (24-hour format)
#define NIGHT_START_HOUR    17
#define NIGHT_END_HOUR      9

// Random number generator state (shared across files)
static uint32_t rand_state = 0;

// Simple RNG using time
static inline uint32_t simple_rand(void)
{
    uint8_t secs, mins, hours;
    boot_GetTime(&secs, &mins, &hours);

    rand_state = rand_state * 1103515245 + 12345;
    rand_state ^= (uint32_t)(secs * 17 + mins * 59 + hours * 3600);
    return rand_state;
}

#endif
