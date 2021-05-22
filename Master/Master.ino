#include <SoftwareSerial.h>
#include "SerialTransfer.h"
#include <MFRC522.h> // for the RFID
#include <SPI.h> // for the RFID and SD card module

SoftwareSerial SerialLocal(A0, 3); // RX, TX de l'arduino (je recois, j'envoie)
SerialTransfer TransferLocal;
//pin A0 recoit des infos (elle ne sera pas utilisée)
//pin 3 envoie des infos

#define CS_RFID 10
#define RST_RFID 7
MFRC522 rfid(CS_RFID, RST_RFID); 
String uidString;

float vitesse_rotation = 1;     //définit la vitesse de rotation du tank (MAX 1)
float vitesse = 1;              //définit la vitesse maximale du tank (MAX 1)

int pin_S1 = A1; //Pin 1 du moteur 1 ; IN1
int pin_S2 = A2; //Pin 2 du moteur 1 ; IN2

int pin_S3 = A3; //Pin 1 du moteur 2 ; IN3
int pin_S4 = A4; //Pin 2 du moteur 2 ; IN4

int pin_ENA = 5; 
int pin_ENB = 6; 

int vitesse_chenille1;
int vitesse_chenille2;
int VRx1_val;
int VRy1_val;

int trig = 2;
int echo = 4;
long lecture_echo;
long distance;


struct STRUCT {
  int VRX_Gauche_ServoMoteur1;
  int VRY_Gauche_ServoMoteur2;
  int BP_Gauche_Tirer;
  int VRX_Droite_Moteur1 = 510;
  int VRY_Droite_Moteur2 = 520;
  int RFID_State;
  int Ultra_Distance;
} data;

void readRFID() {
  rfid.PICC_ReadCardSerial();
  Serial.print("Tag UID: ");
  uidString = String(rfid.uid.uidByte[0]) + " " + String(rfid.uid.uidByte[1]) + " " + 
  String(rfid.uid.uidByte[2]) + " " + String(rfid.uid.uidByte[3]);
  Serial.println(uidString);

}

void setup()
{
  Serial.begin(9600);
 
  
  SerialLocal.begin(38400);
  TransferLocal.begin(SerialLocal);

  pinMode(pin_S1,OUTPUT);
  pinMode(pin_S2,OUTPUT);
  pinMode(pin_S3,OUTPUT);
  pinMode(pin_S4,OUTPUT);
  pinMode(pin_ENA,OUTPUT);
  pinMode(pin_ENB,OUTPUT);

  pinMode(trig, OUTPUT);
  digitalWrite(trig, LOW);
  pinMode(echo, INPUT);

  SPI.begin(); 
  rfid.PCD_Init();
  Serial.println("initialization done.");

}


void loop()
{

  VRx1_val = map(data.VRX_Droite_Moteur1, 0, 1023, -255, 255);
  VRy1_val = map(data.VRY_Droite_Moteur2, 0, 1023, -255, 255);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  lecture_echo = pulseIn(echo, HIGH);
  distance = lecture_echo /58;

  if(rfid.PICC_IsNewCardPresent()) {
    readRFID();
  }
  
  //calcul vitesse chenille 1 et 2 en marche avant
  if (VRy1_val > 0) {
    vitesse_chenille1 = (VRy1_val * vitesse + VRx1_val * vitesse_rotation);
    vitesse_chenille2 = (VRy1_val * vitesse - VRx1_val * vitesse_rotation);
    if (vitesse_chenille1 > 255){
      vitesse_chenille1 = 255;
    }
    if (vitesse_chenille2 > 255){
      vitesse_chenille2 = 255;
    }
  }
  //calcul vitesse chenille 1 et 2 en marche arrière  
  else if (VRy1_val < 0) {
    vitesse_chenille1 = (VRy1_val * vitesse - VRx1_val * vitesse_rotation);
    vitesse_chenille2 = (VRy1_val * vitesse + VRx1_val * vitesse_rotation);
    if (vitesse_chenille1 < -255){
      vitesse_chenille1 = -255;
    }
    if (vitesse_chenille2 < -255){
      vitesse_chenille2 = -255;
    }
  }
  
  //envoi des données de sens de rotation et vitesse aux moteurs (avec une deadband autour de la position neutre)
  if (vitesse_chenille1 > 50) {
    digitalWrite(pin_S2, LOW);
    digitalWrite(pin_S1, HIGH);
    analogWrite(pin_ENA, abs(vitesse_chenille1));
    
  }
  
  else if (vitesse_chenille1 < -50) { 
    digitalWrite(pin_S1, LOW);
    digitalWrite(pin_S2, HIGH);
    analogWrite(pin_ENA, abs(vitesse_chenille1));
    
  }
  
  else {
    digitalWrite(pin_S1, LOW);
    digitalWrite(pin_S2, LOW);
    digitalWrite(pin_ENA, LOW);
  }
  
  if (vitesse_chenille2 > 50) {
    digitalWrite(pin_S4, LOW);
    digitalWrite(pin_S3, HIGH);
    analogWrite(pin_ENB, abs(vitesse_chenille2));
  }
  
  else if (vitesse_chenille2 < -50) {
    digitalWrite(pin_S3, LOW);
    digitalWrite(pin_S4, HIGH);
    analogWrite(pin_ENB, abs(vitesse_chenille2));
    
  }
  
  else {
    digitalWrite(pin_S3, LOW);
    digitalWrite(pin_S4, LOW);
    digitalWrite(pin_ENB, LOW);
  }
 
  //Serial.println("Chenille 1 : "+String(vitesse_chenille1) + " Chenille 2 : "+String(vitesse_chenille2));
  //J'envoie
  data.Ultra_Distance = distance;
  
  uint16_t sendSize = 0;
  sendSize = TransferLocal.txObj(data, sendSize);
  TransferLocal.sendData(sendSize);
    
  //Je recois
  if(TransferLocal.available())
  {
    uint16_t recSize = 0;
    recSize = TransferLocal.rxObj(data, recSize);
  }

//  Serial.print(" BP_1 : ");
//  Serial.print(data.BP_Gauche_Tirer);
//  Serial.print(" VRY_2 : ");
//  Serial.print(data.VRX_Droite_Moteur1);
//  Serial.print(" VRY_2 : ");
//  Serial.println(data.VRY_Droite_Moteur2);

  delay(20);
  
}
