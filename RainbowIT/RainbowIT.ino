// A basic everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <RevEng_PAJ7620.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    D8

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 14

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


// Gesture Sensor!
RevEng_PAJ7620 sensor = RevEng_PAJ7620();

typedef enum {
  Off,
  Rainbow,
  White
} LightMode;

int currLightMode = Off;
int currWhiteBrightness = 150;
int minWhiteBrightness = 30;
int maxWhiteBrightness = 250;
int whiteBrightnessIncrements = 40;
int lastLightMode = Rainbow;


// setup() function -- runs once at startup --------------------------------

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(200); // Set BRIGHTNESS to about 1/5 (max = 255)

  if( !sensor.begin() )
  {
    Serial.print("PAJ7620 I2C error - halting");
    while(true) { }
  }
  
}


// *************************************************************************
void loop() {
  isNewLightMode();
  
  if( lastLightMode != currLightMode && currLightMode == Off ) {
    lastLightMode = currLightMode;
    colorWipe(strip.Color(0,   0,   0), 125);
  } else if( currLightMode == Rainbow ) {
    lastLightMode = currLightMode;
    rainbow(10);
  } else if( lastLightMode != currLightMode && currLightMode == White ) {
    lastLightMode = currLightMode;
    colorWipe(strip.Color(currWhiteBrightness, currWhiteBrightness, currWhiteBrightness), 50);
  }
  delay(50);
}

// ***************************************************************
boolean isNewLightMode() {
  boolean res = false;
  Gesture gesture;                  // Gesture is an enum type from RevEng_PAJ7620.h
  gesture = sensor.readGesture();   // Read back current gesture (if any) of type Gesture

  switch (gesture)
  {
    case GES_LEFT:
      {
        currLightMode--;
        if( currLightMode < 0 ) {
          currLightMode = White;
        }
        res = true;
        break;
      }
    case GES_RIGHT:
      {
        currLightMode ++;
        if( currLightMode > 2 ) {
          currLightMode = Off;
        }
        res = true;
        break;
      }
    case GES_UP:
      {
        currWhiteBrightness += whiteBrightnessIncrements;
        currWhiteBrightness = min(currWhiteBrightness, maxWhiteBrightness);
        lastLightMode = Off;
        res = true;
        break;
      }
    case GES_DOWN:
      {
        currWhiteBrightness += whiteBrightnessIncrements;
        currWhiteBrightness = max(currWhiteBrightness, minWhiteBrightness);
        lastLightMode = Off;
        res = true;
        break;
      }
  };

  return res;
}

// *************************************************************************
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match

    if( isNewLightMode() ) {
      return;
    }

    delay(wait);                           //  Pause for a moment
  }
}


// *************************************************************************
// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    strip.rainbow(firstPixelHue);
    strip.show(); // Update strip with new contents

    if( isNewLightMode() ) {
      return;
    }
    
    delay(wait);  // Pause for a moment
  }
}
