#include <stdint.h>
#include <stdio.h>

#include <graphx.h>
#include <keypadc.h>
#include <sys/rtc.h>
#include <sys/timers.h>
#include <sys/util.h>
#include <sys/power.h>
#include <ti/vars.h>

#include "config.h"
#include "colors.h"
#include "weather.h"
#include "scene.h"
#include "feature.h"

// Check if it's nighttime (before 9 AM or after 5 PM)
static uint8_t is_nighttime(uint8_t hours)
{
    return (hours >= NIGHT_START_HOUR || hours < NIGHT_END_HOUR);
}

int main(void)
{
    uint8_t hours, mins, secs;
    uint8_t day, month;
    uint16_t year;
    uint8_t frame = 0;
    uint8_t weather;
    uint8_t night;
    int8_t night_override = -1;  // -1 = auto, 0 = force day, 1 = force night
    char buf[32];

    gfx_Begin();
    gfx_SetDrawBuffer();

    while (1) {
        boot_GetDate(&day, &month, &year);
        boot_GetTime(&secs, &mins, &hours);

        // Check if it's nighttime (use override if set, otherwise auto)
        if (night_override >= 0) {
            night = night_override;
        } else {
            night = is_nighttime(hours);
        }

        // Initialize particles and weather on first run
        if (!weather_is_initialized()) {
            weather = weather_get_random_type();
            weather_init();
        }

        // Initialize stars if needed
        if (!stars_is_initialized()) {
            stars_init();
        }

        // Fill screen with appropriate background color
        if (night) {
            gfx_FillScreen(COLOR_NIGHT_SKY);
        } else {
            gfx_FillScreen(COLOR_SKY_BLUE);
        }

        // Draw scene background
        scene_draw(night, frame);

        // Draw weather effects
        weather_draw(weather, frame);

        // Draw date and time - white text at night, black during day
        if (night) {
            gfx_SetTextFGColor(COLOR_TEXT_LIGHT);
        } else {
            gfx_SetTextFGColor(COLOR_TEXT_DARK);
        }

        gfx_SetTextScale(4, 4);
        sprintf(buf, "%02d/%02d/%04d", month, day, year);
        gfx_PrintStringXY(buf, (SCREEN_WIDTH - gfx_GetStringWidth(buf)) / 2, 60);
        sprintf(buf, "%02d:%02d", hours, mins);
        gfx_PrintStringXY(buf, (SCREEN_WIDTH - gfx_GetStringWidth(buf)) / 2, 110);
        gfx_SetTextScale(1, 1);

        // Draw battery status
        if (boot_BatteryCharging()) {
            if (boot_GetBatteryStatus() == BATTERY_FULL) {
                gfx_PrintStringXY("Fully Charged", (SCREEN_WIDTH - gfx_GetStringWidth("Fully Charged")) / 2, 10);
            } else {
                gfx_PrintStringXY("Charging...", (SCREEN_WIDTH - gfx_GetStringWidth("Charging...")) / 2, 10);
            }
        } else {
            switch (boot_GetBatteryStatus()) {
                case BATTERY_EMPTY:
                    gfx_SetTextFGColor(gfx_RGBTo1555(255, 0, 0));  // Red
                    break;
                case BATTERY_LOW:
                    gfx_SetTextFGColor(gfx_RGBTo1555(255, 0, 0));  // Red
                    break;
            }

            sprintf(buf, "Battery: %d%%", boot_GetBatteryStatus() * 25);
            gfx_PrintStringXY(buf, (SCREEN_WIDTH - gfx_GetStringWidth(buf)) / 2, 10);
        }

        // Draw free RAM
        if (night) {
            gfx_SetTextFGColor(COLOR_TEXT_LIGHT);
        } else {
            gfx_SetTextFGColor(COLOR_TEXT_DARK);
        }
        size_t free_ram = os_MemChk(NULL);
        sprintf(buf, "Free RAM: %uKB", (unsigned int)(free_ram / 1024));
        gfx_PrintStringXY(buf, (SCREEN_WIDTH - gfx_GetStringWidth(buf)) / 2, 20);

        // Draw sleeping feature animation
        feature_draw();
        feature_draw_zzz(frame);

        gfx_SwapDraw();

        kb_Scan();
        if (kb_Data[6] & kb_Clear) {
            break;
        }

        // Up arrow = force night, Down arrow = force day
        if (kb_Data[7] & kb_Up) {
            night_override = 1;
        }
        if (kb_Data[7] & kb_Down) {
            night_override = 0;
        }

        frame++;
        delay(FRAME_DELAY_MS);
    }

    gfx_End();
    return 0;
}
