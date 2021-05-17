#include <Adafruit_NeoPixel.h>

#define LED_PIN 10
#define LED_COUNT 11

Adafruit_NeoPixel strip (LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ400);

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.show();
  strip.setBrightness(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i <= (LED_COUNT-1); i++) {
    strip.setPixelColor(i, 255, 0, 0);
  }
  strip.show();
}
