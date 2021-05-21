#include <Adafruit_NeoPixel.h>

#define LED_PIN 2
#define LED_COUNT_FRONT 5 // Stip LED : 295
#define LED_COUNT 15 // Stip LED : 295
int brightness = 100;

// Default color
int red = 5;
int green = 255;
int blue = 150;

// Warning color
int orange_red = 255;
int orange_green = 50;
int orange_blue = 0;

// Selection light & Reset color
int iteration = -1;
int reset = 1;

int old_status = 1;

// Time gestion
int now_time = 0;
int previous_time = 0;

Adafruit_NeoPixel strip (LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.show();
  strip.setBrightness(brightness);

  pinMode(3, INPUT); // Clignotant gauche
  pinMode(4, INPUT); // Clignotant droit
  pinMode(5, INPUT); // Feu de détresse
  pinMode(6, INPUT); // Avance

  Serial.begin(9600);
}

void loop() {
  now_time = millis()/200;
  if (reset == 0 and (digitalRead(3) == 0 or digitalRead(4) == 0 or digitalRead(5) == 0)) {
    if (now_time != previous_time) {
      previous_time = now_time;
      iteration += 1;
      if (iteration >= (LED_COUNT_FRONT/2)+1) {
        iteration = -1;
        reset = 1;
      }
      Serial.println(iteration);
    }
  }
  // Couleur si tank bouge
  else if (digitalRead(6) == 0) {
    reset = 0;
    for (int i = 0; i <= (LED_COUNT_FRONT-1); i++) {
      strip.setPixelColor(i, 255, 0, 0);
    }
    strip.show();
  }
  // Couleur par défaut
  else {
    reset = 0;
    for (int i = 0; i <= (LED_COUNT-1); i++) {
      strip.setPixelColor(i, red, green, blue);
    }
    strip.show();
  }


  // Chenillard lampes arrière
  if (iteration != -1) {
    if (digitalRead(3) == 0) {
      strip.setPixelColor(((LED_COUNT_FRONT-1)/2)-iteration, orange_red, orange_green, orange_blue);
      strip.show();
    }

    else if (digitalRead(4) == 0) {
      if (LED_COUNT_FRONT%2 == 0) {
        strip.setPixelColor(((LED_COUNT_FRONT-1)/2+1)+iteration, orange_red, orange_green, orange_blue);
        strip.show();
      }
      else {
        //strip.setPixelColor(((LED_COUNT_FRONT-1)/2)+iteration, orange_red, orange_green, orange_blue);
        strip.setPixelColor(((LED_COUNT_FRONT-1)/2)+iteration, orange_red, orange_green, orange_blue);
        strip.show();
      }
    }

    else if (digitalRead(5) == 0) {
      strip.setPixelColor(((LED_COUNT_FRONT-1)/2)-iteration, orange_red, orange_green, orange_blue);
      if (LED_COUNT_FRONT%2 == 0) {
        strip.setPixelColor(((LED_COUNT_FRONT-1)/2+1)+iteration, orange_red, orange_green, orange_blue);
      }
      else {
        //strip.setPixelColor(((LED_COUNT_FRONT-1)/2)+iteration, orange_red, orange_green, orange_blue);
        strip.setPixelColor(((LED_COUNT_FRONT-1)/2)+iteration, orange_red, orange_green, orange_blue);
      }
      strip.show();
    }
  }
}
