
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logo.h"
#include "lcd.h"
#include "gpio.h"
#include "tim.h"
#include "page_manager.h"

#define PWM_MAX_NAME_SIZE          (10)

#define PWM1_DUTY_VALUE_INDEX      (1)
#define PWM2_DUTY_VALUE_INDEX      (3)

#define TIM2_DUTY_VALUE_INDEX      (5)
#define TIM2_HZ_VALUE_INDEX        (6)
#define TIM3_DUTY_VALUE_INDEX      (8)
#define TIM3_HZ_VALUE_INDEX        (9)

typedef struct _pwm_label
{
    char name[PWM_MAX_NAME_SIZE];
    uint16_t x_pos;
    uint16_t y_pos;
    uint16_t label_color;
    uint16_t back_color;
    uint8_t duty;
    uint32_t hz;
}*pt_pwm_label, t_pwm_label;

static int8_t select_icon_index = -1;
static int8_t select_icon_old_index = -1;

static t_pwm_label g_pwm_label[] = {
    {"PWM1:", 61, 294, White, Blue, 0, 0},
    {"010%", 61, 140, White, Red, 10, 0},
    {"PWM2:", 106, 294, White, Blue, 0, 0},
    {"010%", 106, 140, White, Red, 10, 0},
    {"TIM2:", 151, 294, White, Blue, 0, 0},
    {"50%", 151, 199, White, Red, 0, 0},
    {"00000Hz", 151, 136, White, Red, 0, 0},
    {"TIM3:", 196, 294, White, Blue, 0, 0},
    {"50%", 196, 199, White, Red, 0, 0},
    {"00000Hz", 196, 136, White, Red, 0, 0}
};

static t_icon pwm_icon_array[] = {
    {"sub", 0, 85, 189, 24, 24, whtcc_sub, 1152},
    {"add", 1, 85, 51, 24, 24, whtcc_add, 1152},
    {"sub", 2, 130, 189, 24, 24, whtcc_sub, 1152},
    {"add", 3, 130, 51, 24, 24, whtcc_add, 1152}
};

static int pwm_page_draw_label(pt_pwm_label pwm_label)
{
    LCD_SetBackColor(pwm_label->back_color);
    LCD_SetTextColor(pwm_label->label_color);    
    LCD_DisplayStringWithPos(pwm_label->x_pos, pwm_label->y_pos, (uint8_t *)pwm_label->name);

    return 0;
}

static int pwm_page_draw_labels(pt_pwm_label pwm_label, uint8_t size)
{
    uint8_t i = 0;

    for (i = 0; i < size; i++)
    {
        pwm_page_draw_label(&pwm_label[i]);
    }

    return 0;
}

static int pwm_page_change_pwm_labels(pt_pwm_label pwm_label, uint8_t size)
{
    uint8_t i = 0;

    for (i = 0; i < size; i++)
    {
        if (i == PWM1_DUTY_VALUE_INDEX
            || i == PWM2_DUTY_VALUE_INDEX)
        {
            pwm_page_draw_label(&pwm_label[i]);
        }
    }

    return 0;
}

static int pwm_page_change_tim_labels(pt_pwm_label pwm_label, uint8_t size)
{
    uint8_t i = 0;

    for (i = 0; i < size; i++)
    {
        if (i == TIM2_DUTY_VALUE_INDEX
            || i == TIM2_HZ_VALUE_INDEX
            || i == TIM3_DUTY_VALUE_INDEX
            || i == TIM3_HZ_VALUE_INDEX)
        {
            pwm_page_draw_label(&pwm_label[i]);
        }
    }

    return 0;
}

static int pwm_page_draw_icon(pt_icon pwm_icon_array, uint8_t size)
{
    uint8_t i = 0;

    for (i = 0; i < size; i++)
    {
        LCD_DrawPictureWithPos(pwm_icon_array[i].p_logo,
                            pwm_icon_array[i].logo_size/2,
                            pwm_icon_array[i].x_pos,
                            pwm_icon_array[i].y_pos,
                            pwm_icon_array[i].height,
                            pwm_icon_array[i].weight, 0);
    }

    return 0;
}

