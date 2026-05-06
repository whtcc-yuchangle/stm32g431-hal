
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usart.h"
#include "logo.h"
#include "lcd.h"
#include "gpio.h"
#include "page_manager.h"

#define AI_UART_BUF_LEN                    (16)

#define AI_MAX_NAME_SIZE                   (20)

#define AI_LABEL_FRUIT_CLASS_INDEX         (1)

typedef struct _ai_label
{
    char name[AI_MAX_NAME_SIZE];
    uint16_t x_pos;
    uint16_t y_pos;
    uint16_t label_color;
    uint16_t back_color;
}*pt_ai_label, t_ai_label;


static char ai_uart_rx_buf[AI_UART_BUF_LEN] = {'\0'};
static uint8_t ai_uart_rx_flag = 0;

static t_ai_label g_ai_label[] = {
    {"Class:", 164, 264, White, Blue},
    {"xxxxxx", 164, 152, White, Red},
};

static char g_fruit_cls_name[][AI_MAX_NAME_SIZE] = {
    "apple ",
    "banana",
    "orange"
};

static t_icon ai_icon_array[] = {
    {"apple", 0, 134, 205, 60, 90, whtcc_apple, 10800},
    {"banana", 1, 134, 205, 60, 90, whtcc_banana, 10800},
    {"orange", 2, 134, 205, 60, 90, whtcc_orange, 10800}
};

static int ai_page_draw_label(pt_ai_label ai_label)
{
    LCD_SetBackColor(ai_label->back_color);
    LCD_SetTextColor(ai_label->label_color);    
    LCD_DisplayStringWithPos(ai_label->x_pos, ai_label->y_pos, (uint8_t *)ai_label->name);

    return 0;
}

static int ai_page_draw_labels(pt_ai_label ai_label, uint8_t size)
{
    uint8_t i = 0;

    for (i = 0; i < size; i++)
    {
        ai_page_draw_label(&ai_label[i]);
    }

    return 0;
}

static int ai_page_draw_icon(pt_icon ai_icon_array, uint8_t index)
{
    LCD_DrawPictureWithPos(ai_icon_array[index].p_logo,
                        ai_icon_array[index].logo_size/2,
                        ai_icon_array[index].x_pos,
                        ai_icon_array[index].y_pos,
                        ai_icon_array[index].height,
                        ai_icon_array[index].weight, 0);

    return 0;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (ai_uart_rx_flag == 0)
    {
        ai_uart_rx_flag = 1;
    }
}

static void ai_page_run(void)
{
    uint8_t key_down_index = 0;
    uint8_t label_num = sizeof(g_ai_label)/sizeof(g_ai_label[0]);
    uint32_t class_id = 0;
    
    HAL_UART_Receive_IT(&huart1, (uint8_t *)ai_uart_rx_buf, 7);
    
    LCD_SetDisplayWindow(239, 319, 240, 320);

    LCD_Clear(White);

    main_page_draw_logo();
    
    LCD_SetDisplayWindow(239, 319, 240, 320);
    
    ai_page_draw_labels(g_ai_label, label_num);
    
    LCD_SetDisplayWindow(239, 319, 240, 320);
    
    while(1)
    {
        key_scan(&key_down_index);
        
        if (ai_uart_rx_flag == 1)
        {
            ai_uart_rx_flag = 0;
            
            sscanf(ai_uart_rx_buf, "class:%d", &class_id);
            
            ai_page_draw_icon(ai_icon_array, class_id);
				
            LCD_SetDisplayWindow(239, 319, 240, 320);
            
            memset(g_ai_label[AI_LABEL_FRUIT_CLASS_INDEX].name, '\0', AI_MAX_NAME_SIZE);
                
            strncpy(g_ai_label[AI_LABEL_FRUIT_CLASS_INDEX].name, g_fruit_cls_name[class_id], 6);
            
            ai_page_draw_label(&g_ai_label[AI_LABEL_FRUIT_CLASS_INDEX]);
            
            LCD_SetDisplayWindow(239, 319, 240, 320);
            
            HAL_UART_Receive_IT(&huart1, (uint8_t *)ai_uart_rx_buf, 7);
        }
        
        if (key_down_index == 4)
        {
            break;
        }
    }
}


static t_page g_ai_page = {
    .name = "AI",
    .run  = ai_page_run,
    .next = NULL
};


int ai_page_register(void)
{
    return page_register(&g_ai_page);
}

