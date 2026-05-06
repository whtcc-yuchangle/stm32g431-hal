
#ifndef __PAGE_MANAGER_H__
#define __PAGE_MANAGER_H__

#include "main.h"

typedef struct _icon
{
    char *name;
    uint8_t index;
    uint16_t x_pos;
    uint16_t y_pos;
    uint16_t height;
    uint16_t weight;
    const unsigned char *p_logo;
    uint32_t logo_size;
}*pt_icon, t_icon;

typedef struct _page
{
    char *name;
    void (*run)(void);
    struct _page *next;
}*pt_page, t_page;

int main_page_draw_logo(void);

int page_register(pt_page page);
pt_page page_get(char *page_name);

int main_page_register(void);
int led_page_register(void);
int adc_page_register(void);
int pwm_page_register(void);
int rom_page_register(void);
int clock_page_register(void);
int ai_page_register(void);

int page_init(void);

#endif  // __PAGE_MANAGER_H__
