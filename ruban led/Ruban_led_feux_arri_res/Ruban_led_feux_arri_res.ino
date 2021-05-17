#include <Adafruit_NeoPixel.h>

#define LED_PIN 2
#define LED_COUNT 10

// Default color
int red = 5;
int green = 255;
int blue = 150;

int orange_red = 255;
int orange_green = 50;
int orange_blue = 0;

Adafruit_NeoPixel strip (LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.show();
  strip.setBrightness(255);

  pinMode(3, INPUT); // Clignotant gauche
  pinMode(4, INPUT); // Clignotant droit
  pinMode(5, INPUT); // Feu de détresse
  pinMode(6, INPUT); // Avance
}

void loop() {
  if (digitalRead(6) == 0) {
    for (int i = 0; i <= (LED_COUNT-1); i++) {
      strip.setPixelColor(i, 255, 0, 0);
    }
    strip.show();
  }
  else {
    for (int i = 0; i <= (LED_COUNT-1); i++) {
      strip.setPixelColor(i, red, green, blue);
    }
    strip.show();
  }
  if (digitalRead(3) == 0) {
    for (int i = 0; i <= (LED_COUNT-1)/2; i++) {
      strip.setPixelColor((LED_COUNT-1)/2-i, orange_red, orange_green, orange_blue);
      delay(200);
      strip.show();
    }
    delay(200);
  }
  else if (digitalRead(4) == 0) {
    if (LED_COUNT%2 == 0) {
      for (int i = round((LED_COUNT-1)/2+1); i <= (LED_COUNT-1); i++) {
        strip.setPixelColor(i, orange_red, orange_green, orange_blue);
        delay(200);
        strip.show();
      }
    }
    else {
      for (int i = (LED_COUNT-1)/2; i <= (LED_COUNT-1); i++) {
        strip.setPixelColor(i, orange_red, orange_green, orange_blue);
        delay(200);
        strip.show();
      }
    }
    delay(200);
  }
  else if (digitalRead(5) == 0) { 
    for (int i = 0; i <= (LED_COUNT-1)/2; i++) {
      strip.setPixelColor((LED_COUNT-1)/2-i, orange_red, orange_green, orange_blue);
      if (LED_COUNT%2 == 0) {
        strip.setPixelColor((LED_COUNT-1)/2+i+1, orange_red, orange_green, orange_blue);
      }
      else {
        strip.setPixelColor((LED_COUNT-1)/2+i, orange_red, orange_green, orange_blue);
      }
      delay(200);
      strip.show();
    }
    delay(200);
  }
}
