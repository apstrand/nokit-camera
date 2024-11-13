/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <memory.h>
#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_log.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_camera.h"

#include "lvgl.h"

#include "bsp/esp32s3_wroom_cam.h"

#define TAG "dcam1"

extern "C" void app_main(void)
{
    printf("Hello world!\n");

    bsp_info();

    bsp_i2c_init();
    bsp_display_start();
    bsp_display_backlight_on(); // Set display brightness to 100%

    // Initialize the camera
    const camera_config_t camera_config = BSP_CAMERA_DEFAULT_CONFIG;
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return;
    }
    sensor_t *s = esp_camera_sensor_get();
    s->set_vflip(s, 1);
    s->set_hmirror(s, 0);

    uint32_t cam_buff_size = BSP_LCD_H_RES * BSP_LCD_V_RES * 2;
    uint8_t *cam_buff = (uint8_t *)heap_caps_malloc(cam_buff_size, MALLOC_CAP_SPIRAM);
    assert(cam_buff);

    // Create LVGL canvas for camera image
    bsp_display_lock(0);
    lv_obj_t *camera_canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(camera_canvas, cam_buff, BSP_LCD_H_RES, BSP_LCD_V_RES, LV_COLOR_FORMAT_RGB565);
    assert(camera_canvas);
    lv_obj_center(camera_canvas);
    bsp_display_unlock();

    camera_fb_t *pic;
    while (1)
    {
        pic = esp_camera_fb_get();
        if (pic)
        {
            esp_camera_fb_return(pic);
            bsp_display_lock(0);
            memcpy(cam_buff, pic->buf, cam_buff_size);
            if (BSP_LCD_BIGENDIAN)
            {
                /* Swap bytes in RGB565 */
                lv_draw_sw_rgb565_swap(cam_buff, cam_buff_size);
            }
            lv_obj_invalidate(camera_canvas);
            bsp_display_unlock();
        }
        else
        {
            ESP_LOGE(TAG, "Get frame failed");
        }
        vTaskDelay(1);
    }
}
