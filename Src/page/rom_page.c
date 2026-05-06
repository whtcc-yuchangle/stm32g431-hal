
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logo.h"
#include "lcd.h"
#include "gpio.h"
#include "adc.h"
#include "i2c_hal.h"
#include "page_manager.h"

#define ROM_MAX_NAME_SIZE                   (20)

#define ROM_LABEL_CLICK_CHANGE_NUM          (2)
#define ROM_LABEL_CLICK_SET_INDEX           (4)
#define ROM_LABEL_CLICK_GET_INDEX           (5)

#define ROM_LABEL_ADDR_CHANGE_INDEX         (1)
#define ROM_LABEL_VALUE_CHANGE_INDEX        (3)

#define ROM_LABEL_GET_VALUE_CHANGE_INDEX    (6)

typedef struct _rom_label
{
    char name[ROM_MAX_NAME_SIZE];
    uint16_t x_pos;
    uint16_t y_pos;
    uint16_t label_color;
    uint16_t back_color;
    uint16_t click_back_color;
}*pt_rom_label, t_rom_label;

static int8_t select_icon_index = -1;
static int8_t select_icon_old_index = -1;

static uint8_t g_rom_addr = 0;
static uint8_t g_rom_value = 0;

static t_rom_label g_rom_label[] = {
    {"ADDR:", 61, 294, White, Blue, Blue},
    {"000", 61, 124, White, Red, Red},
    {"VAL:", 106, 294, White, Blue, Blue},
    {"000", 106, 124, White, Red, Red},
    {" Click to Write ", 151, 294, White, Blue, 0xF81F},
    {"Get Val:", 196, 294, White, Blue, 0xF81F},
    {"000:000", 196, 146, White, Red, Red}
};

static t_icon rom_icon_array[] = {
    {"sub", 0, 85, 173, 24, 24, whtcc_sub, 1152},
    {"add", 1, 85, 51, 24, 24, whtcc_add, 1152},
    {"sub", 2, 130, 173, 24, 24, whtcc_sub, 1152},
    {"add", 3, 130, 51, 24, 24, whtcc_add, 1152}
};

static int rom_page_draw_icon(pt_icon rom_icon_array, uint8_t size)
{
    uint8_t i = 0;

    for (i = 0; i < size; i++)
    {
        LCD_DrawPictureWithPos(rom_icon_array[i].p_logo,
                            rom_icon_array[i].logo_size/2,
                            rom_icon_array[i].x_pos,
                            rom_icon_array[i].y_pos,
                            rom_icon_array[i].height,
                            rom_icon_array[i].weight, 0);
    }

    return 0;
}

static int rom_page_draw_click_label(pt_rom_label rom_label, uint8_t select_index, uint8_t click)
{
    if (click == 1)
    {
        LCD_SetBackColor(rom_label[select_index].click_back_color);
    }
    else
    {
        LCD_SetBackColor(rom_label[select_index].back_color);
    }
    LCD_SetTextColor(rom_label[select_index].label_color);
    LCD_DisplayStringWithPos(rom_label[select_index].x_pos, rom_label[select_index].y_pos, (uint8_t *)rom_label[select_index].name);

    return 0;
}

static int rom_page_draw_label(pt_rom_label rom_label)
{
    LCD_SetBackColor(rom_label->back_color);
    LCD_SetTextColor(rom_label->label_color);    
    LCD_DisplayStringWithPos(rom_label->x_pos, rom_label->y_pos, (uint8_t *)rom_label->name);

    return 0;
}

static int rom_page_draw_labels(pt_rom_label rom_label, uint8_t size)
{
    uint8_t i = 0;

    for (i = 0; i < size; i++)
    {
        rom_page_draw_label(&rom_label[i]);
    }

    return 0;
}

static int rom_page_change_addr_value_labels(pt_rom_label rom_label, uint8_t size)
{
    uint8_t i = 0;

    for (i = 0; i < size; i++)
    {
        if (i == ROM_LABEL_ADDR_CHANGE_INDEX
            || i == ROM_LABEL_VALUE_CHANGE_INDEX)
        {
            rom_page_draw_label(&rom_label[i]);
        }
    }

    return 0;
}

