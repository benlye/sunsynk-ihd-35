#include <Arduino.h>
#include <lvgl.h>
#include <XPT2046_Touchscreen.h>
#include "DateTime.h"
#include "Touch.h"

bool touch_swap_xy = true;
int16_t touch_map_x1 = TOUCH_CAL_X1;
int16_t touch_map_x2 = TOUCH_CAL_X2;
int16_t touch_map_y1 = TOUCH_CAL_Y1;
int16_t touch_map_y2 = TOUCH_CAL_Y2;

int16_t touch_max_x = 0, touch_max_y = 0;
int16_t touch_raw_x = 0, touch_raw_y = 0;
int16_t touch_last_x = 0, touch_last_y = 0;

#ifdef TOUCH_XPT2046
XPT2046_Touchscreen ts(TOUCH_XPT2046_CS, TOUCH_XPT2046_INT);
#endif

void touch_init(int16_t w, int16_t h, uint8_t r)
{
    touch_max_x = w - 1;
    touch_max_y = h - 1;
    if (touch_map_x1 == -1)
    {
        switch (r)
        {
        case 3:
            touch_swap_xy = true;
            touch_map_x1 = touch_max_x;
            touch_map_x2 = 0;
            touch_map_y1 = 0;
            touch_map_y2 = touch_max_y;
            break;
        case 2:
            touch_swap_xy = false;
            touch_map_x1 = touch_max_x;
            touch_map_x2 = 0;
            touch_map_y1 = touch_max_y;
            touch_map_y2 = 0;
            break;
        case 1:
            touch_swap_xy = true;
            touch_map_x1 = 0;
            touch_map_x2 = touch_max_x;
            touch_map_y1 = touch_max_y;
            touch_map_y2 = 0;
            break;
        default: // case 0:
            touch_swap_xy = false;
            touch_map_x1 = 0;
            touch_map_x2 = touch_max_x;
            touch_map_y1 = 0;
            touch_map_y2 = touch_max_y;
            break;
        }
    }

    SPI.begin(TOUCH_XPT2046_SCK, TOUCH_XPT2046_MISO, TOUCH_XPT2046_MOSI, TOUCH_XPT2046_CS);
    ts.begin();
    ts.setRotation(TOUCH_XPT2046_ROTATION);
}

bool touch_has_signal()
{
    return ts.tirqTouched();
    return false;
}

void translate_touch_raw()
{
    if (touch_swap_xy)
    {
        touch_last_x = map(touch_raw_y, touch_map_x1, touch_map_x2, 0, touch_max_x);
        touch_last_y = map(touch_raw_x, touch_map_y1, touch_map_y2, 0, touch_max_y);
    }
    else
    {
        touch_last_x = map(touch_raw_x, touch_map_x1, touch_map_x2, 0, touch_max_x);
        touch_last_y = map(touch_raw_y, touch_map_y1, touch_map_y2, 0, touch_max_y);
    }
    //Serial.printf("touch_raw_x: %d, touch_raw_y: %d, touch_last_x: %d, touch_last_y: %d\n", touch_raw_x, touch_raw_y, touch_last_x, touch_last_y);
}

bool touch_touched()
{
    if (ts.touched())
    {
        TS_Point p = ts.getPoint();
        touch_raw_x = p.x;
        touch_raw_y = p.y;
        int max_z = p.z;
        int count = 0;
        while ((ts.touched()) && (count < TOUCH_XPT2046_SAMPLES))
        {
            count++;

            TS_Point p = ts.getPoint();
            if (p.z > max_z)
            {
                touch_raw_x = p.x;
                touch_raw_y = p.y;
                max_z = p.z;
            }
            //Serial.printf("touch_raw_x: %d, touch_raw_y: %d, p.z: %d\n", touch_raw_x, touch_raw_y, p.z);
        }
        translate_touch_raw();
        lastTouchTime = getTime();
        return true;
    }
    return false;
}

bool touch_released()
{
    return true;
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  if (touch_has_signal())
  {
    if (touch_touched())
    {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;
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
