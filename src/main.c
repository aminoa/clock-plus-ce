#include <stdint.h>
#include <stdio.h>
#include <string.h>

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

static const char* quotes[] = {
    "Richard M. Stallman: Free software is a matter of liberty, not price. To understand the concept, you should think of 'free' as in 'free speech,' not as in 'free beer.'",
    "Richard M. Stallman: With software there are only two possibilites: either the users control the programme or the programme controls the users. If the programme controls the users, and the developer controls the programme, then the programme is an instrument of unjust power.",
    "Richard M. Stallman: Sharing is good, and with digital technology, sharing is easy.",
    "Richard M. Stallman: Open source is a development methodology; free software is a social movement.",
    "Richard M. Stallman: I could have made money this way, and perhaps amused myself writing code. But I knew that at the end of my career, I would look back on years of building walls to divide people, and feel I had spent my life making the world a worse place.",
    "Eric S. Raymond: Given enough eyeballs, all bugs are shallow.",
    "Eric S. Raymond: Good programmers know what to write. Great ones know what to rewrite and reuse.",
    "Eric S. Raymond: To solve an interesting problem, start by finding a problem that is interesting to you.",
    "Eric S. Raymond: Treating your users as co-developers is your least-hassle route to rapid code improvement and effective debugging.",
    "Eric S. Raymond: Linus Torvalds's style of development - release early and often, delegate everything you can, be open to the point of promiscuity - came as a surprise.",
    "Bruce Perens: It is a massively parallel drunkards' walk filtered by a Darwinian process.",
    "Bruce Perens: The Open Source Definition is a bill of rights for the computer user.",
    "Tim O'Reilly: Work on Stuff that Matters.",
    "Tim O'Reilly: Create more value than you Capture.",
    "Tim O'Reilly: Money is like gasoline during a road trip. You don't want to run out of gas on your trip, but you're not doing a tour of gas stations.",
    "Tim O'Reilly: Pursue something so important that even if you fail, the world is better off with you having tried.",
    "Tim O'Reilly: For a typical author, obscurity is a far greater threat than piracy.",
    "Eben Moglen: The right to speak PGP is like the right to speak Navajo.",
    "Eben Moglen: We are a non-utopian political movement. We are not interested in going nowhere. The crucial operating premise of the Free Software Movement as a revolutionary politic is: Proof of Concept plus Running code. Here. We did it already.",
    "Eben Moglen: What societies value is what they memorize, and how they memorize it, and who has access to its memorized form determines the structure of power that the society represents.",
    "Linus Torvalds: Talk is cheap. Show me the code.",
    "Larry Wall: The three chief virtues of a programmer are: Laziness, Impatience and Hubris.",
    "Brian Kernighan: Controlling complexity is the essence of computer programming.",
    "Dennis Ritchie: UNIX is basically a simple operating system, but you have to be a genius to understand the simplicity.",
    "Donald Knuth: We should forget about small efficiencies, say about 97% of the time: premature optimization is the root of all evil.",
    "Bjarne Stroustrup: C makes it easy to shoot yourself in the foot; C++ makes it harder, but when you do it blows your whole leg off.",
    "Rob Pike: Simplicity is complicated.",
    "Guido van Rossum: Readability counts.",
    "Eric S. Raymond: Any tool should be useful in the expected way, but a truly great tool lends itself to uses you never expected.",
    "Eric S. Raymond: If you have the right attitude, interesting problems will find you."
};

#define QUOTE_COUNT (sizeof(quotes) / sizeof(quotes[0]))

// Check if it's nighttime (before 9 AM or after 5 PM)
static uint8_t is_nighttime(uint8_t hours)
{
    return (hours >= NIGHT_START_HOUR || hours < NIGHT_END_HOUR);
}

// Draw text with word wrapping
static void draw_wrapped_text(const char *text, uint24_t x, uint24_t y, uint24_t max_width, uint8_t line_height)
{
    char line_buf[64];
    const char *ptr = text;
    uint24_t current_y = y;

    while (*ptr) {
        uint8_t line_len = 0;
        uint8_t last_space = 0;

        // Build line that fits within max_width
        while (*ptr && line_len < sizeof(line_buf) - 1) {
            line_buf[line_len] = *ptr;
            line_buf[line_len + 1] = '\0';

            if (gfx_GetStringWidth(line_buf) > max_width) {
                // Line too long, break at last space
                if (last_space > 0) {
                    line_buf[last_space] = '\0';
                    ptr -= (line_len - last_space);
                } else {
                    // No space found, force break
                    line_buf[line_len] = '\0';
                }
                break;
            }

            if (*ptr == ' ') {
                last_space = line_len;
            }

            ptr++;
            line_len++;
        }

        // Draw the line centered
        gfx_PrintStringXY(line_buf, (SCREEN_WIDTH - gfx_GetStringWidth(line_buf)) / 2, current_y);
        current_y += line_height;

        // Skip leading spaces on next line
        while (*ptr == ' ') ptr++;
    }
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
    uint8_t show_quote = 0;      // 0 = battery/RAM, 1 = quote
    uint8_t current_quote = 0;
    uint8_t left_pressed = 0, right_pressed = 0;
    char buf[32];

    gfx_Begin();
    gfx_SetDrawBuffer();

    // Select random quote on startup
    current_quote = simple_rand() % QUOTE_COUNT;

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

        // Draw battery/RAM status or quote
        if (show_quote) {
            // Draw quote with wrapping
            if (night) {
                gfx_SetTextFGColor(COLOR_TEXT_LIGHT);
            } else {
                gfx_SetTextFGColor(COLOR_TEXT_DARK);
            }
            draw_wrapped_text(quotes[current_quote], 10, 5, SCREEN_WIDTH - 20, 10);
        } else {
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
        }

        gfx_SetTextScale(4, 4);
        sprintf(buf, "%02d/%02d/%04d", month, day, year);
        gfx_PrintStringXY(buf, (SCREEN_WIDTH - gfx_GetStringWidth(buf)) / 2, 60);
        sprintf(buf, "%02d:%02d", hours, mins);
        gfx_PrintStringXY(buf, (SCREEN_WIDTH - gfx_GetStringWidth(buf)) / 2, 110);
        gfx_SetTextScale(1, 1);

        // Draw sleeping feature animation
        feature_draw(frame);
        feature_draw_zzz(frame);

        gfx_SwapDraw();

        kb_Scan();
        if (kb_Data[6] & kb_Clear) {
            break;
        }

        // Up arrow = force night, Down arrow = force day
        if (kb_Data[7] & kb_Up) {
            night_override = 1;
            weather = weather_get_random_type();
        }
        if (kb_Data[7] & kb_Down) {
            night_override = 0;
            weather = weather_get_random_type();
        }

        // Left/Right arrows = toggle between battery/RAM and quote
        if (kb_Data[7] & kb_Left) {
            if (!left_pressed) {
                show_quote = !show_quote;
                if (show_quote) {
                    // Pick a new random quote when switching to quote view
                    current_quote = (rtc_Time() + frame) % QUOTE_COUNT;
                }
                left_pressed = 1;
            }
        } else {
            left_pressed = 0;
        }

        if (kb_Data[7] & kb_Right) {
            if (!right_pressed) {
                show_quote = !show_quote;
                if (show_quote) {
                    // Pick a new random quote when switching to quote view
                    current_quote = (rtc_Time() + frame) % QUOTE_COUNT;
                }
                right_pressed = 1;
            }
        } else {
            right_pressed = 0;
        }

        frame++;
        // delay(FRAME_DELAY_MS);
    }

    gfx_End();
    return 0;
}
