
#include <stdlib.h>
#include <string.h>
#include "logo.h"
#include "lcd.h"
#include "page_manager.h"

t_icon icon_logo = {"logo", 0, 40, 319, 40, 160, whtcc_logo, 11840};

int main_page_draw_logo(void)
{
    LCD_DrawPictureWithPos(icon_logo.p_logo,
                        icon_logo.logo_size/2,
                        icon_logo.x_pos,
                        icon_logo.y_pos,
                        icon_logo.height,
                        icon_logo.weight, 0);

    return 0;
}

pt_page g_pt_page = NULL;

int page_register(pt_page page)
{
    if (g_pt_page == NULL)
    {
        g_pt_page = page;
    }
    else
    {
        page->next = g_pt_page;
        g_pt_page = page;
    }

    return 0;
}

pt_page page_get(char *page_name)
{
    pt_page pt_temp = g_pt_page;

    while(pt_temp)
    {
        if (strcmp(pt_temp->name, page_name) == 0)
        {
            return pt_temp;
        }
        pt_temp = pt_temp->next;
    }

    return NULL;
}

int page_init(void)
{
    main_page_register();
    led_page_register();
    adc_page_register();
    pwm_page_register();
    rom_page_register();
    clock_page_register();
    ai_page_register();

    return 0;
}