static int rom_page_change_get_value_labels(pt_rom_label rom_label, uint8_t size)
{
    uint8_t i = 0;

    for (i = 0; i < size; i++)
    {
        if (i == ROM_LABEL_GET_VALUE_CHANGE_INDEX)
        {
            rom_page_draw_label(&rom_label[i]);
        }
    }

    return 0;
}

static void rom_page_run(void)
{
    uint8_t key_down_index = 0;
    uint8_t icon_num = sizeof(rom_icon_array)/sizeof(rom_icon_array[0]);
    uint8_t label_num = sizeof(g_rom_label)/sizeof(g_rom_label[0]);
    uint8_t click_num = icon_num + ROM_LABEL_CLICK_CHANGE_NUM;
    
    uint8_t rom_get_value = 0;

    LCD_SetDisplayWindow(239, 319, 240, 320);

    LCD_Clear(White);

    main_page_draw_logo();
    rom_page_draw_icon(rom_icon_array, icon_num);
    
    LCD_SetDisplayWindow(239, 319, 240, 320);
    
    rom_page_draw_labels(g_rom_label, label_num);
    
    if (select_icon_index != -1)
    {
        if (select_icon_index >= 0 && select_icon_index < icon_num)
        {
            LCD_DrawPictureWithPos(rom_icon_array[select_icon_index].p_logo,
                                rom_icon_array[select_icon_index].logo_size/2,
                                rom_icon_array[select_icon_index].x_pos,
                                rom_icon_array[select_icon_index].y_pos,
                                rom_icon_array[select_icon_index].height,
                                rom_icon_array[select_icon_index].weight, 1);
        }
        else
        {
            LCD_SetDisplayWindow(239, 319, 240, 320);
            rom_page_draw_click_label(g_rom_label, select_icon_index - icon_num + ROM_LABEL_CLICK_SET_INDEX, 1);
        }
    }
    
    LCD_SetDisplayWindow(239, 319, 240, 320);

    while(1)
    {
        key_scan(&key_down_index);

        if (key_down_index == 1)
        {
            select_icon_old_index = select_icon_index;
            select_icon_index = (select_icon_index + 1) % click_num;

            if (select_icon_old_index != -1)
            {
                if (select_icon_old_index >= 0 && select_icon_old_index < icon_num)
                {
                    LCD_DrawPictureWithPos(rom_icon_array[select_icon_old_index].p_logo,
                                rom_icon_array[select_icon_old_index].logo_size/2,
                                rom_icon_array[select_icon_old_index].x_pos,
                                rom_icon_array[select_icon_old_index].y_pos,
                                rom_icon_array[select_icon_old_index].height,
                                rom_icon_array[select_icon_old_index].weight, 0);
                }
                else
                {
                    LCD_SetDisplayWindow(239, 319, 240, 320);
                    rom_page_draw_click_label(g_rom_label, select_icon_old_index - icon_num + ROM_LABEL_CLICK_SET_INDEX, 0);
                }
            }

            if (select_icon_index >= 0 && select_icon_index < icon_num)
            {
                LCD_DrawPictureWithPos(rom_icon_array[select_icon_index].p_logo,
                                rom_icon_array[select_icon_index].logo_size/2,
                                rom_icon_array[select_icon_index].x_pos,
                                rom_icon_array[select_icon_index].y_pos,
                                rom_icon_array[select_icon_index].height,
                                rom_icon_array[select_icon_index].weight, 1);
            }
            else
            {
                LCD_SetDisplayWindow(239, 319, 240, 320);
                rom_page_draw_click_label(g_rom_label, select_icon_index - icon_num + ROM_LABEL_CLICK_SET_INDEX, 1);
            }
        }
        else if (key_down_index == 2)
        {
            if (select_icon_index == -1)
            {
                select_icon_old_index = select_icon_index;
                select_icon_index = click_num - 1;
            }
            else
            {
                select_icon_old_index = select_icon_index;
                select_icon_index = select_icon_index - 1;
                if (select_icon_index < 0)
                {
                    select_icon_index = click_num - 1;
                }
            }

            if (select_icon_old_index != -1)
            {
                if (select_icon_old_index >= 0 && select_icon_old_index < icon_num)
                {
                    LCD_DrawPictureWithPos(rom_icon_array[select_icon_old_index].p_logo,
                                rom_icon_array[select_icon_old_index].logo_size/2,
                                rom_icon_array[select_icon_old_index].x_pos,
                                rom_icon_array[select_icon_old_index].y_pos,
                                rom_icon_array[select_icon_old_index].height,
                                rom_icon_array[select_icon_old_index].weight, 0);
                }
                else
                {
                    LCD_SetDisplayWindow(239, 319, 240, 320);
                    rom_page_draw_click_label(g_rom_label, select_icon_old_index - icon_num + ROM_LABEL_CLICK_SET_INDEX, 0);
                }
            }

            if (select_icon_index >= 0 && select_icon_index < icon_num)
            {
                LCD_DrawPictureWithPos(rom_icon_array[select_icon_index].p_logo,
                                rom_icon_array[select_icon_index].logo_size/2,
                                rom_icon_array[select_icon_index].x_pos,
                                rom_icon_array[select_icon_index].y_pos,
                                rom_icon_array[select_icon_index].height,
                                rom_icon_array[select_icon_index].weight, 1);
            }
            else
            {
                LCD_SetDisplayWindow(239, 319, 240, 320);
                rom_page_draw_click_label(g_rom_label, select_icon_index - icon_num + ROM_LABEL_CLICK_SET_INDEX, 1);
            }
        }
        else if (key_down_index == 3)
        {
            if (select_icon_index == -1)
            {
                continue;
            }

            LCD_SetDisplayWindow(239, 319, 240, 320);
            
            if (select_icon_index >= 0 && select_icon_index < icon_num)
            {
                if (select_icon_index == 0)
                {
                    if (g_rom_addr == 0)
                    {
                        g_rom_addr = 255;
                    }
                    else
                    {
                        g_rom_addr -= 1;
                    }
                }
                else if (select_icon_index == 1)
                {
                    if (g_rom_addr == 255)
                    {
                        g_rom_addr = 0;
                    }
                    else
                    {
                        g_rom_addr += 1;
                    }
                }
                else if (select_icon_index == 2)
                {
                    if (g_rom_value == 0)
                    {
                        g_rom_value = 255;
                    }
                    else
                    {
                        g_rom_value -= 1;
                    }
                }
                else if (select_icon_index == 3)
                {
                    if (g_rom_value == 255)
                    {
                        g_rom_value = 0;
                    }
                    else
                    {
                        g_rom_value += 1;
                    }
                }
                
                memset(g_rom_label[ROM_LABEL_ADDR_CHANGE_INDEX].name, '\0', ROM_MAX_NAME_SIZE);
                memset(g_rom_label[ROM_LABEL_VALUE_CHANGE_INDEX].name, '\0', ROM_MAX_NAME_SIZE);
                
                sprintf(g_rom_label[ROM_LABEL_ADDR_CHANGE_INDEX].name, "%03d", g_rom_addr);
                sprintf(g_rom_label[ROM_LABEL_VALUE_CHANGE_INDEX].name, "%03d", g_rom_value);
                
                rom_page_change_addr_value_labels(g_rom_label, label_num);
            }
            else if (select_icon_index == 4)
            {
                at24c02_i2c_write(&g_rom_value, 1, g_rom_addr);
            }
            else if (select_icon_index == 5)
            {
                at24c02_i2c_read(&rom_get_value, 1, g_rom_addr);
                
                memset(g_rom_label[ROM_LABEL_GET_VALUE_CHANGE_INDEX].name, '\0', ROM_MAX_NAME_SIZE);
                
                sprintf(g_rom_label[ROM_LABEL_GET_VALUE_CHANGE_INDEX].name, "%03d:%03d", g_rom_addr, rom_get_value);
                
                rom_page_change_get_value_labels(g_rom_label, label_num);
            }
            else
            {
                continue;
            }
        }
        else if (key_down_index == 4)
        {
            break;
        }
    }
}

static t_page g_rom_page = {
    .name = "ROM",
    .run  = rom_page_run,
    .next = NULL
};

int rom_page_register(void)
{
    return page_register(&g_rom_page);
}
