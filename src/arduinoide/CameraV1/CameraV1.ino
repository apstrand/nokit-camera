/**
 * | Supported ESP SoCs | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
 * | ------------------ | ----- | -------- | -------- | -------- | -------- | -------- |
 *
 * | Supported LCD Controllers | GC9A01 | GC9B71 | ILI9341 | NV3022B | SH8601 | SPD2010 | ST7789 | ST77916 | ST77922 |
 * | ------------------------- | ------ | ------ | ------- | ------- | ------ | ------- | ------ | ------- | ------- |
 *
 * # SPI LCD Example
 *
 * The example demonstrates how to develop different model LCDs with SPI interface using standalone drivers and test them by displaying color bars.
 *
 * ## How to use
 *
 * 1. [Configure drivers](https://github.com/esp-arduino-libs/ESP32_Display_Panel#configuring-drivers) if needed.
 * 2. Modify the macros in the example to match the parameters according to your hardware.
 * 3. Navigate to the `Tools` menu in the Arduino IDE to choose a ESP board and configure its parameters, please refter to [Configuring Supported Development Boards](https://github.com/esp-arduino-libs/ESP32_Display_Panel#configuring-supported-development-boards)
 * 4. Verify and upload the example to your ESP board.
 *
 * ## Serial Output
 *
 * ```
 * ...
 * SPI LCD example start
 * Initialize backlight control pin and turn it off
 * Create SPI LCD bus
 * Create LCD device
 * Draw color bar from top left to bottom right, the order is B - G - R
 * Draw bitmap finish callback
 * Draw bitmap finish callback
 * Draw bitmap finish callback
 * Draw bitmap finish callback
 * Draw bitmap finish callback
 * Draw bitmap finish callback
 * Draw bitmap finish callback
 * Draw bitmap finish callback
 * Draw bitmap finish callback
 * Draw bitmap finish callback
 * Draw bitmap finish callback
 * Draw bitmap finish callback
 * Draw bitmap finish callback
 * Draw bitmap finish callback
 * Draw bitmap finish callback
 * Draw bitmap finish callback
 * Turn on the backlight
 * SPI LCD example end
 * IDLE loop
 * ...
 * ```
 *
 * ## Troubleshooting
 *
 * Please check the [FAQ](https://github.com/esp-arduino-libs/ESP32_Display_Panel#faq) first to see if the same question exists. If not, please create a [Github issue](https://github.com/esp-arduino-libs/ESP32_Display_Panel/issues). We will get back to you as soon as possible.
 *
 */

#include <Arduino.h>
#include <ESP_Panel_Library.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Currently, the library supports the following SPI LCDs:
 *      - GC9A01, GC9B71
 *      - ILI9341
 *      - NV3022B
 *      - SH8601
 *      - SPD2010
 *      - ST7789, ST77916, ST77922
 */
#define EXAMPLE_LCD_NAME                ST7789
#define EXAMPLE_LCD_WIDTH               (240)
#define EXAMPLE_LCD_HEIGHT              (320)
#define EXAMPLE_LCD_COLOR_BITS          (16)
#define EXAMPLE_LCD_SPI_FREQ_HZ         (40 * 1000 * 1000)
#define EXAMPLE_LCD_USE_EXTERNAL_CMD    (0)
#if EXAMPLE_LCD_USE_EXTERNAL_CMD
/**
 * LCD initialization commands.
 *
 * Vendor specific initialization can be different between manufacturers, should consult the LCD supplier for
 * initialization sequence code.
 *
 * Please uncomment and change the following macro definitions, then use `configVendorCommands()` to pass them in the
 * same format if needed. Otherwise, the LCD driver will use the default initialization sequence code.
 *
 * There are two formats for the sequence code:
 *   1. Raw data: {command, (uint8_t []){ data0, data1, ... }, data_size, delay_ms}
 *   2. Formater: ESP_PANEL_LCD_CMD_WITH_8BIT_PARAM(delay_ms, command, { data0, data1, ... }) and
 *                ESP_PANEL_LCD_CMD_WITH_NONE_PARAM(delay_ms, command)
 */
