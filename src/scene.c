// scene.c - Scene/background drawing implementation
#include "scene.h"
#include "config.h"
#include "colors.h"
#include "weather.h"

#include <graphx.h>

static void draw_clouds(void)
{
    // Cloud 1 - top left area
    gfx_SetColor(COLOR_CLOUD_WHITE);
    gfx_FillCircle(35, 35, 14);
    gfx_FillCircle(55, 32, 16);
    gfx_FillCircle(75, 35, 12);

    // Cloud 2 - top right area (smaller)
    gfx_SetColor(COLOR_CLOUD_WHITE);
    gfx_FillCircle(250, 48, 10);
    gfx_FillCircle(268, 48, 9);
    gfx_FillCircle(259, 44, 8);
}

static void draw_ground(void)
{
    // Main ground line
    gfx_SetColor(COLOR_GRASS_GREEN);
    gfx_FillRectangle(0, 225, SCREEN_WIDTH, 15);

    // Grass tufts - small triangular blades
    gfx_SetColor(COLOR_GRASS_LIGHT);
    uint16_t i;
    for (i = 5; i < SCREEN_WIDTH; i += 15) {
        gfx_Line(i, 225, i + 3, 218);
        gfx_Line(i + 3, 218, i + 6, 225);
        gfx_Line(i + 8, 225, i + 10, 220);
        gfx_Line(i + 10, 220, i + 12, 225);
    }
}

static void draw_tree(int16_t x, int16_t y, uint8_t size)
{
    // Tree trunk
    gfx_SetColor(COLOR_TREE_TRUNK);
    gfx_FillRectangle(x - size/4, y, size/2, size + size/2);

    // Tree foliage - layered circles
    gfx_SetColor(COLOR_TREE_LEAVES);
    gfx_FillCircle(x, y - size/2, size);
    gfx_FillCircle(x - size/2, y - size/4, size * 3/4);
    gfx_FillCircle(x + size/2, y - size/4, size * 3/4);

    // Highlight
    gfx_SetColor(COLOR_TREE_LIGHT);
    gfx_FillCircle(x - size/4, y - size/2 - size/4, size/2);
}

static void draw_trees(void)
{
    // Left side trees
    draw_tree(30, 200, 20);
    draw_tree(70, 213, 14);
}

static void draw_flower(int16_t x, int16_t y, uint16_t color)
{
    // Simple 5-petal flower
    gfx_SetColor(color);
    gfx_FillCircle(x - 3, y, 2);
    gfx_FillCircle(x + 3, y, 2);
    gfx_FillCircle(x, y - 3, 2);
    gfx_FillCircle(x, y + 3, 2);
    // Center
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

void scene_draw(uint8_t is_night, uint8_t frame)
{
    if (is_night) {
        stars_draw(frame);
    } else {
        draw_clouds();
    }
    draw_trees();
    draw_ground();
    draw_flowers();
}
