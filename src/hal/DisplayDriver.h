#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include <SD.h>
#include <SPI.h>
#include "pins.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <functional> 

class LGFX_Majin : public lgfx::LGFX_Device {
    lgfx::Panel_ILI9341 _panel_instance;
    lgfx::Bus_SPI       _bus_instance;
    lgfx::Light_PWM     _light_instance;
    lgfx::Touch_FT5x06  _touch_instance;

public:
    LGFX_Majin() {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host = SPI3_HOST; 
            cfg.spi_mode = 0;
            cfg.freq_write = 40000000;
            cfg.freq_read  = 16000000;
            cfg.spi_3wire  = true;
            cfg.use_lock   = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            
            cfg.pin_sclk = PIN_SCLK;
            cfg.pin_mosi = PIN_MOSI;
            cfg.pin_miso = PIN_MISO;
            cfg.pin_dc   = PIN_TFT_DC;
            
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs           = PIN_TFT_CS;
            cfg.pin_rst          = PIN_TFT_RST;
            cfg.pin_busy         = -1;
            cfg.panel_width      = 240;
            cfg.panel_height     = 320;
            cfg.offset_x         = 0;
            cfg.offset_y         = 0;
            cfg.offset_rotation  = 0;
            cfg.dummy_read_pixel = 8;
            cfg.readable         = true;
            cfg.invert           = true; 
            cfg.rgb_order        = false;
            cfg.dlen_16bit       = false;
            cfg.bus_shared       = true;

            _panel_instance.config(cfg);
        }

        {
            auto cfg = _light_instance.config();
            cfg.pin_bl = PIN_TFT_BL;
            cfg.invert = false;
            cfg.freq   = 5000;
            cfg.pwm_channel = 7;
            
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        {
            auto cfg = _touch_instance.config();
            cfg.x_min      = 0;
            cfg.x_max      = 319;
            cfg.y_min      = 0;
            cfg.y_max      = 319;
            
            cfg.pin_int    = PIN_FT_INT;
            cfg.pin_rst    = PIN_FT_RST;
            cfg.bus_shared = true;
            cfg.offset_rotation = 0;
            cfg.i2c_port = 1;
            cfg.i2c_addr = 0x38;
            cfg.pin_sda  = PIN_FT_SDA;
            cfg.pin_scl  = PIN_FT_SCL;
            cfg.freq     = 400000;
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
        setPanel(&_panel_instance);
    }
};

class DisplayDriver {
public:
    DisplayDriver();
    bool begin();
    LGFX_Majin* getLGFX();
    void secureDraw(std::function<void(LGFX_Majin*)> drawFunction);
    void setBrightness(uint8_t level);
    void logTouch();
    void setTouchCalibration(uint16_t* calData);

    /**
     * @brief Récupère les coordonnées tactiles de manière Thread-Safe
     * @param x pointeur vers variable x
     * @param y pointeur vers variable y
     * @return true si touché, false sinon
     */
    bool getTouchSafe(uint16_t* x, uint16_t* y);

private:
    LGFX_Majin _tft;
    bool _sdInitialized = false;
    bool _initSD();
    SemaphoreHandle_t _mutex;
};

#endif