const esp_lcd_panel_vendor_init_cmd_t lcd_init_cmd[] = {
    // {0xFF, (uint8_t []){0x77, 0x01, 0x00, 0x00, 0x10}, 5, 0},
    // {0xC0, (uint8_t []){0x3B, 0x00}, 2, 0},
    // {0xC1, (uint8_t []){0x0D, 0x02}, 2, 0},
    // {0x29, (uint8_t []){0x00}, 0, 120},
    // // or
    // ESP_PANEL_LCD_CMD_WITH_8BIT_PARAM(0, 0xFF, {0x77, 0x01, 0x00, 0x00, 0x10}),
    // ESP_PANEL_LCD_CMD_WITH_8BIT_PARAM(0, 0xC0, {0x3B, 0x00}),
    // ESP_PANEL_LCD_CMD_WITH_8BIT_PARAM(0, 0xC1, {0x0D, 0x02}),
    // ESP_PANEL_LCD_CMD_WITH_NONE_PARAM(120, 0x29),
};
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your board spec ////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_LCD_PIN_NUM_SPI_CS      (21)
#define EXAMPLE_LCD_PIN_NUM_SPI_DC      (47)
#define EXAMPLE_LCD_PIN_NUM_SPI_SCK     (41)
#define EXAMPLE_LCD_PIN_NUM_SPI_SDA     (42)
#define EXAMPLE_LCD_PIN_NUM_SPI_SDO     (-1)
#define EXAMPLE_LCD_PIN_NUM_RST         (2)    // Set to -1 if not used
#define EXAMPLE_LCD_PIN_NUM_BK_LIGHT    (1)    // Set to -1 if not used
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL   (1)

#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL

/* Enable or disable the attachment of a callback function that is called after each bitmap drawing is completed */
#define EXAMPLE_ENABLE_ATTACH_CALLBACK  (1)

#define _EXAMPLE_LCD_CLASS(name, ...)   ESP_PanelLcd_##name(__VA_ARGS__)
#define EXAMPLE_LCD_CLASS(name, ...)    _EXAMPLE_LCD_CLASS(name, ##__VA_ARGS__)

#if EXAMPLE_ENABLE_ATTACH_CALLBACK
IRAM_ATTR bool onDrawBitmapFinishCallback(void *user_data)
{
    // esp_rom_printf("Draw bitmap finish callback\n");

    return false;
}
#endif

ESP_PanelLcd *lcd;

#define CAMERA_MODEL_ESP32S3_EYE
#include "esp_camera.h"
#include "camera_pins.h"

void setup_camera() {

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 2;

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
    sensor_t *s = esp_camera_sensor_get();
s->set_vflip(s, 1);
Serial.print(s->pixformat);
Serial.println(" Camera Setup Done!");
}


#define SENSOR_SDA  20
#define SENSOR_SCL  19
#define SENSOR_IRQ  14

#include "TouchDrvFT6X36.hpp"


TouchDrvFT6X36 touch;


void touch_setup()
{
    pinMode(SENSOR_IRQ, INPUT);

    if (!touch.begin(Wire, FT6X36_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL)) {
        Serial.println("Failed to find FT6X36 - check your wiring!");
    }
    touch.interruptTrigger();
    Serial.println("Init 2 FT6X36 Sensor done!");
}

