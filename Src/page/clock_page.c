
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logo.h"
#include "lcd.h"
#include "gpio.h"
#include "adc.h"
#include "i2c_hal.h"
#include "rtc.h"
#include "page_manager.h"

#define CLOCK_MAX_NAME_SIZE             (20)

#define CLOCK_YEAR_LABEL_INDEX          (1)
#define CLOCK_MONTH_LABEL_INDEX         (3)
#define CLOCK_DATE_LABEL_INDEX          (5)
#define CLOCK_HOUR_LABEL_INDEX          (7)
#define CLOCK_MINUTE_LABEL_INDEX        (9)
#define CLOCK_SECOND_LABEL_INDEX        (11)
#define CLOCK_WEEK_LABEL_INDEX          (13)
#define CLOCK_REFRESH_LABEL_INDEX       (14)

typedef struct _clock_label
{
    char name[CLOCK_MAX_NAME_SIZE];
    uint8_t select_state;
    uint16_t x_pos;
    uint16_t y_pos;
    uint16_t label_color;
    uint16_t back_color;
    uint16_t click_back_color;
}*pt_clock_label, t_clock_label;

static RTC_TimeTypeDef g_time = {0};
static RTC_DateTypeDef g_date = {0};

static uint8_t g_change_flag = 0;

static int8_t select_label_index = -1;
static int8_t select_label_old_index = -1;

static uint8_t change_label_index_map[] = {
    CLOCK_YEAR_LABEL_INDEX,
    CLOCK_MONTH_LABEL_INDEX,
    CLOCK_DATE_LABEL_INDEX,
    CLOCK_HOUR_LABEL_INDEX,
    CLOCK_MINUTE_LABEL_INDEX,
    CLOCK_SECOND_LABEL_INDEX,
    CLOCK_WEEK_LABEL_INDEX,
    CLOCK_REFRESH_LABEL_INDEX
};

static char *g_week_str[] = {
    "  Monday  ",
    " Tuesday  ",
    " Wednesday",
    " Thursday ",
    "  Friday  ",
    " Saturday ",
    "  Sunday  "
};

static t_clock_label g_clock_label[] = {
    {"Date:", 0, 61, 294, White, Blue, Blue},
    {"2024", 0, 61, 194, White, Red, 0xF81F},
    {"-", 0, 61, 130, Blue, White, White},
    {"11", 0, 61, 114, White, Red, 0xF81F},
    {"-", 0, 61, 82, Blue, White, White},
    {"11", 0, 61, 66, White, Red, 0xF81F},
    {"Time:", 0, 106, 294, White, Blue, Blue},
    {"11", 0, 106, 178, White, Red, 0xF81F},
    {":", 0, 106, 146, Blue, White, White},
    {"11", 0, 106, 130, White, Red, 0xF81F},
    {":", 0, 106, 98, Blue, White, White},
    {"11", 0, 106, 82, White, Red, 0xF81F},
    {"Week:", 0, 151, 294, White, Blue, Blue},
    {"  Friday  ", 0, 151, 194, White, Red, 0xF81F},
    {"  Refresh Time  ", 0, 196, 294, White, Blue, 0xF81F}
};

static int clock_page_draw_label(pt_clock_label clock_label)
{
    if (clock_label->select_state == 0)
    {
        LCD_SetBackColor(clock_label->back_color);
    }
    else
    {
        LCD_SetBackColor(clock_label->click_back_color);
    }
    LCD_SetTextColor(clock_label->label_color);    
    LCD_DisplayStringWithPos(clock_label->x_pos, clock_label->y_pos, (uint8_t *)clock_label->name);

    return 0;
}

static int clock_page_draw_labels(pt_clock_label clock_label, uint8_t size)
{
    uint8_t i = 0;

    for (i = 0; i < size; i++)
    {
        clock_page_draw_label(&clock_label[i]);
    }

    return 0;
}

static int clock_page_draw_change_labels(pt_clock_label clock_label, uint8_t size)
{
    uint8_t i = 0;

    for (i = 0; i < size; i++)
    {
        if (i == CLOCK_YEAR_LABEL_INDEX
            || i == CLOCK_MONTH_LABEL_INDEX
            || i == CLOCK_DATE_LABEL_INDEX
            || i == CLOCK_HOUR_LABEL_INDEX
            || i == CLOCK_MINUTE_LABEL_INDEX
            || i == CLOCK_SECOND_LABEL_INDEX
            || i == CLOCK_WEEK_LABEL_INDEX)
        {
            clock_page_draw_label(&clock_label[i]);
        }
    }

    return 0;
}

