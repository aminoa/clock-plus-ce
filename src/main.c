#include <stdint.h>
#include <stdio.h>

#include <graphx.h>
#include <keypadc.h>
#include <sys/rtc.h>
#include <sys/timers.h>
#include <sys/power.h>
#include <ti/vars.h>

/* RGB color definitions for caterpillar */
#define COLOR_PURPLE_LIGHT  gfx_RGBTo1555(190, 162, 218)
#define COLOR_PURPLE_DARK   gfx_RGBTo1555(128, 80, 128)
#define COLOR_PINK          gfx_RGBTo1555(255, 182, 193)
#define COLOR_WHITE         gfx_RGBTo1555(255, 255, 255)
#define COLOR_SKY_BLUE      gfx_RGBTo1555(200, 220, 255)

/* Weather effect colors */
#define COLOR_RAIN_BLUE     gfx_RGBTo1555(100, 149, 237)
#define COLOR_SNOW_WHITE    gfx_RGBTo1555(240, 248, 255)
#define COLOR_GRAY          gfx_RGBTo1555(169, 169, 169)

/* Scene colors */
#define COLOR_CLOUD_WHITE   gfx_RGBTo1555(245, 245, 250)
#define COLOR_CLOUD_SHADOW  gfx_RGBTo1555(200, 200, 210)
#define COLOR_GRASS_GREEN   gfx_RGBTo1555(34, 139, 34)
#define COLOR_GRASS_LIGHT   gfx_RGBTo1555(50, 180, 50)
#define COLOR_TREE_TRUNK    gfx_RGBTo1555(0, 102, 0)
#define COLOR_TREE_LEAVES   gfx_RGBTo1555(34, 120, 34)
#define COLOR_TREE_LIGHT    gfx_RGBTo1555(0, 102, 0)
#define COLOR_FLOWER_RED    gfx_RGBTo1555(255, 80, 80)
#define COLOR_FLOWER_YELLOW gfx_RGBTo1555(255, 220, 50)
#define COLOR_FLOWER_CENTER gfx_RGBTo1555(255, 200, 0)

/* Weather types */
#define WEATHER_NONE    0
#define WEATHER_SNOW    1
#define WEATHER_RAIN    2

/* Particle system for weather effects */
#define MAX_PARTICLES   18

typedef struct {
    int16_t x;
    int16_t y;
    int8_t  speed;
    int8_t  drift;
} particle_t;

static particle_t particles[MAX_PARTICLES];
static uint8_t particles_initialized = 0;

/* Simple pseudo-random number generator */
static uint16_t rand_seed = 12345;
static uint16_t simple_rand(void)
{
    rand_seed = rand_seed * 25173 + 13849;
    return rand_seed;
}

static uint8_t get_weather_type(void)
{
    /* RNG-based weather: 80% none, 10% snow, 10% rain */
    uint8_t r = simple_rand() % 10;
    if (r == 0) return WEATHER_SNOW;
    if (r == 1) return WEATHER_RAIN;
    return WEATHER_NONE;
}

static void init_particles(void)
{
    uint8_t i;
    for (i = 0; i < MAX_PARTICLES; i++) {
        particles[i].x = simple_rand() % 320;
        particles[i].y = simple_rand() % 240;
        particles[i].speed = 2 + (simple_rand() % 3);
        particles[i].drift = (simple_rand() % 3) - 1;
    }
    particles_initialized = 1;
}

static void draw_snow(uint8_t frame)
{
    uint8_t i;
    (void)frame;

    for (i = 0; i < MAX_PARTICLES; i++) {
        /* Draw snowflake as small filled circle */
        gfx_SetColor(COLOR_SNOW_WHITE);
        gfx_FillCircle(particles[i].x, particles[i].y, 2);
        gfx_SetColor(COLOR_GRAY);
        gfx_Circle(particles[i].x, particles[i].y, 2);

        /* Update position - snow falls gently */
        particles[i].y += particles[i].speed;
        particles[i].x += particles[i].drift;

        /* Wrap around screen */
        if (particles[i].y > 240) {
            particles[i].y = -5;
            particles[i].x = simple_rand() % 320;
        }
        if (particles[i].x < 0) particles[i].x = 319;
        if (particles[i].x > 319) particles[i].x = 0;
    }
}

static void draw_rain(uint8_t frame)
{
    uint8_t i;
    (void)frame;

    gfx_SetColor(COLOR_RAIN_BLUE);
    for (i = 0; i < MAX_PARTICLES; i++) {
        /* Draw raindrop as a line */
        gfx_Line(particles[i].x, particles[i].y,
                 particles[i].x + 1, particles[i].y + 6);

        /* Update position - rain falls fast */
        particles[i].y += particles[i].speed + 3;
        particles[i].x += particles[i].drift;

        /* Wrap around screen */
        if (particles[i].y > 240) {
            particles[i].y = -10;
            particles[i].x = simple_rand() % 320;
        }
        if (particles[i].x < 0) particles[i].x = 319;
        if (particles[i].x > 319) particles[i].x = 0;
    }
}

