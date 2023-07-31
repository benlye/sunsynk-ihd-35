#ifndef __TOUCH_H
#define __TOUCH_H

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

extern unsigned long lastTouchTime;

#endif // __TOUCH_H
