// weather.c - Weather and particle system implementation
#include "weather.h"
#include "config.h"
#include "colors.h"

#include <graphx.h>
#include <sys/rtc.h>

typedef struct {
    int16_t x;
    int16_t y;
    int8_t  speed;
    int8_t  drift;
} particle_t;

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t brightness;  // 0 = dim, 1 = bright
} star_t;

static particle_t particles[MAX_PARTICLES];
static star_t stars[MAX_STARS];
static uint8_t particles_initialized = 0;
static uint8_t stars_initialized = 0;

uint8_t weather_get_random_type(void)
{
    // RNG-based weather: 80% none, 10% snow, 10% rain
    int r = rtc_Time() % 10;
    if (r == 0) return WEATHER_SNOW;
    if (r == 1) return WEATHER_RAIN;
    return WEATHER_NONE;
}

uint8_t weather_is_initialized(void)
{
    return particles_initialized;
}

uint8_t stars_is_initialized(void)
{
    return stars_initialized;
}

void weather_init(void)
{
    uint8_t i;
    for (i = 0; i < MAX_PARTICLES; i++) {
        particles[i].x = simple_rand() % SCREEN_WIDTH;
        particles[i].y = simple_rand() % SCREEN_HEIGHT;
        particles[i].speed = 2 + (simple_rand() % 3);
        particles[i].drift = (simple_rand() % 3) - 1;
    }
    particles_initialized = 1;
}

void stars_init(void)
{
    uint8_t i;
    for (i = 0; i < MAX_STARS; i++) {
        stars[i].x = simple_rand() % SCREEN_WIDTH;
        stars[i].y = simple_rand() % 50;  // Keep stars in upper portion
        stars[i].brightness = simple_rand() % 2;
    }
    stars_initialized = 1;
}

void stars_draw(uint8_t frame)
{
    uint8_t i;
    for (i = 0; i < MAX_STARS; i++) {
        // Twinkle effect: toggle brightness based on frame (slower twinkle)
        uint8_t twinkle = ((frame + i * 5) / 12) % 2;

        if (stars[i].brightness ^ twinkle) {
            gfx_SetColor(COLOR_STAR_BRIGHT);
            gfx_FillCircle(stars[i].x, stars[i].y, 2);
        } else {
            gfx_SetColor(COLOR_STAR_DIM);
            gfx_SetPixel(stars[i].x, stars[i].y);
        }
    }
}

static void draw_snow(uint8_t frame)
{
    uint8_t i;
    (void)frame;

    for (i = 0; i < MAX_PARTICLES; i++) {
        // Draw snowflake as small filled circle
        gfx_SetColor(COLOR_SNOW_WHITE);
        gfx_FillCircle(particles[i].x, particles[i].y, 2);
        gfx_SetColor(COLOR_GRAY);
        gfx_Circle(particles[i].x, particles[i].y, 2);

        // Update position - snow falls gently
        particles[i].y += particles[i].speed;
        particles[i].x += particles[i].drift;

        // Wrap around screen
        if (particles[i].y > SCREEN_HEIGHT) {
            particles[i].y = -5;
            particles[i].x = simple_rand() % SCREEN_WIDTH;
        }
        if (particles[i].x < 0) particles[i].x = SCREEN_WIDTH - 1;
        if (particles[i].x > SCREEN_WIDTH - 1) particles[i].x = 0;
    }
}

static void draw_rain(uint8_t frame)
{
    uint8_t i;
    (void)frame;

    gfx_SetColor(COLOR_RAIN_BLUE);
    for (i = 0; i < MAX_PARTICLES; i++) {
        // Draw raindrop as a line
        gfx_Line(particles[i].x, particles[i].y,
                 particles[i].x + 1, particles[i].y + 6);

        // Update position - rain falls fast
        particles[i].y += particles[i].speed + 3;
        particles[i].x += particles[i].drift;

        // Wrap around screen
        if (particles[i].y > SCREEN_HEIGHT) {
            particles[i].y = -10;
            particles[i].x = simple_rand() % SCREEN_WIDTH;
        }
        if (particles[i].x < 0) particles[i].x = SCREEN_WIDTH - 1;
        if (particles[i].x > SCREEN_WIDTH - 1) particles[i].x = 0;
    }
}

void weather_draw(uint8_t weather_type, uint8_t frame)
{
    switch (weather_type) {
        case WEATHER_SNOW: draw_snow(frame); break;
        case WEATHER_RAIN: draw_rain(frame); break;
    }
}