void setup()
{
    Serial.begin(115200);
    Serial.println("SPI LCD example start");

#if EXAMPLE_LCD_PIN_NUM_BK_LIGHT >= 0
    Serial.println("Initialize backlight control pin and turn it off");
    ESP_PanelBacklight *backlight = new ESP_PanelBacklight(EXAMPLE_LCD_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL, true);
    backlight->begin();
    backlight->off();
#endif

    Serial.println("Create SPI LCD bus");
    ESP_PanelBus_SPI *panel_bus = new ESP_PanelBus_SPI(EXAMPLE_LCD_PIN_NUM_SPI_CS, EXAMPLE_LCD_PIN_NUM_SPI_DC,
                                                       EXAMPLE_LCD_PIN_NUM_SPI_SCK, EXAMPLE_LCD_PIN_NUM_SPI_SDA,
                                                       EXAMPLE_LCD_PIN_NUM_SPI_SDO);
    panel_bus->configSpiFreqHz(EXAMPLE_LCD_SPI_FREQ_HZ);
    panel_bus->begin();

    Serial.println("Create LCD device");
    
    lcd = new EXAMPLE_LCD_CLASS(EXAMPLE_LCD_NAME, panel_bus, EXAMPLE_LCD_COLOR_BITS, EXAMPLE_LCD_PIN_NUM_RST);
#if EXAMPLE_LCD_USE_EXTERNAL_CMD
    // Configure external initialization commands, should called before `init()`
    lcd->configVendorCommands(lcd_init_cmd, sizeof(lcd_init_cmd)/sizeof(lcd_init_cmd[0]));
#endif
    lcd->init();
    lcd->reset();
    lcd->begin();
    lcd->displayOn();
#if EXAMPLE_ENABLE_ATTACH_CALLBACK
    lcd->attachRefreshFinishCallback(onDrawBitmapFinishCallback, NULL);
#endif

    Serial.println("Draw color bar from top left to bottom right, the order is B - G - R");
    // lcd->colorBarTest(EXAMPLE_LCD_WIDTH, EXAMPLE_LCD_HEIGHT);

#if EXAMPLE_LCD_PIN_NUM_BK_LIGHT >= 0
    Serial.println("Turn on the backlight");
    backlight->on();
#endif

    setup_camera();
    Serial.println("SPI LCD example end2");
    Serial.print("Total heap: ");
    Serial.println(ESP.getHeapSize());
    Serial.println();
    Serial.print("Free heap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println();
    Serial.print("Total PSRAM: ");
    Serial.print(ESP.getPsramSize());
    Serial.println();
    Serial.print("Free PSRAM: ");
    Serial.print(ESP.getFreePsram());
    Serial.println();
    Serial.println("SPI LCD example end2");

    touch_setup();
}

int once = 0;
uint16_t buf[320*240];

void blit(const uint16_t *src, uint16_t *dst, int width, int height) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      uint16_t pix = src[y*width+x];
      // pix = (pix >> 8 | ((pix&0xff)<<8));
      // uint16_t b = (pix & 0xf800) >> 11;
      // uint16_t g = (pix & 0x07e0) >> 5;
      // uint16_t r = (pix & 0x001f);
      uint16_t r = (pix >> 6) & 0x1f;
      uint16_t b = (pix >> 11) & 0x1f;
      uint16_t g = pix & 0x3f;
      // dst[x*height+y] = ~(b<<11 | g<<5 | r);
        dst[x*height+y] = ~(b<<11 | r<<6 | g);
        if (0 && y > height/2) {
      if (((y/8) % 4) == 0) {
        dst[x*height+y] = ~0xf800;
      }
      if (((y/8) % 4) == 1) {
        dst[x*height+y] = ~0x07e0;
      }
      if (((y/8) % 4) == 2) {
        dst[x*height+y] = ~0x001f;
      }
      if (((y/8) % 4) == 3) {
        dst[x*height+y] = ~(0);
      }
        }
    }
  }
}
int once2 = 0;
int lastx = 0, lasty = 0;
void loop()
{
    if (once) 
      return;
    // Serial.println("loop");
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera fb failed!");
      once = 1;
    } else {
      if (!once2) {
        Serial.print("fmt ");
        Serial.print(fb->format);
        Serial.print(" len ");
        Serial.print(fb->len);
        Serial.print(" width ");
        Serial.print(fb->width);
        Serial.println();
        once2 = 1;
      }

      blit((uint16_t*)fb->buf, buf, 320, 240);
      lcd->drawBitmap(0, 0, 240, 320, (uint8_t*)buf);
      esp_camera_fb_return(fb);
      int16_t tx, ty;
      if (touch.getPoint(&tx, &ty, 1)) {
        Serial.print(tx);
        Serial.print(' ');
        Serial.print(ty);
        Serial.println();
      }
    }
}
