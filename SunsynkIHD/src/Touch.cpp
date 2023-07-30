#include <Arduino.h>
#include <lvgl.h>
#include "DateTime.h"
#include "Graphics.h"
#include "Touch.h"

int16_t touch_last_x = 0, touch_last_y = 0;

#ifdef TOUCH_XPT2046
#include <XPT2046_Touchscreen.h>
XPT2046_Touchscreen ts(TOUCH_XPT2046_CS, TOUCH_XPT2046_INT);
#endif

#ifdef TOUCH_GT911
#include <Wire.h>
#include <TAMC_GT911.h>
TAMC_GT911 ts = TAMC_GT911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, max(TOUCH_MAP_X1, TOUCH_MAP_X2), max(TOUCH_MAP_Y1, TOUCH_MAP_Y2));
#endif

#ifdef TOUCH_FT6X36
#include <FT6236.h>
FT6236 ts = FT6236();
#endif

void touch_init()
{
#if defined(TOUCH_GT911)
    Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
    ts.begin();
    ts.setRotation(TOUCH_GT911_ROTATION);
#elif defined(TOUCH_XPT2046)
    SPI.begin(TOUCH_XPT2046_SCK, TOUCH_XPT2046_MISO, TOUCH_XPT2046_MOSI, TOUCH_XPT2046_CS);
    ts.begin();
    ts.setRotation(TOUCH_XPT2046_ROTATION);
#endif
}

bool touch_has_signal()
{
#if defined(TOUCH_FT6X36)
    return true;
#elif defined(TOUCH_GT911)
    return true;
#elif defined(TOUCH_XPT2046)
    return ts.tirqTouched();
#else
    return false;
#endif
}

bool touch_touched()
{
#if defined(TOUCH_FT6X36)
    if (ts.touched())
    {
        TS_Point p = ts.getPoint();
#if defined(TOUCH_SWAP_XY)
        touch_last_x = map(p.y, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, SCREEN_WIDTH - 1);
        touch_last_y = map(p.x, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, SCREEN_HEIGHT - 1);
#else
        touch_last_x = map(p.x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, SCREEN_WIDTH - 1);
        touch_last_y = map(p.y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, SCREEN_HEIGHT - 1);
#endif
        lastTouchTime = getTime();
        return true;
    }
    else
    {
        return false;
    }

#elif defined(TOUCH_GT911)
    ts.read();
    if (ts.isTouched)
    {
#if defined(TOUCH_SWAP_XY)
        touch_last_x = map(ts.points[0].y, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, SCREEN_WIDTH - 1);
        touch_last_y = map(ts.points[0].x, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, SCREEN_HEIGHT - 1);
#else
        touch_last_x = map(ts.points[0].x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, SCREEN_WIDTH - 1);
        touch_last_y = map(ts.points[0].y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, SCREEN_HEIGHT - 1);
#endif
        lastTouchTime = getTime();
        return true;
    }
    else
    {
        return false;
    }

#elif defined(TOUCH_XPT2046)
    if (ts.touched())
    {
        TS_Point p = ts.getPoint();
#if defined(TOUCH_SWAP_XY)
        touch_last_x = map(p.y, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, SCREEN_WIDTH - 1);
        touch_last_y = map(p.x, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, SCREEN_HEIGHT - 1);
#else
        touch_last_x = map(p.x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, SCREEN_WIDTH - 1);
        touch_last_y = map(p.y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, SCREEN_HEIGHT - 1);
#endif
        lastTouchTime = getTime();
        return true;
    }
    else
    {
        return false;
    }

#else
    return false;
#endif
}

bool touch_released()
{
#if defined(TOUCH_FT6X36)
    return true;
#elif defined(TOUCH_GT911)
    return true;
#elif defined(TOUCH_XPT2046)
    return true;
#else
    return false;
#endif
}


/*Read the touchpad*/
/*
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    if (touch_has_signal())
    {
        if (touch_touched())
        {
            data->state = LV_INDEV_STATE_PR;

            data->point.x = touch_last_x;
            data->point.y = touch_last_y;
            Serial.printf("Touched: X:%d Y:%d\n", data->point.x, data->point.y);
        }
        else if (touch_released())
        {
            data->state = LV_INDEV_STATE_REL;
        }
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}
*/

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
        Serial.printf("Touched: X:%d Y:%d\n", data->point.x, data->point.y);
        lastTouchTime = getTime();
    }
}
