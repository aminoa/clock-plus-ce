// feature.c - Feature (caterpillar) character drawing implementation
#include "feature.h"
#include "config.h"
#include "colors.h"

#include <graphx.h>

void feature_draw(uint8_t frame)
{
    // Calculate breathing effect: oscillates between -1 and +1
    int8_t breath = ((frame / 4) % 20 < 10) ? ((frame / 4) % 10) / 5 : (9 - ((frame / 4) % 10)) / 5;
    
    // Body segments (back to front, so front overlaps)
    // Segment 4 (tail)
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_Circle(CAT_BASE_X + 38, CAT_BASE_Y + 8, 12 + breath);
    gfx_SetColor(COLOR_PURPLE_LIGHT);
    gfx_FillCircle(CAT_BASE_X + 38, CAT_BASE_Y + 8, 11 + breath);

    // Segment 3
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_Circle(CAT_BASE_X + 22, CAT_BASE_Y + 4, 14 + breath);
    gfx_SetColor(COLOR_PURPLE_LIGHT);
    gfx_FillCircle(CAT_BASE_X + 22, CAT_BASE_Y + 4, 13 + breath);

    // Segment 2
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_Circle(CAT_BASE_X + 4, CAT_BASE_Y + 2, 16 + breath);
    gfx_SetColor(COLOR_PURPLE_LIGHT);
    gfx_FillCircle(CAT_BASE_X + 4, CAT_BASE_Y + 2, 15 + breath);

    // Head (segment 1)
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_Circle(CAT_BASE_X - 16, CAT_BASE_Y + 2, 18 + breath);
    gfx_SetColor(COLOR_PURPLE_LIGHT);
    gfx_FillCircle(CAT_BASE_X - 16, CAT_BASE_Y + 2, 17 + breath);

    // Antennae
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_Line(CAT_BASE_X - 24, CAT_BASE_Y - 14, CAT_BASE_X - 28, CAT_BASE_Y - 26);
    gfx_FillCircle(CAT_BASE_X - 28, CAT_BASE_Y - 27, 3);
    gfx_Line(CAT_BASE_X - 10, CAT_BASE_Y - 14, CAT_BASE_X - 6, CAT_BASE_Y - 26);
    gfx_FillCircle(CAT_BASE_X - 6, CAT_BASE_Y - 27, 3);

    // Closed eyes (sleeping) - horizontal lines
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_HorizLine(CAT_BASE_X - 24, CAT_BASE_Y, 6);
    gfx_HorizLine(CAT_BASE_X - 12, CAT_BASE_Y, 6);

    // Blush marks
    gfx_SetColor(COLOR_PINK);
    gfx_FillCircle(CAT_BASE_X - 26, CAT_BASE_Y + 6, 2);
    gfx_FillCircle(CAT_BASE_X - 6, CAT_BASE_Y + 6, 2);

    // Little feet
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_FillCircle(CAT_BASE_X + 32, CAT_BASE_Y + 18, 2);
    gfx_FillCircle(CAT_BASE_X + 44, CAT_BASE_Y + 18, 2);
    gfx_FillCircle(CAT_BASE_X + 16, CAT_BASE_Y + 16, 2);
    gfx_FillCircle(CAT_BASE_X + 28, CAT_BASE_Y + 16, 2);
}

void feature_draw_zzz(uint8_t frame)
{
    uint8_t offset = (frame / 2) % 12;

    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_PrintStringXY("z", CAT_BASE_X - 38, CAT_BASE_Y - 34 - offset);
    gfx_PrintStringXY("z", CAT_BASE_X - 46, CAT_BASE_Y - 44 - offset);
    gfx_PrintStringXY("Z", CAT_BASE_X - 54, CAT_BASE_Y - 56 - offset);
}
