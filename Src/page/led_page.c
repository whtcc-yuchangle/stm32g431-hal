
#include <stdlib.h>
#include "logo.h"
#include "lcd.h"
#include "gpio.h"
#include "page_manager.h"

typedef struct _led_label
{
    char *name1;
    char *name2;
    uint8_t select_state;
    uint8_t onoff_state;
    uint16_t x_pos;
    uint16_t y_pos;
    uint16_t label_color;
    uint16_t back_color;
    uint16_t revert_back_color;
}*pt_led_label, t_led_label;

static int8_t select_label_index = -1;
static int8_t select_label_old_index = -1;

static t_led_label g_led_label[] = {
    {"Led1 on ", "Led1 off", 0, 0, 52, 299, White, Blue, Red},
    {"Led2 on ", "Led2 off", 0, 0, 52, 155, White, Blue, Red},
    {"Led3 on ", "Led3 off", 0, 0, 88, 299, White, Blue, Red},
    {"Led4 on ", "Led4 off", 0, 0, 88, 155, White, Blue, Red},
    {"Led5 on ", "Led5 off", 0, 0, 124, 299, White, Blue, Red},
    {"Led6 on ", "Led6 off", 0, 0, 124, 155, White, Blue, Red},
    {"Led7 on ", "Led7 off", 0, 0, 160, 299, White, Blue, Red},
    {"Led8 on ", "Led8 off", 0, 0, 160, 155, White, Blue, Red},
    {"   Led all on    ", "   Led all off   ", 0, 0, 196, 299, White, Blue, Red}
};

static int led_page_draw_label(pt_led_label led_label)
{
    if (led_label->onoff_state == 0 && led_label->select_state == 0)
    {
        LCD_SetBackColor(led_label->back_color);
        LCD_SetTextColor(led_label->label_color);    
        LCD_DisplayStringWithPos(led_label->x_pos, led_label->y_pos, (uint8_t *)led_label->name1);
    }
    else if (led_label->onoff_state == 0 && led_label->select_state == 1)
    {
        LCD_SetBackColor(led_label->revert_back_color);
        LCD_SetTextColor(led_label->label_color);    
        LCD_DisplayStringWithPos(led_label->x_pos, led_label->y_pos, (uint8_t *)led_label->name1);
    }
    else if (led_label->onoff_state == 1 && led_label->select_state == 0)
    {
        LCD_SetBackColor(led_label->back_color);
        LCD_SetTextColor(led_label->label_color);    
        LCD_DisplayStringWithPos(led_label->x_pos, led_label->y_pos, (uint8_t *)led_label->name2);
    }
    else
    {
        LCD_SetBackColor(led_label->revert_back_color);
        LCD_SetTextColor(led_label->label_color);    
        LCD_DisplayStringWithPos(led_label->x_pos, led_label->y_pos, (uint8_t *)led_label->name2);
    }

    return 0;
}

static int led_page_draw_labels(pt_led_label led_label, uint8_t size)
{
    uint8_t i = 0;

    for (i = 0; i < size; i++)
    {
        led_page_draw_label(&led_label[i]);
    }

    return 0;
}

static void led_page_run(void)
{
    uint8_t key_down_index = 0;
    uint8_t label_num = sizeof(g_led_label)/sizeof(g_led_label[0]);

    LCD_SetDisplayWindow(239, 319, 240, 320);

    LCD_Clear(White);

    main_page_draw_logo();

    LCD_SetDisplayWindow(239, 319, 240, 320);

    led_page_draw_labels(g_led_label, label_num);

    while(1)
    {
        key_scan(&key_down_index);

        if (key_down_index == 1)
        {
            select_label_old_index = select_label_index;
            select_label_index = (select_label_index + 1) % label_num;

            if (select_label_old_index != -1)
            {
                g_led_label[select_label_old_index].select_state = 0;
                led_page_draw_label(&g_led_label[select_label_old_index]);
            }

            g_led_label[select_label_index].select_state = 1;
            led_page_draw_label(&g_led_label[select_label_index]);
        }
        else if (key_down_index == 2)
        {
            if (select_label_index == -1)
            {
                select_label_old_index = select_label_index;
                select_label_index = label_num - 1;
            }
            else
            {
                select_label_old_index = select_label_index;
                select_label_index = select_label_index - 1;
                if (select_label_index < 0)
                {
                    select_label_index = label_num - 1;
                }
            }

            if (select_label_old_index != -1)
            {
                g_led_label[select_label_old_index].select_state = 0;
                led_page_draw_label(&g_led_label[select_label_old_index]);
            }

            g_led_label[select_label_index].select_state = 1;
            led_page_draw_label(&g_led_label[select_label_index]);

        }
        else if (key_down_index == 3)
        {
            if (select_label_index == -1)
            {
                continue;
            }

            if (g_led_label[select_label_index].onoff_state == 0)
            {
                g_led_label[select_label_index].onoff_state = 1;
            }
            else
            {
                g_led_label[select_label_index].onoff_state = 0;
            }

            led_page_draw_label(&g_led_label[select_label_index]);

            if (select_label_index >= 0 && select_label_index <= 7)
            {
                led_single_set_state(select_label_index+1, g_led_label[select_label_index].onoff_state);
            }
            else
            {
                if (g_led_label[select_label_index].onoff_state == 0)
                {
                    led_all_off();
                }
                else
                {
                    led_all_on();
                }
            }
        }
        else if (key_down_index == 4)
        {
            break;
        }
    }
}

static t_page g_led_page = {
    .name = "LED",
    .run  = led_page_run,
    .next = NULL
};

int led_page_register(void)
{
    return page_register(&g_led_page);
}