static int clock_refresh_labels(pt_clock_label clock_label, uint8_t size)
{
    memset(g_clock_label[CLOCK_YEAR_LABEL_INDEX].name, '\0', CLOCK_MAX_NAME_SIZE);
    memset(g_clock_label[CLOCK_MONTH_LABEL_INDEX].name, '\0', CLOCK_MAX_NAME_SIZE);
    memset(g_clock_label[CLOCK_DATE_LABEL_INDEX].name, '\0', CLOCK_MAX_NAME_SIZE);
    memset(g_clock_label[CLOCK_HOUR_LABEL_INDEX].name, '\0', CLOCK_MAX_NAME_SIZE);
    memset(g_clock_label[CLOCK_MINUTE_LABEL_INDEX].name, '\0', CLOCK_MAX_NAME_SIZE);
    memset(g_clock_label[CLOCK_SECOND_LABEL_INDEX].name, '\0', CLOCK_MAX_NAME_SIZE);
    memset(g_clock_label[CLOCK_WEEK_LABEL_INDEX].name, '\0', CLOCK_MAX_NAME_SIZE);
                
    sprintf(g_clock_label[CLOCK_YEAR_LABEL_INDEX].name, "20%02d", g_date.Year);
    sprintf(g_clock_label[CLOCK_MONTH_LABEL_INDEX].name, "%02d", g_date.Month);
    sprintf(g_clock_label[CLOCK_DATE_LABEL_INDEX].name, "%02d", g_date.Date);
    sprintf(g_clock_label[CLOCK_HOUR_LABEL_INDEX].name, "%02d", g_time.Hours);
    sprintf(g_clock_label[CLOCK_MINUTE_LABEL_INDEX].name, "%02d", g_time.Minutes);
    sprintf(g_clock_label[CLOCK_SECOND_LABEL_INDEX].name, "%02d", g_time.Seconds);
    strcpy(g_clock_label[CLOCK_WEEK_LABEL_INDEX].name, g_week_str[g_date.WeekDay - 1]);
    
    clock_page_draw_change_labels(clock_label, size);

    return 0;
}

