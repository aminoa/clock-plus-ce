// colors.h - Color palette definitions
#ifndef COLORS_H
#define COLORS_H

#include <graphx.h>

// Caterpillar colors
#define COLOR_PURPLE_LIGHT  gfx_RGBTo1555(190, 162, 218)
#define COLOR_PURPLE_DARK   gfx_RGBTo1555(128, 80, 128)
#define COLOR_PINK          gfx_RGBTo1555(255, 182, 193)
#define COLOR_WHITE         gfx_RGBTo1555(255, 255, 255)

// Weather effect colors
#define COLOR_RAIN_BLUE     gfx_RGBTo1555(100, 149, 237)
#define COLOR_SNOW_WHITE    gfx_RGBTo1555(240, 248, 255)
#define COLOR_GRAY          gfx_RGBTo1555(169, 169, 169)

// Scene colors
#define COLOR_CLOUD_WHITE   gfx_RGBTo1555(245, 245, 250)
#define COLOR_GRASS_GREEN   gfx_RGBTo1555(34, 139, 34)
#define COLOR_GRASS_LIGHT   gfx_RGBTo1555(50, 180, 50)
#define COLOR_TREE_TRUNK    gfx_RGBTo1555(0, 102, 0)
#define COLOR_TREE_LEAVES   0x02
#define COLOR_TREE_LIGHT    0x03
#define COLOR_FLOWER_RED    0x18
#define COLOR_FLOWER_YELLOW 0xE6
#define COLOR_FLOWER_CENTER 0x6A

// Sky colors
#define COLOR_SKY_BLUE      gfx_RGBTo1555(200, 220, 255)
#define COLOR_NIGHT_SKY     0x08
#define COLOR_STAR_BRIGHT   0xE7
#define COLOR_STAR_DIM      0xA4

// Text colors for day/night
#define COLOR_TEXT_LIGHT    0xFE
#define COLOR_TEXT_DARK     0x00

#endif
