
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logo.h"
#include "lcd.h"
#include "gpio.h"
#include "adc.h"
#include "dac.h"
#include "i2c_hal.h"
#include "page_manager.h"

#define MAX_NAME_SIZE               (10)

#define ADC1_D_VALUE_INDEX          (1)
#define ADC1_A_VALUE_INDEX          (2)
#define ADC2_D_VALUE_INDEX          (4)
#define ADC2_A_VALUE_INDEX          (5)

#define DAC1_CHANNEL1_VALUE_INDEX   (7)
#define DAC1_CHANNEL2_VALUE_INDEX   (9)

typedef struct _adc_label
{
    char name[MAX_NAME_SIZE];
    uint16_t x_pos;
    uint16_t y_pos;
    uint16_t label_color;
    uint16_t back_color;
}*pt_adc_label, t_adc_label;

static t_adc_label g_adc_label[] = {
    {"ADC1:", 61, 295, White, Blue},
    {"0000", 61, 191, White, Red},
    {"0.00V", 61, 103, White, Red},
    {"ADC2:", 106, 295, White, Blue},
    {"0000", 106, 191, White, Red},
    {"0.00V", 106, 103, White, Red},
    {"DAC1:", 151, 295, White, Blue},
    {"0.0V", 151, 140, White, Red},
    {"DAC2:", 196, 295, White, Blue},
    {"0.0V", 196, 140, White, Red}
};

static t_icon adc_icon_array[] = {
    {"sub", 0, 175, 189, 24, 24, whtcc_sub, 1152},
    {"add", 1, 175, 51, 24, 24, whtcc_add, 1152},
    {"sub", 2, 220, 189, 24, 24, whtcc_sub, 1152},
    {"add", 3, 220, 51, 24, 24, whtcc_add, 1152}
};

static int8_t select_icon_index = -1;
static int8_t select_icon_old_index = -1;

static double dac1_1_value = 0.0;
static double dac1_2_value = 0.0;

static int adc_page_draw_label(pt_adc_label adc_label)
{
    LCD_SetBackColor(adc_label->back_color);
    LCD_SetTextColor(adc_label->label_color);    
    LCD_DisplayStringWithPos(adc_label->x_pos, adc_label->y_pos, (uint8_t *)adc_label->name);
    
    return 0;
}

static int adc_page_draw_labels(pt_adc_label adc_label, uint8_t size)
{
    uint8_t i = 0;
    
    for (i = 0; i < size; i++)
    {
        adc_page_draw_label(&adc_label[i]);
    }
    
    return 0;
}

static int adc_page_change_labels(pt_adc_label adc_label, uint8_t size)
{
    uint8_t i = 0;
    
    for (i = 0; i < size; i++)
    {
        if (i == ADC1_D_VALUE_INDEX
            || i == ADC1_A_VALUE_INDEX
            || i == ADC2_D_VALUE_INDEX
            || i == ADC2_A_VALUE_INDEX)
        {
            adc_page_draw_label(&adc_label[i]);
        }
    }
    
    return 0;
}

static int adc_page_change_dac_labels(pt_adc_label adc_label, uint8_t size)
{
    uint8_t i = 0;
    
    for (i = 0; i < size; i++)
    {
        if (i == DAC1_CHANNEL1_VALUE_INDEX
            || i == DAC1_CHANNEL2_VALUE_INDEX)
        {
            adc_page_draw_label(&adc_label[i]);
        }
    }
    
    return 0;
}

static int adc_page_draw_icon(pt_icon adc_icon_array, uint8_t size)
{
    uint8_t i = 0;

    for (i = 0; i < size; i++)
    {
        LCD_DrawPictureWithPos(adc_icon_array[i].p_logo,
                            adc_icon_array[i].logo_size/2,
                            adc_icon_array[i].x_pos,
                            adc_icon_array[i].y_pos,
                            adc_icon_array[i].height,
                            adc_icon_array[i].weight, 0);
    }

    return 0;
}