static void clock_page_run(void)
{
    uint8_t key_down_index = 0;
    uint8_t label_num = sizeof(g_clock_label)/sizeof(g_clock_label[0]);
    uint8_t change_label_num = sizeof(change_label_index_map)/sizeof(change_label_index_map[0]);

    LCD_SetDisplayWindow(239, 319, 240, 320);

    LCD_Clear(White);

    main_page_draw_logo();
    
    LCD_SetDisplayWindow(239, 319, 240, 320);
    
    clock_page_draw_labels(g_clock_label, label_num);
    
    HAL_RTC_GetDate(&hrtc, &g_date, RTC_FORMAT_BIN);
    HAL_RTC_GetTime(&hrtc, &g_time, RTC_FORMAT_BIN);

    while(1)
    {
        key_scan(&key_down_index);

        if (key_down_index == 1)
        {
            select_label_old_index = select_label_index;
            select_label_index = (select_label_index + 1) % change_label_num;

            if (select_label_old_index != -1)
            {
                g_clock_label[change_label_index_map[select_label_old_index]].select_state = 0;
                clock_page_draw_label(&g_clock_label[change_label_index_map[select_label_old_index]]);
            }

            g_clock_label[change_label_index_map[select_label_index]].select_state = 1;
            clock_page_draw_label(&g_clock_label[change_label_index_map[select_label_index]]);
        }
        else if (key_down_index == 2)
        {
            if (select_label_index == -1)
            {
                select_label_old_index = select_label_index;
                select_label_index = change_label_num - 1;
            }
            else
            {
                select_label_old_index = select_label_index;
                select_label_index = select_label_index - 1;
                if (select_label_index < 0)
                {
                    select_label_index = change_label_num - 1;
                }
            }

            if (select_label_old_index != -1)
            {
                g_clock_label[change_label_index_map[select_label_old_index]].select_state = 0;
                clock_page_draw_label(&g_clock_label[change_label_index_map[select_label_old_index]]);
            }

            g_clock_label[change_label_index_map[select_label_index]].select_state = 1;
            clock_page_draw_label(&g_clock_label[change_label_index_map[select_label_index]]);

        }
        else if (key_down_index == 3)
        {
            switch(select_label_index)
            {
                case 0:
                    g_date.Year += 1;
                    if (g_date.Year > 30)
                    {
                        g_date.Year = 20;
                    }
                    memset(g_clock_label[CLOCK_YEAR_LABEL_INDEX].name, '\0', CLOCK_MAX_NAME_SIZE);
                    sprintf(g_clock_label[CLOCK_YEAR_LABEL_INDEX].name, "20%02d", g_date.Year);
                    clock_page_draw_label(&g_clock_label[change_label_index_map[select_label_index]]);
                    break;
                case 1:
                    g_date.Month += 1;
                    if (g_date.Month > 12)
                    {
                        g_date.Month = 1;
                    }
                    memset(g_clock_label[CLOCK_MONTH_LABEL_INDEX].name, '\0', CLOCK_MAX_NAME_SIZE);
                    sprintf(g_clock_label[CLOCK_MONTH_LABEL_INDEX].name, "%02d", g_date.Month);
                    clock_page_draw_label(&g_clock_label[change_label_index_map[select_label_index]]);
                    break;
                case 2:
                    g_date.Date += 1;
                    if (g_date.Date > 31)
                    {
                        g_date.Date = 1;
                    }
                    memset(g_clock_label[CLOCK_DATE_LABEL_INDEX].name, '\0', CLOCK_MAX_NAME_SIZE);
                    sprintf(g_clock_label[CLOCK_DATE_LABEL_INDEX].name, "%02d", g_date.Date);
                    clock_page_draw_label(&g_clock_label[change_label_index_map[select_label_index]]);
                    break;
                case 3:
                    g_time.Hours += 1;
                    if (g_time.Hours >= 24)
                    {
                        g_time.Hours = 0;
                    }
                    memset(g_clock_label[CLOCK_HOUR_LABEL_INDEX].name, '\0', CLOCK_MAX_NAME_SIZE);
                    sprintf(g_clock_label[CLOCK_HOUR_LABEL_INDEX].name, "%02d", g_time.Hours);
                    clock_page_draw_label(&g_clock_label[change_label_index_map[select_label_index]]);
                    break;
                case 4:
                    g_time.Minutes += 1;
                    if (g_time.Minutes >= 60)
                    {
                        g_time.Minutes = 0;
                    }
                    memset(g_clock_label[CLOCK_MINUTE_LABEL_INDEX].name, '\0', CLOCK_MAX_NAME_SIZE);
                    sprintf(g_clock_label[CLOCK_MINUTE_LABEL_INDEX].name, "%02d", g_time.Minutes);
                    clock_page_draw_label(&g_clock_label[change_label_index_map[select_label_index]]);
                    break;
                case 5:
                    g_time.Seconds += 1;
                    if (g_time.Seconds >= 60)
                    {
                        g_time.Seconds = 0;
                    }
                    memset(g_clock_label[CLOCK_SECOND_LABEL_INDEX].name, '\0', CLOCK_MAX_NAME_SIZE);
                    sprintf(g_clock_label[CLOCK_SECOND_LABEL_INDEX].name, "%02d", g_time.Seconds);
                    clock_page_draw_label(&g_clock_label[change_label_index_map[select_label_index]]);
                    break;
                case 6:
                    g_date.WeekDay += 1;
                    if (g_date.WeekDay > 7)
                    {
                        g_date.WeekDay = 1;
                    }
                    memset(g_clock_label[CLOCK_WEEK_LABEL_INDEX].name, '\0', CLOCK_MAX_NAME_SIZE);
                    strcpy(g_clock_label[CLOCK_WEEK_LABEL_INDEX].name, g_week_str[g_date.WeekDay - 1]);
                    clock_page_draw_label(&g_clock_label[change_label_index_map[select_label_index]]);
                    break;
                case 7:
                    HAL_RTC_SetDate(&hrtc, &g_date, RTC_FORMAT_BIN);
                    HAL_RTC_SetTime(&hrtc, &g_time, RTC_FORMAT_BIN);
                    g_clock_label[change_label_index_map[select_label_index]].select_state = 0;
                    clock_page_draw_label(&g_clock_label[change_label_index_map[select_label_index]]);
                    select_label_index = -1;
                    select_label_old_index = -1;
                    break;
                default:
                    break;
            }
        }
        else if (key_down_index == 4)
        {
            break;
        }

        if (select_label_index != -1)
        {
            g_change_flag = 1;
        }
        else
        {
            g_change_flag = 0;
        }

        if (g_change_flag == 0)
        {
            HAL_RTC_GetDate(&hrtc, &g_date, RTC_FORMAT_BIN);
            HAL_RTC_GetTime(&hrtc, &g_time, RTC_FORMAT_BIN);
            clock_refresh_labels(g_clock_label, label_num);
        }
    }
}

static t_page g_clock_page = {
    .name = "CLOCK",
    .run  = clock_page_run,
    .next = NULL
};

int clock_page_register(void)
{
    return page_register(&g_clock_page);
}
