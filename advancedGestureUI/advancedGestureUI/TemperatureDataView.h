
#ifndef TEMP_DATA_VIEW_H
#define TEMP_DATA_VIEW_H

#include <math.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_wemos.h>

#include "TemperatureData.h"

class TemperatureDataView {
  Adafruit_SSD1306 * _display;
  TemperatureData * _temperatureData;

public:
  TemperatureDataView(Adafruit_SSD1306 * oled_display, TemperatureData * temperatureData) {
    _display = oled_display;
    _temperatureData = temperatureData;
  }

  void render() {
    _display->clearDisplay();

    render_temperature_range();
    render_temperature_samples();

    _display->display();
  }

  void render_temperature_range() {
    int temp_floor_C = (int)_temperatureData->get_min_temp();
    int temp_ceil_C = ceil(_temperatureData->get_max_temp());
    if(temp_floor_C == temp_ceil_C) { temp_ceil_C++; } // Avoid #div/0

    _display->setTextSize(1);
    _display->setCursor(0,0);
    _display->print(temp_ceil_C);
    _display->setCursor(0,40);
    _display->print(temp_floor_C);
  }

  void render_temperature_samples() {
    int temp_floor_C = (int)_temperatureData->get_min_temp();
    int temp_ceil_C = ceil(_temperatureData->get_max_temp());
    if(temp_floor_C == temp_ceil_C) { temp_ceil_C++; } // Avoid #div/0

//    Serial.print(temp_floor_C);
//    Serial.print("\t");
//    Serial.println(temp_ceil_C);

    float temp_range_C = temp_ceil_C - temp_floor_C;
    int num_pixels_y = 40;
    int start_pixel_x = 10;
    int baseline_pixel_y = 46;
    int baseline_pixel_x = 64;

//    float deg_C_per_pixel_y = temp_range_C / num_pixels_y;
    float pixels_y_per_deg_C = num_pixels_y / temp_range_C;

    for( int sample_index = 0; sample_index < _temperatureData->get_sample_count(); sample_index++ ) {
      float curr_temp_sample = _temperatureData->get_sample_at(sample_index);

      float temp_val_offset = curr_temp_sample - temp_floor_C;
      int pixels_off_baseline_y = pixels_y_per_deg_C * temp_val_offset;
      
      int pixel_y = baseline_pixel_y - pixels_off_baseline_y;

      int pixel_x = baseline_pixel_x - sample_index;

      _display->drawPixel(pixel_x, pixel_y, WHITE);
      
    }

  }


};






#endif