static void adc_page_run(void)
{
    uint8_t key_down_index = 0;
    uint8_t label_num = sizeof(g_adc_label)/sizeof(g_adc_label[0]);
    uint8_t icon_num = sizeof(adc_icon_array)/sizeof(adc_icon_array[0]);
    
    uint32_t value1 = 0;
    uint32_t value2 = 0;
    double fvalue1 = 0.0;
    double fvalue2 = 0.0;

    LCD_SetDisplayWindow(239, 319, 240, 320);

    LCD_Clear(White);

    main_page_draw_logo();
    adc_page_draw_icon(adc_icon_array, icon_num);
    
    if (select_icon_index != -1)
    {
        LCD_DrawPictureWithPos(adc_icon_array[select_icon_index].p_logo,
                            adc_icon_array[select_icon_index].logo_size/2,
                            adc_icon_array[select_icon_index].x_pos,
                            adc_icon_array[select_icon_index].y_pos,
                            adc_icon_array[select_icon_index].height,
                            adc_icon_array[select_icon_index].weight, 1);
    }

    LCD_SetDisplayWindow(239, 319, 240, 320);
    
    if (dac1_1_value != 0.0)
    {
        memset(g_adc_label[DAC1_CHANNEL1_VALUE_INDEX].name, '\0', MAX_NAME_SIZE);
        sprintf(g_adc_label[DAC1_CHANNEL1_VALUE_INDEX].name, "%01.01lf", dac1_1_value);
    }
    
    if (dac1_2_value != 0.0)
    {
        memset(g_adc_label[DAC1_CHANNEL2_VALUE_INDEX].name, '\0', MAX_NAME_SIZE);
        sprintf(g_adc_label[DAC1_CHANNEL2_VALUE_INDEX].name, "%01.01lf", dac1_2_value);
    }
    
    dac1_set_value(1, dac1_1_value);
    dac1_set_value(2, dac1_2_value);

    adc_page_draw_labels(g_adc_label, label_num);

    while(1)
    {
        key_scan(&key_down_index);

        if (key_down_index == 1)
        {
            select_icon_old_index = select_icon_index;
            select_icon_index = (select_icon_index + 1) % icon_num;

            if (select_icon_old_index != -1)
            {
                LCD_DrawPictureWithPos(adc_icon_array[select_icon_old_index].p_logo,
                            adc_icon_array[select_icon_old_index].logo_size/2,
                            adc_icon_array[select_icon_old_index].x_pos,
                            adc_icon_array[select_icon_old_index].y_pos,
                            adc_icon_array[select_icon_old_index].height,
                            adc_icon_array[select_icon_old_index].weight, 0);
            }

            LCD_DrawPictureWithPos(adc_icon_array[select_icon_index].p_logo,
                            adc_icon_array[select_icon_index].logo_size/2,
                            adc_icon_array[select_icon_index].x_pos,
                            adc_icon_array[select_icon_index].y_pos,
                            adc_icon_array[select_icon_index].height,
                            adc_icon_array[select_icon_index].weight, 1);
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
                LCD_DrawPictureWithPos(adc_icon_array[select_icon_old_index].p_logo,
                            adc_icon_array[select_icon_old_index].logo_size/2,
                            adc_icon_array[select_icon_old_index].x_pos,
                            adc_icon_array[select_icon_old_index].y_pos,
                            adc_icon_array[select_icon_old_index].height,
                            adc_icon_array[select_icon_old_index].weight, 0);
            }

            LCD_DrawPictureWithPos(adc_icon_array[select_icon_index].p_logo,
                            adc_icon_array[select_icon_index].logo_size/2,
                            adc_icon_array[select_icon_index].x_pos,
                            adc_icon_array[select_icon_index].y_pos,
                            adc_icon_array[select_icon_index].height,
                            adc_icon_array[select_icon_index].weight, 1);
        }
        else if (key_down_index == 3)
        {
            if (select_icon_index == -1)
            {
                continue;
            }

            LCD_SetDisplayWindow(239, 319, 240, 320);
            
            switch (select_icon_index)
            {
                case 0:
                    dac1_1_value -= 0.1;
                    if (dac1_1_value < 0.0)
                    {
                        dac1_1_value = 3.3;
                    }
                    break;
                case 1:
                    dac1_1_value += 0.1;
                    if (dac1_1_value > 3.3)
                    {
                        dac1_1_value = 0.0;
                    }
                    break;
                case 2:
                    dac1_2_value -= 0.1;
                    if (dac1_2_value < 0.0)
                    {
                        dac1_2_value = 3.3;
                    }
                    break;
                case 3:
                    dac1_2_value += 0.1;
                    if (dac1_2_value > 3.3)
                    {
                        dac1_2_value = 0.0;
                    }
                    break;
                default:
                    break;
            }
            
            dac1_set_value(1, dac1_1_value);
            dac1_set_value(2, dac1_2_value);
            
            memset(g_adc_label[DAC1_CHANNEL1_VALUE_INDEX].name, '\0', MAX_NAME_SIZE);
            memset(g_adc_label[DAC1_CHANNEL2_VALUE_INDEX].name, '\0', MAX_NAME_SIZE);

            sprintf(g_adc_label[DAC1_CHANNEL1_VALUE_INDEX].name, "%01.01lf", dac1_1_value);
            sprintf(g_adc_label[DAC1_CHANNEL2_VALUE_INDEX].name, "%01.01lf", dac1_2_value);
            
            adc_page_change_dac_labels(g_adc_label, label_num);
        }
        if (key_down_index == 4)
        {
            break;
        }
        
        LCD_SetDisplayWindow(239, 319, 240, 320);

        adc_get_value(&hadc1, &value1, &fvalue1);
        adc_get_value(&hadc2, &value2, &fvalue2);
        
        memset(g_adc_label[ADC1_D_VALUE_INDEX].name, '\0', MAX_NAME_SIZE);
        memset(g_adc_label[ADC1_A_VALUE_INDEX].name, '\0', MAX_NAME_SIZE);
        memset(g_adc_label[ADC2_D_VALUE_INDEX].name, '\0', MAX_NAME_SIZE);
        memset(g_adc_label[ADC2_A_VALUE_INDEX].name, '\0', MAX_NAME_SIZE);
        
        sprintf(g_adc_label[ADC1_D_VALUE_INDEX].name, "%04d", value1);
        sprintf(g_adc_label[ADC1_A_VALUE_INDEX].name, "%1.02lfV", fvalue1);
        sprintf(g_adc_label[ADC2_D_VALUE_INDEX].name, "%04d", value2);
        sprintf(g_adc_label[ADC2_A_VALUE_INDEX].name, "%1.02lfV", fvalue2);
        
        adc_page_change_labels(g_adc_label, label_num);
    }
}

static t_page g_adc_page = {
    .name = "ADC",
    .run  = adc_page_run,
    .next = NULL
};

int adc_page_register(void)
{
    return page_register(&g_adc_page);
}
