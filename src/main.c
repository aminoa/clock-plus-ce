#include <stdint.h>
#include <stdio.h>

#include <graphx.h>
#include <keypadc.h>
#include <sys/rtc.h>
#include <sys/timers.h>
#include <sys/power.h>

/* RGB color definitions for caterpillar */
#define COLOR_PURPLE_LIGHT  gfx_RGBTo1555(190, 162, 218)
#define COLOR_PURPLE_DARK   gfx_RGBTo1555(128, 80, 128)
#define COLOR_PINK          gfx_RGBTo1555(255, 182, 193)
#define COLOR_WHITE         gfx_RGBTo1555(255, 255, 255)

/* Caterpillar position (bottom-right corner) */
#define CAT_BASE_X  270
#define CAT_BASE_Y  200

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
    gfx_Circle(CAT_BASE_X + 4, CAT_BASE_Y - 2, 16);
    gfx_SetColor(COLOR_PURPLE_LIGHT);
    gfx_FillCircle(CAT_BASE_X + 4, CAT_BASE_Y - 2, 15);

    /* Head (segment 1) */
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_Circle(CAT_BASE_X - 16, CAT_BASE_Y - 14, 18);
    gfx_SetColor(COLOR_PURPLE_LIGHT);
    gfx_FillCircle(CAT_BASE_X - 16, CAT_BASE_Y - 14, 17);

    /* Antennae */
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_Line(CAT_BASE_X - 24, CAT_BASE_Y - 30, CAT_BASE_X - 28, CAT_BASE_Y - 42);
    gfx_FillCircle(CAT_BASE_X - 28, CAT_BASE_Y - 43, 3);
    gfx_Line(CAT_BASE_X - 10, CAT_BASE_Y - 30, CAT_BASE_X - 6, CAT_BASE_Y - 42);
    gfx_FillCircle(CAT_BASE_X - 6, CAT_BASE_Y - 43, 3);

    /* Closed eyes (sleeping) - horizontal lines */
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_HorizLine(CAT_BASE_X - 24, CAT_BASE_Y - 16, 6);
    gfx_HorizLine(CAT_BASE_X - 12, CAT_BASE_Y - 16, 6);

    /* Blush marks */
    gfx_SetColor(COLOR_PINK);
    gfx_FillCircle(CAT_BASE_X - 26, CAT_BASE_Y - 10, 2);
    gfx_FillCircle(CAT_BASE_X - 6, CAT_BASE_Y - 10, 2);

    /* Little feet */
    gfx_SetColor(COLOR_PURPLE_DARK);
    gfx_FillCircle(CAT_BASE_X + 32, CAT_BASE_Y + 18, 2);
    gfx_FillCircle(CAT_BASE_X + 44, CAT_BASE_Y + 18, 2);
    gfx_FillCircle(CAT_BASE_X + 16, CAT_BASE_Y + 16, 2);
    gfx_FillCircle(CAT_BASE_X + 28, CAT_BASE_Y + 16, 2);
}

static void draw_zzz(uint8_t frame)
{
    uint8_t offset = frame % 12;
    uint8_t alpha = (frame % 24) < 12 ? 1 : 0;

    gfx_SetColor(COLOR_PURPLE_DARK);

    /* Animate Zzz floating up */
    if (alpha) {
        gfx_PrintStringXY("z", CAT_BASE_X - 38, CAT_BASE_Y - 38 - offset);
        gfx_PrintStringXY("z", CAT_BASE_X - 44, CAT_BASE_Y - 46 - offset);
        gfx_PrintStringXY("Z", CAT_BASE_X - 50, CAT_BASE_Y - 56 - offset);
    } else {
        gfx_PrintStringXY("z", CAT_BASE_X - 38, CAT_BASE_Y - 32 - offset);
        gfx_PrintStringXY("z", CAT_BASE_X - 44, CAT_BASE_Y - 40 - offset);
        gfx_PrintStringXY("Z", CAT_BASE_X - 50, CAT_BASE_Y - 50 - offset);
    }
}

int main(void)
{
    uint8_t hours, mins, secs;
    uint8_t day, month;
    uint16_t year;
    uint8_t frame = 0;
    char buf[16];

    gfx_Begin();
    gfx_SetDrawBuffer();

    while (1) {
        boot_GetDate(&day, &month, &year);
        boot_GetTime(&secs, &mins, &hours);

        gfx_FillScreen(COLOR_WHITE);

        /* Draw date and time */
        gfx_SetTextFGColor(0x00);
        sprintf(buf, "%02d/%02d/%04d", month, day, year);
        gfx_PrintStringXY(buf, 110, 100);
        sprintf(buf, "%02d:%02d:%02d", hours, mins, secs);
        gfx_PrintStringXY(buf, 120, 120);

        // Draw battery status
        if (boot_BatteryCharging()) {
            if (boot_GetBatteryStatus() == BATTERY_FULL) {
                gfx_PrintStringXY("Fully Charged", 104, 10);
            } else {
                gfx_PrintStringXY("Charging...", 112, 10);
            }
        } else {
            switch (boot_GetBatteryStatus()) {
                // 4 - 100%
                // 3 - 80%
                // 2 - 60%
                // 1 - 40%
                // 0 - 0-20%
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

            sprintf(buf, "Battery: %d\%", boot_GetBatteryStatus()  * 25);
            gfx_PrintStringXY(buf, 112, 10);
        }

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
