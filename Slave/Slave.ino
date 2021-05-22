#include <SoftwareSerial.h>
#include "SerialTransfer.h"
#include <LiquidCrystal.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
//sound


const int delay_ms = 20; //CHOISIR le temps du delay en ms

const int rs = A2, en = A3, d4 = A4, d5 = A5, d6 = A0, d7 = A1;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//---------------------------------- ruban LED ---------------------------------//

#define LED_PIN 7

// Stip LED : 295
#define LED_COUNT_FRONT 5 // Begin => End LED front
#define LED_COUNT_SIDE 11 // Begin => End LED side
#define LED_COUNT_BACK 16 // Bengin => End LED back
#define LED_COUNT 22 // Begin => End LED other side (end strip)
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
int flag_begin = 0;

// Time gestion
int now_time = 0;
int previous_time = 0;

int iter = 0;

Adafruit_NeoPixel strip (LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

int trigger_Clignotant_gauche;
int trigger_Clignotant_droite;
int trigger_feu_de_detresse;
int trigger_marche_avant;

//-------------------------------- END ruban LED -------------------------------//


//---------------------------------- Bluetooth ----------------------------------//

SoftwareSerial SerialLocal(3, A7); // RX, TX de l'arduino (je recois, j'envoie)
//pin 3 recoit des infos
//pin A7 envoie des infos (elle ne sera pas utilisée) 

SerialTransfer TransferLocal;

struct STRUCT {
  int VRX_Gauche_ServoMoteur1;
  int VRY_Gauche_ServoMoteur2;
  int BP_Gauche_Tirer;
  int VRX_Droite_Moteur1;
  int VRY_Droite_Moteur2;
  int RFID_State;
  int Ultra_Distance;
} data;

//-------------------------------- END Bluetooth --------------------------------//


//----------------------------------- Motors -----------------------------------//

Servo servo;
float tourelle_y = 90;   //hauteur initiale du canon
int yValue; //valeur lue du potentiometre y
const int vitesse_verticale_tourelle = 150; //CHOISIR une vitesse en deg/seconde

int PWM_ENABLE = 6;
int D0_motor_1 = 2;
int D0_motor_2 = 4;

int tourelle_x;                             //variable vitesse de rotation tourelle
int xValue; //valeur lue du potentiometre x
const float vitesse_rotation_tourelle = 0.8;//CHOISIR un taux de vitesse entre 0 et 1

//--------------------------------- END Motors ---------------------------------//

void setup()
{

  servo.attach(5);
  
  lcd.begin(16, 2); 
  
  Serial.begin(38400);
  SerialLocal.begin(38400);
  TransferLocal.begin(SerialLocal);
  
  pinMode(PWM_ENABLE,OUTPUT);
  pinMode(D0_motor_1,OUTPUT);
  pinMode(D0_motor_2,OUTPUT);

  strip.begin();
  strip.show();
  strip.setBrightness(brightness);

  //strip.setBrightness(10);
  //strip.show();
  //delay(1000);
  //strip.setBrightness(150);
  //strip.show();

  /*now_time = millis()/500;
  if (now_time != previous_time) {
    previous_time = now_time;
    iter += 1;
    if (iter <= LED_COUNT) {
      strip.setPixelColor(iter, red, green, blue);
    }
    else {
      flag_begin = 1;
    }
  }*/
}

void loop()
{
  //---------------------------------- ruban LED ---------------------------------//
  //if (flag_begin == 1) {
  if (data.VRY_Droite_Moteur2 < 400){
    trigger_Clignotant_gauche = 1;  
  }
  else {
    trigger_Clignotant_gauche = 0;  
  }
  
  if (data.VRY_Droite_Moteur2 > 600){
    trigger_Clignotant_droite = 1;
  }
  else {
    trigger_Clignotant_droite = 0;
  }

  if (data.VRX_Droite_Moteur1 < 400){
    trigger_marche_avant = 1;
  }
  else {
    trigger_marche_avant = 0;
  }

  if (((data.VRX_Droite_Moteur1 > 400) and (data.VRX_Droite_Moteur1 < 600)) and ((data.VRY_Droite_Moteur2 > 400) and (data.VRY_Droite_Moteur2 < 600))){
    trigger_feu_de_detresse = 1;
  }
  else {
    trigger_feu_de_detresse = 0;
  } 
  //}
  /*else {
    /*for (int i = 0; i <= brightness; i++) {
        strip.setBrightness(i);
        //delay(int((260-brightness)/10));
        delay(10);
    }*/
    /*strip.setBrightness(80);
    strip.show();
    delay(1000);
    strip.setBrightness(150);
    strip.show();*/
    
    /*for (int i = 0; i <= LED_COUNT; i++) {
      strip.setPixelColor(i, red, green, blue);
      delay(100);
    }
    flag_begin = 1;*/
  //}
  now_time = millis()/200;
  if (reset == 0 and (trigger_Clignotant_gauche == 1 or trigger_Clignotant_droite == 1 or trigger_feu_de_detresse == 1)) {
    if (now_time != previous_time) {
      previous_time = now_time;
      iteration += 1;
      if (iteration >= (LED_COUNT_FRONT/2)+1) {
        iteration = -1;
        reset = 1;
      }
      //Serial.println(iteration);
    }
  }

  // Couleur si tank bouge
  else if (trigger_marche_avant == 1) {
    reset = 0;
    for (int i = 0; i <= (LED_COUNT_FRONT-1); i++) {
      strip.setPixelColor(i, 255, 255, 255);
    }
    for (int i = LED_COUNT_SIDE; i <= (LED_COUNT_BACK-1); i++) {
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
    if (trigger_Clignotant_gauche == 1) {
      strip.setPixelColor(((LED_COUNT_FRONT-1)/2)-iteration, orange_red, orange_green, orange_blue);
      if (LED_COUNT_FRONT%2 == 0) {
        strip.setPixelColor((((LED_COUNT_BACK-LED_COUNT_SIDE)-1)/2+1)+LED_COUNT_SIDE+iteration, orange_red, orange_green, orange_blue);
      }
      else {
        strip.setPixelColor((((LED_COUNT_BACK-LED_COUNT_SIDE)-1)/2)+LED_COUNT_SIDE+iteration, orange_red, orange_green, orange_blue);
      }
      strip.show();
    }

  else if (trigger_Clignotant_droite == 1) {
      if (LED_COUNT_FRONT%2 == 0) {
        strip.setPixelColor(((LED_COUNT_FRONT-1)/2+1)+iteration, orange_red, orange_green, orange_blue);
      }
      else {
        strip.setPixelColor(((LED_COUNT_FRONT-1)/2)+iteration, orange_red, orange_green, orange_blue);
      }
      strip.setPixelColor((((LED_COUNT_BACK-LED_COUNT_SIDE)-1)/2)+LED_COUNT_SIDE-iteration, orange_red, orange_green, orange_blue);
      strip.show();
    }
    
  else if (trigger_feu_de_detresse == 1) {
      strip.setPixelColor(((LED_COUNT_FRONT-1)/2)-iteration, orange_red, orange_green, orange_blue);
      strip.setPixelColor((((LED_COUNT_BACK-LED_COUNT_SIDE)-1)/2)+LED_COUNT_SIDE-iteration, orange_red, orange_green, orange_blue);
      if (LED_COUNT_FRONT%2 == 0) {
        strip.setPixelColor(((LED_COUNT_FRONT-1)/2+1)+iteration, orange_red, orange_green, orange_blue);
        strip.setPixelColor((((LED_COUNT_BACK-LED_COUNT_SIDE)-1)/2+1)+LED_COUNT_SIDE+iteration, orange_red, orange_green, orange_blue);
      }
      else {
        strip.setPixelColor(((LED_COUNT_FRONT-1)/2)+iteration, orange_red, orange_green, orange_blue);
        strip.setPixelColor((((LED_COUNT_BACK-LED_COUNT_SIDE)-1)/2)+LED_COUNT_SIDE+iteration, orange_red, orange_green, orange_blue);
      }
      strip.show();
    }
  }
  
  
  //-------------------------------- END ruban LED -------------------------------//
  
  
  //----------------------------------- Motors -----------------------------------//
  
  yValue = map(data.VRX_Gauche_ServoMoteur1, 0, 1023, -100, 100); 
  
  //configuration de la deadband du joystick 2 en Y
  if (yValue > -25 and yValue < 25) {
    yValue = 0;
  }

  //incrémentation tourelle_y
  else if (yValue < -50 and tourelle_y > 67)  { //diminuer pour que le canon aie une meilleur dépression
    tourelle_y -= delay_ms/1000.0 * vitesse_verticale_tourelle;
  }

  else if (50 < yValue and tourelle_y < 135) { //augmenter pour que le canon aie une meilleur élévation
    tourelle_y += delay_ms/1000.0 * vitesse_verticale_tourelle;
  }

  servo.write(tourelle_y);
  
  if(TransferLocal.available())
  {
    uint16_t recSize = 0;
    recSize = TransferLocal.rxObj(data, recSize);
  }


  xValue = data.VRY_Gauche_ServoMoteur2; 


  if (xValue > 400 and xValue < 600) {
    tourelle_x = 0;
  }
  else if (xValue > 800) {
    digitalWrite(D0_motor_2, LOW);
    digitalWrite(D0_motor_1, HIGH);
    
    tourelle_x = map(xValue,0,400,255,0);
  }

  else if (xValue < 400) {
    digitalWrite(D0_motor_1, LOW);
    digitalWrite(D0_motor_2,HIGH);
    
    tourelle_x = map(xValue,600,1023,0,255);
  }
  analogWrite(PWM_ENABLE,tourelle_x * vitesse_rotation_tourelle);

  
  lcd.setCursor(0,0);
  lcd.print(String(data.VRX_Gauche_ServoMoteur1)+"   ");
  lcd.setCursor(0,1);
  lcd.print(String(data.VRY_Gauche_ServoMoteur2)+"   ");
  lcd.setCursor(10,1);
  lcd.print(String(tourelle_y)+"   ");
  lcd.setCursor(10,0);
  lcd.print(String(tourelle_x)+"   ");
  
  //--------------------------------- END Motors ---------------------------------//

  delay(delay_ms);
}
