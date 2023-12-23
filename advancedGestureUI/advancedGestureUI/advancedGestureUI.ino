/**
 * Advanced UI controls for the Desk IoT Board
 *
 * Author: Aaron S. Crandall <crandall@gonzaga.edu>
 * Copyright: 2023
 * License: GPLv3
 *
 */

#include <TaskScheduler.h>
#include "Wire.h"
#include "SHT2x.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_wemos.h>

#include <Adafruit_NeoPixel.h>
#define LED_PIN     D8
#define LED_COUNT   14
#define LED_BRIGHTNESS 200
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

#include "TemperatureData.h"
#include "TemperatureDataView.h"



// Callback methods prototypes
void tLED_BlinkCallback();
void tRainbowLightsCallback();
void tEnvSenseCallback();

Task tLED_Blink(1000, TASK_FOREVER, &tLED_BlinkCallback);
Task tRainbowLights(50, TASK_FOREVER, &tRainbowLightsCallback);
Task tEnvSense(600000, TASK_FOREVER, &tEnvSenseCallback);


Scheduler runner;

bool g_LEDStatus = false;
long g_firstPixelHue = 0;

// ******* OLED display
#define OLED_RESET 0
Adafruit_SSD1306 display(OLED_RESET);

// ******* TEMP / Humidity stuff
SHT2x sht;
TemperatureData tempData;
TemperatureDataView tempDataView(&display, &tempData);



#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


// *****************************************************
void tLED_BlinkCallback() {
  if( g_LEDStatus ) {
    led_on();
  } else {
    led_off();
  }
  g_LEDStatus = !g_LEDStatus;
}

void tRainbowLightsCallback() {
  g_firstPixelHue += 256;
  g_firstPixelHue %= 5*65536;

  strip.rainbow(g_firstPixelHue);
  strip.show();
}

void tEnvSenseCallback() {
  sht.read();
  //Serial.print("Temp: ");
  //Serial.println(sht.getTemperature(), 2);
  tempData.add_sample(sht.getTemperature());
  tempData.dump();

  tempDataView.render();
}


// *****************************************************
void led_blink() {
  led_on();
  delay(500);
  led_off();
}
void led_on() {
  digitalWrite(LED_BUILTIN, LOW);
}
void led_off() {
  digitalWrite(LED_BUILTIN, HIGH);
}


// *****************************************************
void initialize_hardware() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  led_off();

  strip.begin();
  strip.show();
  strip.setBrightness(LED_BRIGHTNESS);

  sht.begin();  // Fire up temp sensor

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // OLED setup
  display.clearDisplay(); // for Clearing the display
  display.display();
}


// **************************************************************************
void setup() {
  initialize_hardware();

  // Setup the task scheduler
  runner.init();
  runner.addTask(tLED_Blink);
  runner.addTask(tRainbowLights);
  runner.addTask(tEnvSense);

  //tLED_Blink.enable();
  tRainbowLights.enable();
  tEnvSense.enable();

  tempData.init();

  display.setTextSize(1);
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.print("ASC Inc.");
  display.display();
  display.stopscroll();

  delay(1000);

  tEnvSenseCallback();  // Call once so there's data right away

}


// *************************************************************************
//  loop only executes the task scheduler
void loop() {
  runner.execute();
}

