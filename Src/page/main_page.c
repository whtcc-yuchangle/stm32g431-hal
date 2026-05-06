
#include <stdlib.h>
#include <string.h>
#include "logo.h"
#include "lcd.h"
#include "gpio.h"
#include "page_manager.h"

static int8_t select_icon_index = -1;
static int8_t select_icon_old_index = -1;

static t_icon icon_array[] = {
    {"led", 0, 102, 286, 48, 48, whtcc_led, 4608},
    {"adc", 1, 102, 213, 48, 48, whtcc_adc, 4608},
    {"pwm", 2, 102, 140, 48, 48, whtcc_pwm, 4608},
    {"rom", 3, 102, 67, 48, 48, whtcc_rom, 4608},
    {"clock", 4, 164, 286, 48, 48, whtcc_clock, 4608},
    {"ai", 5, 164, 213, 48, 48, whtcc_ai, 4608}
};

static int main_page_draw_icon(pt_icon icon_array, uint8_t size)
{
    uint8_t i = 0;

    for (i = 0; i < size; i++)
    {
        LCD_DrawPictureWithPos(icon_array[i].p_logo,
                            icon_array[i].logo_size/2,
                            icon_array[i].x_pos,
                            icon_array[i].y_pos,
                            icon_array[i].height,
                            icon_array[i].weight, 0);
    }

    return 0;
}

static void main_page_run(void)
{
    uint8_t key_down_index = 0;
    uint8_t icon_num = sizeof(icon_array)/sizeof(icon_array[0]);

    pt_page page = NULL;

    LCD_Clear(White);

    main_page_draw_logo();
    main_page_draw_icon(icon_array, icon_num);

    LCD_SetDisplayWindow(239, 319, 240, 320);

    while(1)
    {
        key_scan(&key_down_index);
        
        if (key_down_index == 1)
        {
            select_icon_old_index = select_icon_index;
            select_icon_index = (select_icon_index + 1) % icon_num;

            if (select_icon_old_index != -1)
            {
                LCD_DrawPictureWithPos(icon_array[select_icon_old_index].p_logo,
                            icon_array[select_icon_old_index].logo_size/2,
                            icon_array[select_icon_old_index].x_pos,
                            icon_array[select_icon_old_index].y_pos,
                            icon_array[select_icon_old_index].height,
                            icon_array[select_icon_old_index].weight, 0);
            }

            LCD_DrawPictureWithPos(icon_array[select_icon_index].p_logo,
                            icon_array[select_icon_index].logo_size/2,
                            icon_array[select_icon_index].x_pos,
                            icon_array[select_icon_index].y_pos,
                            icon_array[select_icon_index].height,
                            icon_array[select_icon_index].weight, 1);
        }
        else if (key_down_index == 2)
        {
            if (select_icon_index == -1)
            {
                select_icon_old_index = select_icon_index;
                select_icon_index = icon_num - 1;
            }
            else
            {
                select_icon_old_index = select_icon_index;
                select_icon_index = select_icon_index - 1;
                if (select_icon_index < 0)
                {
                    select_icon_index = icon_num - 1;
                }
            }
            
            if (select_icon_old_index != -1)
            {
                LCD_DrawPictureWithPos(icon_array[select_icon_old_index].p_logo,
                            icon_array[select_icon_old_index].logo_size/2,
                            icon_array[select_icon_old_index].x_pos,
                            icon_array[select_icon_old_index].y_pos,
                            icon_array[select_icon_old_index].height,
                            icon_array[select_icon_old_index].weight, 0);
            }
            
            LCD_DrawPictureWithPos(icon_array[select_icon_index].p_logo,
                            icon_array[select_icon_index].logo_size/2,
                            icon_array[select_icon_index].x_pos,
                            icon_array[select_icon_index].y_pos,
                            icon_array[select_icon_index].height,
                            icon_array[select_icon_index].weight, 1);
        }
        else if (key_down_index == 3)
        {
            if (select_icon_index == -1)
            {
                continue;
            }

            if (strcmp(icon_array[select_icon_index].name, "led") == 0)
            {
                page = page_get("LED"); 
            }
            else if (strcmp(icon_array[select_icon_index].name, "adc") == 0)
            {
                page = page_get("ADC"); 
            }
            else if (strcmp(icon_array[select_icon_index].name, "pwm") == 0)
            {
                page = page_get("PWM"); 
            }
            else if (strcmp(icon_array[select_icon_index].name, "rom") == 0)
            {
                page = page_get("ROM"); 
            }
            else if (strcmp(icon_array[select_icon_index].name, "clock") == 0)
            {
                page = page_get("CLOCK"); 
            }
            else if (strcmp(icon_array[select_icon_index].name, "ai") == 0)
            {
                page = page_get("AI"); 
            }
            else
            {
                continue;
            }

            page->run();
            
            LCD_SetDisplayWindow(239, 319, 240, 320);

            LCD_Clear(White);

            main_page_draw_logo();
            main_page_draw_icon(icon_array, icon_num);

            LCD_SetDisplayWindow(239, 319, 240, 320);

            LCD_DrawPictureWithPos(icon_array[select_icon_index].p_logo,
                            icon_array[select_icon_index].logo_size/2,
                            icon_array[select_icon_index].x_pos,
                            icon_array[select_icon_index].y_pos,
                            icon_array[select_icon_index].height,
                            icon_array[select_icon_index].weight, 1);
        }
    }
}

static t_page g_main_page = {
    .name = "MAIN",
    .run  = main_page_run,
    .next = NULL
};

int main_page_register(void)
{
    return page_register(&g_main_page);
}
