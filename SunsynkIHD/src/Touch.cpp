#include <Arduino.h>
#include <lvgl.h>
#include <Displays.h>
#include "DateTime.h"
#include "Graphics.h"
#include "Touch.h"

int16_t touch_last_x = 0, touch_last_y = 0;

void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
    uint16_t touchX, touchY;
    data->state = LV_INDEV_STATE_REL;
    if(gfx->getTouchRaw( &touchX, &touchY ))
    {
        
#if defined(TOUCH_SWAP_XY)
        touch_last_x = map(touchY, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, SCREEN_WIDTH - 1);
        touch_last_y = map(touchX, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, SCREEN_HEIGHT - 1);
#else
        touch_last_x = map(touchX, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, SCREEN_WIDTH - 1);
        touch_last_y = map(touchY, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, SCREEN_HEIGHT - 1);
#endif
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch_last_x;
        data->point.y = touch_last_y;

        //Serial.printf("Raw:   X:%d Y:%d\n", touchX, touchY);
        Serial.printf("Touch: X:%d Y:%d\n", data->point.x, data->point.y);
        lastTouchTime = getTime();
    }
}
