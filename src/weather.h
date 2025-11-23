// weather.h - Weather and particle system
#ifndef WEATHER_H
#define WEATHER_H

#include <stdint.h>

// Initialize the weather/particle system
void weather_init(void);

// Draw weather effects (snow/rain)
void weather_draw(uint8_t weather_type, uint8_t frame);

// Get a random weather type
uint8_t weather_get_random_type(void);

// Initialize stars
void stars_init(void);

// Draw twinkling stars
void stars_draw(uint8_t frame);

// Check if particles are initialized
uint8_t weather_is_initialized(void);

// Check if stars are initialized
uint8_t stars_is_initialized(void);

#endif