static void draw_weather(uint8_t weather, uint8_t frame)
{
    switch (weather) {
        case WEATHER_SNOW: draw_snow(frame); break;
        case WEATHER_RAIN: draw_rain(frame); break;
    }
}

/* Scene drawing functions */
static void draw_clouds(void)
{
    /* Cloud 1 - top left area */
    gfx_SetColor(COLOR_CLOUD_WHITE);
    gfx_FillCircle(35, 35, 14);
    gfx_FillCircle(55, 32, 16);
    gfx_FillCircle(75, 35, 12);

    /* Cloud 2 - top right area (smaller) */
    gfx_SetColor(COLOR_CLOUD_WHITE);
    gfx_FillCircle(250, 48, 10);
    gfx_FillCircle(268, 48, 9);
    gfx_FillCircle(259, 44, 8);
}

static void draw_ground(void)
{
    /* Main ground line */
    gfx_SetColor(COLOR_GRASS_GREEN);
    gfx_FillRectangle(0, 225, 320, 15);

    /* Grass tufts - small triangular blades */
    gfx_SetColor(COLOR_GRASS_LIGHT);
    uint16_t i;
    for (i = 5; i < 320; i += 15) {
        gfx_Line(i, 225, i + 3, 218);
        gfx_Line(i + 3, 218, i + 6, 225);
        gfx_Line(i + 8, 225, i + 10, 220);
        gfx_Line(i + 10, 220, i + 12, 225);
    }
}

static void draw_tree(int16_t x, int16_t y, uint8_t size)
{
    /* Tree trunk */
    gfx_SetColor(COLOR_TREE_TRUNK);
    gfx_FillRectangle(x - size/4, y, size/2, size + size/2);

    /* Tree foliage - layered circles */
    gfx_SetColor(COLOR_TREE_LEAVES);
    gfx_FillCircle(x, y - size/2, size);
    gfx_FillCircle(x - size/2, y - size/4, size * 3/4);
    gfx_FillCircle(x + size/2, y - size/4, size * 3/4);

    /* Highlight */
    gfx_SetColor(COLOR_TREE_LIGHT);
    gfx_FillCircle(x - size/4, y - size/2 - size/4, size/2);
}

static void draw_trees(void)
{
    /* Left side trees */
    draw_tree(30, 200, 20);
    draw_tree(70, 213, 14);
}

static void draw_flower(int16_t x, int16_t y, uint16_t color)
{
    /* Simple 5-petal flower */
    gfx_SetColor(color);
    gfx_FillCircle(x - 3, y, 2);
    gfx_FillCircle(x + 3, y, 2);
    gfx_FillCircle(x, y - 3, 2);
    gfx_FillCircle(x, y + 3, 2);
    /* Center */
    gfx_SetColor(COLOR_FLOWER_CENTER);
    gfx_FillCircle(x, y, 2);
}

static void draw_flowers(void)
{
    draw_flower(100, 218, COLOR_FLOWER_RED);
    draw_flower(130, 220, COLOR_FLOWER_YELLOW);
    draw_flower(160, 217, COLOR_FLOWER_RED);
    draw_flower(200, 219, COLOR_FLOWER_YELLOW);
}

static void draw_scene(void)
{
    draw_clouds();
    draw_trees();
    draw_ground();
    draw_flowers();
}

/* Feature position (bottom-right corner) */
#define CAT_BASE_X  270
#define CAT_BASE_Y  208

// Battery Status definitions
#define BATTERY_EMPTY       0
#define BATTERY_LOW         1
#define BATTERY_MEDIUM      2
#define BATTERY_HIGH        3
#define BATTERY_FULL        4

static void draw_caterpillar(void)
{
    /* Body segments (back to front, so front overlaps) */
    /* Segment 4 (tail) */
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_Circle(CAT_BASE_X + 38, CAT_BASE_Y + 8, 12);
    gfx_SetColor(COLOR_PURPLE_LIGHT);
    gfx_FillCircle(CAT_BASE_X + 38, CAT_BASE_Y + 8, 11);

    /* Segment 3 */
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_Circle(CAT_BASE_X + 22, CAT_BASE_Y + 4, 14);
    gfx_SetColor(COLOR_PURPLE_LIGHT);
    gfx_FillCircle(CAT_BASE_X + 22, CAT_BASE_Y + 4, 13);

    /* Segment 2 */
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_Circle(CAT_BASE_X + 4, CAT_BASE_Y + 2, 16);
    gfx_SetColor(COLOR_PURPLE_LIGHT);
    gfx_FillCircle(CAT_BASE_X + 4, CAT_BASE_Y + 2, 15);

    /* Head (segment 1) */
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_Circle(CAT_BASE_X - 16, CAT_BASE_Y + 2, 18);
    gfx_SetColor(COLOR_PURPLE_LIGHT);
    gfx_FillCircle(CAT_BASE_X - 16, CAT_BASE_Y + 2, 17);

    /* Antennae */
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_Line(CAT_BASE_X - 24, CAT_BASE_Y - 14, CAT_BASE_X - 28, CAT_BASE_Y - 26);
    gfx_FillCircle(CAT_BASE_X - 28, CAT_BASE_Y - 27, 3);
    gfx_Line(CAT_BASE_X - 10, CAT_BASE_Y - 14, CAT_BASE_X - 6, CAT_BASE_Y - 26);
    gfx_FillCircle(CAT_BASE_X - 6, CAT_BASE_Y - 27, 3);

    /* Closed eyes (sleeping) - horizontal lines */
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_HorizLine(CAT_BASE_X - 24, CAT_BASE_Y, 6);
    gfx_HorizLine(CAT_BASE_X - 12, CAT_BASE_Y, 6);

    /* Blush marks */
    gfx_SetColor(COLOR_PINK);
    gfx_FillCircle(CAT_BASE_X - 26, CAT_BASE_Y + 6, 2);
    gfx_FillCircle(CAT_BASE_X - 6, CAT_BASE_Y + 6, 2);

    /* Little feet */
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_FillCircle(CAT_BASE_X + 32, CAT_BASE_Y + 18, 2);
    gfx_FillCircle(CAT_BASE_X + 44, CAT_BASE_Y + 18, 2);
    gfx_FillCircle(CAT_BASE_X + 16, CAT_BASE_Y + 16, 2);
    gfx_FillCircle(CAT_BASE_X + 28, CAT_BASE_Y + 16, 2);
}

