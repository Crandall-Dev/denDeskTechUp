/*
  Example Script: paj7620_9_gestures.ino
  Package: RevEng_PAJ7620

  Copyright (c) 2020 Aaron S. Crandall
  Author        : Aaron S. Crandall <crandall@gonzaga.edu>
  Modified Time : December 2020

  Description: This demo can recognize 9 gestures and output the result,
    including move up, move down, move left, move right, move forward,
    move backward, circle-clockwise, circle-counter clockwise, and wave.

  License: Same as package under MIT License (MIT)
*/

// Includes enum definition of GES_* return values from readGesture()
#include "RevEng_PAJ7620.h"

#include "Adafruit_HTU21DF.h"

// Create gesture sensor driver object
RevEng_PAJ7620 sensor = RevEng_PAJ7620();

Adafruit_HTU21DF htu = Adafruit_HTU21DF();


// *********************************************************************
void setup()
{
  Serial.begin(115200);

  Serial.println("PAJ7620 sensor demo: Recognizing all 9 gestures.");

  if( !sensor.begin() )           // return value of 0 == success
  {
    Serial.print("PAJ7620 I2C error - halting");
    while(true) { }
  }

  if (!htu.begin()) {
    Serial.println("HTU21 - Couldn't find sensor!");
    while (1);
  }

  Serial.println("PAJ7620 init: OK");
  Serial.println("Please input your gestures:");
  sensor.invertYAxis();
  sensor.invertXAxis();
}


// *********************************************************************
void loop()
{
  Gesture gesture;                  // Gesture is an enum type from RevEng_PAJ7620.h
  gesture = sensor.readGesture();   // Read back current gesture (if any) of type Gesture

  switch (gesture)
  {
    case GES_FORWARD:
      {
        Serial.print("GES_FORWARD");
        break;
      }

    case GES_BACKWARD:
      {
        Serial.print("GES_BACKWARD");
        break;
      }

    case GES_LEFT:
      {
        Serial.println("GES_LEFT");
        float temp = htu.readTemperature();
        float rel_hum = htu.readHumidity();
        Serial.print("Temp: "); Serial.print(temp); Serial.print(" C");
        Serial.print("\t\t");
        Serial.print("Humidity: "); Serial.print(rel_hum); Serial.println(" \%");
        break;
      }

    case GES_RIGHT:
      {
        Serial.print("GES_RIGHT");
        break;
      }

    case GES_UP:
      {
        Serial.print("GES_UP");
        break;
      }

    case GES_DOWN:
      {
        Serial.print("GES_DOWN");
        break;
      }

    case GES_CLOCKWISE:
      {
        Serial.print("GES_CLOCKWISE");
        break;
      }

    case GES_ANTICLOCKWISE:
      {
        Serial.print("GES_ANTICLOCKWISE");
        break;
      }

    case GES_WAVE:
      {
        Serial.print("GES_WAVE");
        break;
      }

    case GES_NONE:
      {
        break;
      }
  }

  if( gesture != GES_NONE )
  {
    Serial.print(", Code: ");
    Serial.println(gesture);
  }

  delay(100);
}