static void pwm_page_run(void)
{
    uint8_t key_down_index = 0;
    uint8_t icon_num = sizeof(pwm_icon_array)/sizeof(pwm_icon_array[0]);
    uint8_t label_num = sizeof(g_pwm_label)/sizeof(g_pwm_label[0]);

    LCD_SetDisplayWindow(239, 319, 240, 320);

    LCD_Clear(White);

    main_page_draw_logo();
    pwm_page_draw_icon(pwm_icon_array, icon_num);

    if (select_icon_index != -1)
    {
        LCD_DrawPictureWithPos(pwm_icon_array[select_icon_index].p_logo,
                            pwm_icon_array[select_icon_index].logo_size/2,
                            pwm_icon_array[select_icon_index].x_pos,
                            pwm_icon_array[select_icon_index].y_pos,
                            pwm_icon_array[select_icon_index].height,
                            pwm_icon_array[select_icon_index].weight, 1);
    }

    LCD_SetDisplayWindow(239, 319, 240, 320);

    pwm_page_draw_labels(g_pwm_label, label_num);

    while(1)
    {
        key_scan(&key_down_index);

        if (key_down_index == 1)
        {
            select_icon_old_index = select_icon_index;
            select_icon_index = (select_icon_index + 1) % icon_num;

            if (select_icon_old_index != -1)
            {
                LCD_DrawPictureWithPos(pwm_icon_array[select_icon_old_index].p_logo,
                            pwm_icon_array[select_icon_old_index].logo_size/2,
                            pwm_icon_array[select_icon_old_index].x_pos,
                            pwm_icon_array[select_icon_old_index].y_pos,
                            pwm_icon_array[select_icon_old_index].height,
                            pwm_icon_array[select_icon_old_index].weight, 0);
            }

            LCD_DrawPictureWithPos(pwm_icon_array[select_icon_index].p_logo,
                            pwm_icon_array[select_icon_index].logo_size/2,
                            pwm_icon_array[select_icon_index].x_pos,
                            pwm_icon_array[select_icon_index].y_pos,
                            pwm_icon_array[select_icon_index].height,
                            pwm_icon_array[select_icon_index].weight, 1);
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
                LCD_DrawPictureWithPos(pwm_icon_array[select_icon_old_index].p_logo,
                            pwm_icon_array[select_icon_old_index].logo_size/2,
                            pwm_icon_array[select_icon_old_index].x_pos,
                            pwm_icon_array[select_icon_old_index].y_pos,
                            pwm_icon_array[select_icon_old_index].height,
                            pwm_icon_array[select_icon_old_index].weight, 0);
            }

            LCD_DrawPictureWithPos(pwm_icon_array[select_icon_index].p_logo,
                            pwm_icon_array[select_icon_index].logo_size/2,
                            pwm_icon_array[select_icon_index].x_pos,
                            pwm_icon_array[select_icon_index].y_pos,
                            pwm_icon_array[select_icon_index].height,
                            pwm_icon_array[select_icon_index].weight, 1);
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
                    g_pwm_label[PWM1_DUTY_VALUE_INDEX].duty -= 10;
                    if (g_pwm_label[PWM1_DUTY_VALUE_INDEX].duty < 10)
                    {
                        g_pwm_label[PWM1_DUTY_VALUE_INDEX].duty = 90;
                    }
                    break;
                case 1:
                    g_pwm_label[PWM1_DUTY_VALUE_INDEX].duty += 10;
                    if (g_pwm_label[PWM1_DUTY_VALUE_INDEX].duty > 90)
                    {
                        g_pwm_label[PWM1_DUTY_VALUE_INDEX].duty = 10;
                    }
                    break;
                case 2:
                    g_pwm_label[PWM2_DUTY_VALUE_INDEX].duty -= 10;
                    if (g_pwm_label[PWM2_DUTY_VALUE_INDEX].duty < 10)
                    {
                        g_pwm_label[PWM2_DUTY_VALUE_INDEX].duty = 90;
                    }
                    break;
                case 3:
                    g_pwm_label[PWM2_DUTY_VALUE_INDEX].duty += 10;
                    if (g_pwm_label[PWM2_DUTY_VALUE_INDEX].duty > 90)
                    {
                        g_pwm_label[PWM2_DUTY_VALUE_INDEX].duty = 10;
                    }
                    break;
                default:
                    break;
            }

            TIM16->CCR1 = g_pwm_label[PWM1_DUTY_VALUE_INDEX].duty;
            TIM17->CCR1 = g_pwm_label[PWM2_DUTY_VALUE_INDEX].duty;

            memset(g_pwm_label[PWM1_DUTY_VALUE_INDEX].name, '\0', PWM_MAX_NAME_SIZE);
            memset(g_pwm_label[PWM2_DUTY_VALUE_INDEX].name, '\0', PWM_MAX_NAME_SIZE);

            sprintf(g_pwm_label[PWM1_DUTY_VALUE_INDEX].name, "%03d%%", g_pwm_label[PWM1_DUTY_VALUE_INDEX].duty);
            sprintf(g_pwm_label[PWM2_DUTY_VALUE_INDEX].name, "%03d%%", g_pwm_label[PWM2_DUTY_VALUE_INDEX].duty);

            pwm_page_change_pwm_labels(g_pwm_label, label_num);
        }
        else if (key_down_index == 4)
        {
            break;
        }

        time_input_capture_get(&htim2, &g_pwm_label[TIM2_DUTY_VALUE_INDEX].duty, &g_pwm_label[TIM2_HZ_VALUE_INDEX].hz);
        time_input_capture_get(&htim3, &g_pwm_label[TIM3_DUTY_VALUE_INDEX].duty, &g_pwm_label[TIM3_HZ_VALUE_INDEX].hz);

        memset(g_pwm_label[TIM2_DUTY_VALUE_INDEX].name, '\0', PWM_MAX_NAME_SIZE);
        memset(g_pwm_label[TIM3_DUTY_VALUE_INDEX].name, '\0', PWM_MAX_NAME_SIZE);
        memset(g_pwm_label[TIM2_HZ_VALUE_INDEX].name, '\0', PWM_MAX_NAME_SIZE);
        memset(g_pwm_label[TIM3_HZ_VALUE_INDEX].name, '\0', PWM_MAX_NAME_SIZE);

        sprintf(g_pwm_label[TIM2_DUTY_VALUE_INDEX].name, "%02d%%", g_pwm_label[TIM2_DUTY_VALUE_INDEX].duty);
        sprintf(g_pwm_label[TIM3_DUTY_VALUE_INDEX].name, "%02d%%", g_pwm_label[TIM3_DUTY_VALUE_INDEX].duty);
        sprintf(g_pwm_label[TIM2_HZ_VALUE_INDEX].name, "%05dHz", g_pwm_label[TIM2_HZ_VALUE_INDEX].hz);
        sprintf(g_pwm_label[TIM3_HZ_VALUE_INDEX].name, "%05dHz", g_pwm_label[TIM3_HZ_VALUE_INDEX].hz);

        pwm_page_change_tim_labels(g_pwm_label, label_num);
    }
}

static t_page g_pwm_page = {
    .name = "PWM",
    .run  = pwm_page_run,
    .next = NULL
};

int pwm_page_register(void)
{
    return page_register(&g_pwm_page);
}