static void draw_zzz(uint8_t frame)
{
    uint8_t offset = (frame / 2) % 12;

    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_PrintStringXY("z", CAT_BASE_X - 38, CAT_BASE_Y - 34 - offset);
    gfx_PrintStringXY("z", CAT_BASE_X - 46, CAT_BASE_Y - 44 - offset);
    gfx_PrintStringXY("Z", CAT_BASE_X - 54, CAT_BASE_Y - 56 - offset);
}

int main(void)
{
    uint8_t hours, mins, secs;
    uint8_t day, month;
    uint16_t year;
    uint8_t frame = 0;
    uint8_t weather;
    char buf[16];

    gfx_Begin();
    gfx_SetDrawBuffer();

    while (1) {
        boot_GetDate(&day, &month, &year);
        boot_GetTime(&secs, &mins, &hours);

        /* Initialize particles and weather on first run */
        if (!particles_initialized) {
            weather = get_weather_type();
            init_particles();
        }

        gfx_FillScreen(COLOR_SKY_BLUE);

        /* Draw scene background */
        draw_scene();

        /* Draw weather effects */
        draw_weather(weather, frame);

        /* Draw date and time */
        gfx_SetTextFGColor(0x00);

        gfx_SetTextScale(2, 2);
        sprintf(buf, "%02d/%02d/%04d", month, day, year);
        gfx_PrintStringXY(buf, (320 - gfx_GetStringWidth(buf)) / 2, 95);
        sprintf(buf, "%02d:%02d:%02d", hours, mins, secs);
        gfx_PrintStringXY(buf, (320 - gfx_GetStringWidth(buf)) / 2, 120);
        gfx_SetTextScale(1, 1);

        // Draw battery status
        if (boot_BatteryCharging()) {
            if (boot_GetBatteryStatus() == BATTERY_FULL) {
                gfx_PrintStringXY("Fully Charged", (320 - gfx_GetStringWidth("Fully Charged")) / 2, 10);
            } else {
                gfx_PrintStringXY("Charging...", (320 - gfx_GetStringWidth("Charging...")) / 2, 10);
            }
        } else {
            switch (boot_GetBatteryStatus()) {
                case BATTERY_EMPTY:
                    gfx_SetTextFGColor(gfx_RGBTo1555(255, 0, 0));  /* Red */
                    break;
                case BATTERY_LOW:
                    gfx_SetTextFGColor(gfx_RGBTo1555(255, 165, 0));  /* Orange */
                    break;
                case BATTERY_MEDIUM:
                    gfx_SetTextFGColor(gfx_RGBTo1555(255, 255, 0));  /* Yellow */
                    break;
                case BATTERY_HIGH:
                    gfx_SetTextFGColor(gfx_RGBTo1555(173, 255, 47));  /* GreenYellow */
                    break;
                case BATTERY_FULL:
                    gfx_SetTextFGColor(gfx_RGBTo1555(0, 128, 0));    /* Green */
                    break;
                default:
                    gfx_SetTextFGColor(0x00);  /* Default to black */
                    break;
            }

            sprintf(buf, "Battery: %d%%", boot_GetBatteryStatus() * 25);
            gfx_PrintStringXY(buf, (320 - gfx_GetStringWidth(buf)) / 2, 10);
        }

        // Draw free RAM
        gfx_SetTextFGColor(0x00);
        size_t free_ram = os_MemChk(NULL);
        sprintf(buf, "Free RAM: %uKB", (unsigned int)(free_ram / 1024));
        gfx_PrintStringXY(buf, (320 - gfx_GetStringWidth(buf)) / 2, 20);

        /* Draw sleeping caterpillar animation */
        draw_caterpillar();
        draw_zzz(frame);

        gfx_SwapDraw();

        kb_Scan();
        if (kb_Data[6] & kb_Clear) {
            break;
        }

        frame++;
        delay(200);  /* Faster updates for smooth animation */
    }

    gfx_End();
    return 0;
}
