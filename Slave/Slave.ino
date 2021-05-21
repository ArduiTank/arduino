#include <SoftwareSerial.h>
#include "SerialTransfer.h"
#include <LiquidCrystal.h>
#include <Servo.h>
//sound


const int delay_ms = 20; //CHOISIR le temps du delay en ms

const int rs = A2, en = A3, d4 = A4, d5 = A5, d6 = A0, d7 = A1;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

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


}

void loop()
{
  yValue = map(data.VRY_Gauche_ServoMoteur2, 0, 1023, -100, 100); 

 
  
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


  xValue = data.VRX_Gauche_ServoMoteur1; 


  if (xValue > 400 and xValue < 800) {
    tourelle_x = 0;
  }
  else if (xValue < 400) {
    digitalWrite(D0_motor_1, HIGH);
    digitalWrite(D0_motor_2, LOW);
    tourelle_x = map(xValue,0,400,255,0);
  }

  else if (xValue > 800) {
    digitalWrite(D0_motor_1, LOW);
    digitalWrite(D0_motor_2,HIGH);
    tourelle_x = map(xValue,800,1023,0,255);
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

  delay(delay_ms);
}
