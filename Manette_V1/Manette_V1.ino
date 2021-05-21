#include <SoftwareSerial.h>
#include "SerialTransfer.h"
#include <LiquidCrystal.h>

SoftwareSerial SerialLocal(A0, A1); // RX, TX de l'arduino (je recois, j'envoie)
SerialTransfer TransferLocal;

//pins
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

struct STRUCT {
  int VRX_Gauche_ServoMoteur1;
  int VRY_Gauche_ServoMoteur2;
  int BP_Gauche_Tirer;
  int VRX_Droite_Moteur1;
  int VRY_Droite_Moteur2;
  int RFID_State;
  int Ultra_Distance;
} data;

const int U_Batteries = A6;
const int LED_Verte = 9;
const int Buzzer = 8;

const int VRX_Droite = A2;
const int VRY_Droite = A3;
const int BP_Droite = 12;

const int VRX_Gauche = A4;
const int VRY_Gauche = A5;
const int BP_Gauche = 11;

//autre
int FlaggSecondes;
float U_Batteries_Valeurs;
int Niveau_de_charge;

int VRX_Droite_Valeur;
int VRY_Droite_Valeur;
int BP_Droite_Valeur;
int MEMO_BP_Droite_Valeur;

int VRX_Gauche_Valeur;
int VRY_Gauche_Valeur;
int BP_Gauche_Valeur;
int MEMO_BP_Gauche_Valeur;

int Front_montant_BP_Droite;
int Front_montant_BP_Gauche;

//mise en page LCD
int MENU = 1;
int AUTO = 0;
int MANU = 0;

int curseur = 1; 
int indice_click_Batterie = 1;

void (* resetFunc) ( void ) = 0;  // déclare la fonction de réinitialisation à l'adresse 0

void setup() {
  Serial.begin(9600);
  SerialLocal.begin(38400);
  TransferLocal.begin(SerialLocal);
  
  lcd.begin(16, 2);
  lcd.print("MAIN MENU"); 
  tone(Buzzer, 0, 100);
  
  
  pinMode(LED_Verte, OUTPUT);
  pinMode(BP_Droite, INPUT);
  pinMode(BP_Gauche, INPUT);

}

