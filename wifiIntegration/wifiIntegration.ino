/*
 * Den Desk Tech Up Project
 * 
 * Aaron S. Crandall <crandall@gonzaga.edu>
 * Copyright 2023
 */

#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>

#define MQTT_MAX_PACKET_SIZE 3100  // The LCD is 64x48 and I'm being wasteful
#include <PubSubClient.h>


#include "Adafruit_SSD1306_wemos.h"



// OLED
#if (SSD1306_LCDHEIGHT != 48)
  #error("Height incorrect, please fix Adafruit_SSD1306.h! - see WEMOS libraries");
#endif
#define OLED_RESET 0
Adafruit_SSD1306 display(OLED_RESET);

// Green status LED - used for networking status
#define LED_STATUS_PIN D7

// RGB LED strip (NeoPixels)
#define RGB_LED_STRIP_PIN D8
#define RGB_LED_STRIP_SIZE 14
#define RGB_LED_STRIP_BRIGHTNESS 50 // Set BRIGHTNESS to about 1/5 (max = 255)
Adafruit_NeoPixel rgb_strip(RGB_LED_STRIP_SIZE, RGB_LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

// Networking and MQTT Client
WiFiClient espWiFiClient;
PubSubClient mqtt_client(espWiFiClient);
const char* mqtt_server_hostname = "broker.hivemq.com";
const int mqtt_server_port = 1883;
String mqtt_client_ID = "ASC-IoT-" + WiFi.macAddress();
const char* mqtt_subscribe_topic = "asc/testingOut123";


// ***************************************************************************** //
void setupWiFi() {
  WiFi.begin();
  Serial.print("Connecting to ");
  Serial.print(WiFi.SSID()); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());  
}

// ***************************************************************************** //
void setupSerial() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\nSerial initialized");
}

// ***************************************************************************** //
void setupOLED() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
}

// ***************************************************************************** //
void setupStatusLEDs() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_STATUS_PIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);
  digitalWrite(LED_BUILTIN, HIGH);
}

// ***************************************************************************** //
void setupRGBLEDStrip() {
  rgb_strip.begin();
  rgb_strip.show();
  rgb_strip.setBrightness(RGB_LED_STRIP_BRIGHTNESS); 
}

void blueStatusLED_ON() {
  digitalWrite(LED_BUILTIN, LOW);
}
void blueStatusLED_OFF() {
  digitalWrite(LED_BUILTIN, HIGH);
}

void greenStatusLED_ON() {
  digitalWrite(LED_STATUS_PIN, HIGH);
}
void greenStatusLED_OFF() {
  digitalWrite(LED_STATUS_PIN, LOW);
}

// ***************************************************************************** //
void handle_lcd_message(byte* payload, unsigned int length) {
  Serial.println("Handling LCD update message");
  byte* data_payload = payload + 4;
  unsigned int data_length = length - 4;

  for( int index = 0; index < data_length; index++ ) {
    int y = index / 64;
    int x = index % 64;
    if( data_payload[index] == '1' ) {
      display.drawPixel(x, y, 1);
    } else {
      display.drawPixel(x, y, 0);
    }
  }
  display.display();
}

// ***************************************************************************** //
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  if( !memcmp(payload, "lcd:", 4) ) {
    handle_lcd_message(payload, length);
  }
  Serial.println();
}

// ***************************************************************************** //
void mqtt_reconnect() {
  while (!mqtt_client.connected()) {  // Loop until we're reconnected - note: blocking
    greenStatusLED_OFF();
    Serial.print("Attempting MQTT connection as ");
    Serial.print(mqtt_client_ID);
    if (mqtt_client.connect(mqtt_client_ID.c_str())) {
      Serial.println(" - connected");
      // mqtt_client.publish("asc/testingOut123", "hello world");
      mqtt_client.subscribe(mqtt_subscribe_topic);
      greenStatusLED_ON();
    } else {
      greenStatusLED_ON();
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      delay(250);
      greenStatusLED_OFF();
      delay(5000);
    }
  }
}

// ***************************************************************************** //
void mqtt_service_loop() {
  if (!mqtt_client.connected()) {
    mqtt_reconnect();
  }
  mqtt_client.loop();
}

// ***************************************************************************** //
void setupMQTTClient() {
  mqtt_client.setServer(mqtt_server_hostname, mqtt_server_port);
  mqtt_client.setCallback(callback);
  mqtt_client.setBufferSize(MQTT_MAX_PACKET_SIZE);
  Serial.println(mqtt_client.getBufferSize());
}

// ***************************************************************************** //
void setup() {
  setupSerial();
  setupWiFi();
  setupOLED();
  setupStatusLEDs();
  setupRGBLEDStrip();

  randomSeed(micros());
  setupMQTTClient();



  //testdrawcircle();

}



// ***************************************************************************** //
void loop() {
  mqtt_service_loop();

  
}



void testdrawcircle(void) {
  for (int16_t i=0; i<display.height(); i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, WHITE);
    display.display();
    delay(1);
  }
}
