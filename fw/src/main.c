
#include <stdio.h>
#include <stdlib.h>

#include "gu25664drv.h"
#include "ugui.h"

UG_GUI gui;

void driver_adapter (UG_S16 x,UG_S16 y, UG_COLOR color)
{
    gu25664drv_draw_point((int)x, (int)y, (int)color);
}

void win_callback (UG_MESSAGE *p_msg)
{

}

int main(int argc, char* argv[])
{
//    int i;
//
//    gu25664drv_init();
//
//    for (int x = 0; x < 256; x++) {
//        for (int y = 0; y < 64; y++) {
//            gu25664drv_draw_point(x, y, 1);
//        }
//    }
//
//    for (int x = 0; x < 256; x++) {
//        for (int y = 0; y < 64; y++) {
//            gu25664drv_draw_point(x, y, 0);
//        }
//    }

    gu25664drv_init();

    UG_Init(&gui, driver_adapter, 256, 64);

    UG_FontSelect(&FONT_6X8);
    UG_PutString(2, 2, "Gugugugugugu World.");

    UG_FontSelect(&FONT_24X40);
    UG_PutString(10, 12, "THE WORLD");

//    UG_WINDOW win;
//    UG_BUTTON button;
//    UG_OBJECT objbuffer[10];
//
//    UG_WindowCreate(&win, objbuffer, 10, win_callback);
//    UG_ButtonCreate(&win, &button, BTN_ID_0, 10, 10, 100, 60);
//
//    UG_WindowShow(&win);
//
//    while(1) {
//        UG_Update();
//    }
}