void loop() {
  
  FlaggSecondes = (millis() / 500); //toute les 500ms le flag s'incrémente
  U_Batteries_Valeurs = analogRead(U_Batteries)*0.85;
  Niveau_de_charge = map(U_Batteries_Valeurs, 875, 750, 100, 0);
  
  VRX_Droite_Valeur = analogRead(VRX_Droite);
  VRY_Droite_Valeur = analogRead(VRY_Droite);
  BP_Droite_Valeur = digitalRead(BP_Droite);
    
  VRX_Gauche_Valeur = analogRead(VRX_Gauche);
  VRY_Gauche_Valeur = analogRead(VRY_Gauche);
  BP_Gauche_Valeur = digitalRead(BP_Gauche);

  if ((BP_Droite_Valeur == 0) and (BP_Droite_Valeur != MEMO_BP_Droite_Valeur)){ //detection du front montant
      tone(Buzzer, 200, 100); // tone(pin, frequence signal carré(min20,max 20000), durée du signal)
      Front_montant_BP_Droite = 1;  
    }
  else {
      Front_montant_BP_Droite = 0;  
      
  }
    MEMO_BP_Droite_Valeur = BP_Droite_Valeur;
  
    if ((BP_Gauche_Valeur == 0) and (BP_Gauche_Valeur != MEMO_BP_Gauche_Valeur)){ //detection du front montant
      tone(Buzzer, 200, 100); // tone(pin, frequence signal carré(min20,max 20000), durée du signal) 
      Front_montant_BP_Gauche = 1; 
    }
     else {
      Front_montant_BP_Gauche = 0;  
    }
    MEMO_BP_Gauche_Valeur = BP_Gauche_Valeur;

  if (VRX_Droite_Valeur > 800){ //up
    curseur = 0; 
  }
  else if ((VRX_Droite_Valeur < 200) and (curseur == 0)){ //down
      curseur = 2;
  }
  else if ((VRY_Droite_Valeur < 200) and not(curseur == 0) and not(curseur == 2 and MANU or AUTO)){ //left
    curseur = 1;
  }
  else if ((VRY_Droite_Valeur > 800) and not(curseur == 0)){ //right
    curseur = 2;
  }
  
  
  if (FlaggSecondes %2 == 0){
  digitalWrite(LED_Verte, HIGH);
    if (curseur == 0){ //Back
      
      lcd.setCursor(9, 0);
      lcd.print(">");
      if (MENU){          
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.setCursor(9, 1);
        lcd.print(" ");
      }
    }
    
    else if (curseur == 1 and not(MANU or AUTO)){
      
      lcd.setCursor(0, 1);
      lcd.print(">"); 
      lcd.setCursor(9, 0);
      lcd.print(" ");
      lcd.setCursor(9, 1);
      lcd.print(" ");
    }

    else if (curseur == 2){
      
      lcd.setCursor(9, 1);
      lcd.print(">"); 
      lcd.setCursor(9, 0);
      lcd.print(" ");
      if (MENU){
      lcd.setCursor(0, 1);
      lcd.print(" ");
      }
    }
    
  }
  else {
    
    lcd.setCursor(9, 0);
    lcd.print(" ");
    lcd.setCursor(9, 1);
    lcd.print(" ");
    if (MENU){ 
    digitalWrite(LED_Verte, LOW);
    lcd.setCursor(0, 1);
    lcd.print(" ");
    
    }
  }

  
  //Serial.println("BP_Droite : "+ String(BP_Droite_Valeur) + " X : "+ String(VRX_Droite_Valeur) + " Y : "+ String(VRY_Droite_Valeur) + " BP_Gauche : "+ String(BP_Gauche_Valeur) + " X : "+ String(VRX_Gauche_Valeur) + " Y : "+ String(VRY_Gauche_Valeur));
  //Serial.println(String(U_Batteries_Valeurs)+" "+String(Niveau_de_charge));
  //affichages 
  if (MENU) {
    AUTO = 0;
    MANU = 0;
    lcd.setCursor(0, 0);
    lcd.print("MAIN MENU"); 
    lcd.setCursor(1, 1);
    lcd.print(" Manu   ");
    lcd.setCursor(10, 1);
    lcd.print(" Auto ");
    lcd.setCursor(10, 0);
    lcd.print(" Reset ");
    if (Front_montant_BP_Droite and curseur == 0){
      resetFunc ();
    }
    else if (Front_montant_BP_Droite and curseur == 1){
      MANU = 1;
      MENU = 0;
    }
    else if (Front_montant_BP_Droite and curseur == 2){
      AUTO = 1;
      MENU = 0;
    }

   
  }
  
  else if (MANU){
    AUTO = 0;
    digitalWrite(LED_Verte, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("MANU MODE"); 
    lcd.setCursor(10, 0);
    lcd.print(" Back ");
    lcd.setCursor(0, 1);
    lcd.print("Battery :");
    if (Front_montant_BP_Droite and curseur == 0){
      MENU = 1;
      MANU = 0;
    }
    if (Front_montant_BP_Droite and curseur == 2){
      indice_click_Batterie +=1;
    }
    if (indice_click_Batterie %2 == 0){
        lcd.setCursor(10, 1);
        lcd.print(" "+String(U_Batteries_Valeurs/100.00)+"V    " );
    }
    else{
        lcd.setCursor(10, 1);
        lcd.print(" "+String(Niveau_de_charge)+"%    " );
    }
  }

  else if (AUTO){
    MANU = 0;
    digitalWrite(LED_Verte, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("AUTO MODE"); 
    lcd.setCursor(10, 0);
    lcd.print(" Back ");
    lcd.setCursor(0, 1);
    lcd.print("Battery :");
    if (Front_montant_BP_Droite and curseur == 0){
      MENU = 1;
      AUTO = 0;
    }
    if (Front_montant_BP_Droite and curseur == 2){
      indice_click_Batterie +=1;
    }
    if (indice_click_Batterie %2 == 0){
        lcd.setCursor(10, 1);
        lcd.print(" "+String(U_Batteries_Valeurs/100.00)+"V    " );
    }
    else{
        lcd.setCursor(10, 1);
        lcd.print(" "+String(Niveau_de_charge)+"%    " );
    }
  
  }

  //J'envoie
  data.VRX_Gauche_ServoMoteur1 = VRX_Gauche_Valeur;
  data.VRY_Gauche_ServoMoteur2 = VRY_Gauche_Valeur;
  data.BP_Gauche_Tirer = BP_Gauche_Valeur;
  data.VRX_Droite_Moteur1 = VRX_Droite_Valeur;
  data.VRY_Droite_Moteur2 = VRY_Droite_Valeur;
  
  uint16_t sendSize = 0;
  sendSize = TransferLocal.txObj(data, sendSize);
  TransferLocal.sendData(sendSize);

  //Je recois
  if(TransferLocal.available())
  {
    uint16_t recSize = 0;
    recSize = TransferLocal.rxObj(data, recSize); 
  }
  
  Serial.println(data.Ultra_Distance);
  
    delay(50);

}
