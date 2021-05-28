#include <SoftwareSerial.h> //bibliotheque Bluetooth
#include "SerialTransfer.h" //bibliotheque Bluetooth
#include <LiquidCrystal.h> //bibliotheque LCD
#include <Servo.h> //bibliotheque ruban LED 
#include <Adafruit_NeoPixel.h> //bibliotheque ruban LED 

const int delay_ms = 5; //CHOISIR le temps du delay en ms

const int rs = A2, en = A3, d4 = A4, d5 = A5, d6 = A0, d7 = A1; //Configuration des pins de l'LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //Configuration des pins de l'LCD

//---------------------------------- ruban LED ---------------------------------//

#define LED_PIN 7 //Pin digitale du ruban LED

#define LED_COUNT_FRONT 5 // Begin => End LED front
#define LED_COUNT_SIDE 11 // Begin => End LED side
#define LED_COUNT_BACK 16 // Bengin => End LED back
#define LED_COUNT 22 // Begin => End LED other side (end strip)

int brightness = 100; //intensité lumineuse du ruban

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

//flags du programme
int old_status = 1; 
int flag_begin = 1;

// Time gestion
int now_time = 0;
int previous_time = 0;

int iter = 0;

Adafruit_NeoPixel strip (LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); //Creation de notre ruban LED

int trigger_Clignotant_gauche; //Variable active lorsque le joystick se penche vers la droite
int trigger_Clignotant_droite; //Variable active lorsque le joystick se penche vers la gauche
int trigger_feu_de_detresse; //Variable active lorsque le joystick ne se penche pas, il est au centre
int trigger_marche_avant; //Variable active lorsque le joystick se penche vers l'avant

//-------------------------------- END ruban LED -------------------------------//

float U_Batteries_Valeurs;        //Valeur ADC lue en volt
int Niveau_de_charge;             //Valeur ADC lue en pourcentage

//---------------------------------- Bluetooth ----------------------------------//

SoftwareSerial SerialLocal(3, A7); // RX, TX de l'arduino (je recois, j'envoie)
SerialTransfer TransferLocal; 
//pin 3 recoit des infos
//pin A7 envoie des infos (elle ne sera pas utilisée) 

struct STRUCT { //Liste de données bluetooth
  int VRX_Gauche_ServoMoteur1 = 510; //Variable du potentiomettre de l'axe X du joystick Gauche de la manette
  int VRY_Gauche_ServoMoteur2 = 520; //Variable du potentiomettre de l'axe Y du joystick Gauche de la manette
  int BP_Gauche_Tirer; //Variable de l'état du bouton du joystock Gauche nous permettant de tirer
  int VRX_Droite_Moteur1; //Variable du potentiomettre de l'axe X du joystick Droite de la manette
  int VRY_Droite_Moteur2; //Variable du potentiomettre de l'axe Y du joystick Droite de la manette
  int RFID_State; //Variable de l'état validé ou pas de la carte RFID
  int Ultra_Distance; //Variable en cm de la distance détectée
} data;

//-------------------------------- END Bluetooth --------------------------------//


//----------------------------------- Motors -----------------------------------//

Servo servo; //Création du servomoteur du canon
float tourelle_y = 90;   //hauteur initiale du canon
int yValue; //valeur lue du potentiometre y
const int vitesse_verticale_tourelle = 150; //CHOISIR une vitesse en deg/seconde

int PWM_ENABLE = 6; //Pin PWM du motoreducteur de la rotation de la tourelle
int D0_motor_1 = 2; //Pin digital rotation Gauche du motoreducteur de la rotation de la tourelle
int D0_motor_2 = 4; //Pin digital rotation Droite du motoreducteur de la rotation de la tourelle

int tourelle_x; //variable vitesse de rotation tourelle
int xValue; //valeur lue du potentiometre x
const float vitesse_rotation_tourelle = 0.8;//CHOISIR un taux de vitesse entre 0 et 1

//--------------------------------- END Motors ---------------------------------//

void setup()
{

  servo.attach(5);                      //Liaison du servomoteur à la pin 5
  
  lcd.begin(16, 2);                     //Création du LCD
  
  Serial.begin(38400);                  //vitesse de communication fixée
  SerialLocal.begin(38400);             //vitesse de communication fixée 
  TransferLocal.begin(SerialLocal);     //Liaison des deux bibliothèque pour la communication Bluetooth
  
  pinMode(PWM_ENABLE,OUTPUT);           //Pin PWM du motoreducteur de la rotation de la tourelle mis en tant que sortie
  pinMode(D0_motor_1,OUTPUT);           //Pin digital rotation Gauche du motoreducteur de la rotation de la tourelle mis en tant que sortie
  pinMode(D0_motor_2,OUTPUT);           //Pin digital rotation Droite du motoreducteur de la rotation de la tourelle mis en tant que sortie

  strip.begin();                        //Initialisation du ruban LED
  strip.show();                         //Rafraichissage du ruban LED
  strip.setBrightness(brightness);      //Rafraichissage de l'intensité du ruban LED
}

void loop()
{
  //---------------------------------- ruban LED ---------------------------------//
  if (data.VRY_Droite_Moteur2 > 600){   //Si la Variable du potentiomettre de l'axe Y du joystick Droite de la manette est Supérieur à 600
    trigger_Clignotant_gauche = 1;      //trigger à 1  
  }
  else {
    trigger_Clignotant_gauche = 0;      //trigger à 0   
  }
  
  if (data.VRY_Droite_Moteur2 < 400){   //Si la Variable du potentiomettre de l'axe Y du joystick Droite de la manette est Inférieur à 400
    trigger_Clignotant_droite = 1;      //trigger à 1  
  }
  else {
    trigger_Clignotant_droite = 0;      //trigger à 0  
  }

  if (data.VRX_Droite_Moteur1 < 400){   //Si la Variable du potentiomettre de l'axe X du joystick Droite de la manette est Inférieur à 400
    trigger_marche_avant = 1;           //trigger à 1  
  }
  else {
    trigger_marche_avant = 0;           //trigger à 0  
  }
                                        //Si les Variable du potentiomettre de l'axe X & Y du joystick Droite de la manette sont au centre
  if (((data.VRX_Droite_Moteur1 > 400) and (data.VRX_Droite_Moteur1 < 600)) and ((data.VRY_Droite_Moteur2 > 400) and (data.VRY_Droite_Moteur2 < 600))){
    trigger_feu_de_detresse = 1;        //trigger à 1  
  }
  else {
    trigger_feu_de_detresse = 0;        //trigger à 0  
  } 
  
  if (flag_begin == 1) {                      //Si nous sommes au tout début du lancement du programme alors on évolue fluidement au taux d'intensité de luminausité du ruban
    now_time = millis() / 5;                  //Création d'un délais de toute les 5 ms par interruption
    if (now_time != previous_time) {          //Si les 5ms sont écoulées
      previous_time = now_time;               //Mise en commun des vaiable temporelle pour mieux s'y reperer au niveau des délais
      iteration += 1;                         //Incrémentation d'une variable évolutive proportionnelle au taux d'intensité de luminausité
      for (int j = 0; j <= LED_COUNT; j++) {  //Sur toute les LED, Une par Une, on met à jour le niveau de luminausité par couleur
        strip.setPixelColor(j, ((float(red) / float(255)) * iteration), ((float(green) / float(255)) * iteration), ((float(blue) / float(255)) * iteration));
        strip.show();                         //Rafraichissement du ruban sur toutes les couleurs
      }
    }
    if (iteration == 255) { //Une fois atteint la valeur maximale des LED
      iteration = -1;       //On remet l'itération à la variable initiale
      flag_begin = 0;       //On ne rerentre plus jamais dans cette grande condition
    }
  }
  else {                    //Sinon, si nous ne sommes pas au début du lancement du programme on commence les différents "thèmes" du ruban
  now_time = millis()/200;  //meme principe qu'avant, création d'un délais via les interruptions
  if (reset == 0 and (trigger_Clignotant_gauche == 1 or trigger_Clignotant_droite == 1 or trigger_feu_de_detresse == 1)) {
    if (now_time != previous_time) {
      previous_time = now_time;
      iteration += 1;
      if (iteration >= (LED_COUNT_FRONT/2)+1) {
        iteration = -1;
        reset = 1;
      }
    }
  }

  // Couleur si tank bouge
  else if (trigger_marche_avant == 1) { //Si nous sommes en marche avant alors on active le thème correspondant
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
    if (trigger_Clignotant_droite == 1) { //Si nous tournons à gauche alors on active le thème correspondant
      strip.setPixelColor(((LED_COUNT_FRONT-1)/2)-iteration, orange_red, orange_green, orange_blue);
      if (LED_COUNT_FRONT%2 == 0) {
        strip.setPixelColor((((LED_COUNT_BACK-LED_COUNT_SIDE)-1)/2+1)+LED_COUNT_SIDE+iteration, orange_red, orange_green, orange_blue);
      }
      else {
        strip.setPixelColor((((LED_COUNT_BACK-LED_COUNT_SIDE)-1)/2)+LED_COUNT_SIDE+iteration, orange_red, orange_green, orange_blue);
      }
      strip.show();
    }

  else if (trigger_Clignotant_gauche== 1) { //Si nous tournons à droite alors on active le thème correspondant
      if (LED_COUNT_FRONT%2 == 0) {
        strip.setPixelColor(((LED_COUNT_FRONT-1)/2+1)+iteration, orange_red, orange_green, orange_blue);
      }
      else {
        strip.setPixelColor(((LED_COUNT_FRONT-1)/2)+iteration, orange_red, orange_green, orange_blue);
      }
      strip.setPixelColor((((LED_COUNT_BACK-LED_COUNT_SIDE)-1)/2)+LED_COUNT_SIDE-iteration, orange_red, orange_green, orange_blue);
      strip.show();
    }
    
  else if (trigger_feu_de_detresse == 1) { //Si nous sommes à l'arret alors on active le thème correspondant
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
}
  
  //-------------------------------- END ruban LED -------------------------------//
  
  U_Batteries_Valeurs = analogRead(A6)*0.85;   //Lecture ADC de la tension de la batterie par l'intermédiaire d'un pont diviseur de tension
  Niveau_de_charge = map(U_Batteries_Valeurs, 875, 750, 100, 0); //Mise à l'échelle de la tension
  
  //----------------------------------- Motors -----------------------------------//
  
  yValue = map(data.VRX_Gauche_ServoMoteur1, 0, 1023, -100, 100); //Lecture de la donnée bluethoot VRX_Gauche_ServoMoteur1 et mise à l'échelle de sa valeur sur la variable yValue
  
  //configuration de la deadband du joystick 2 en Y
  if (yValue > -25 and yValue < 25) {                             //point neutre
    yValue = 0;                                                   //yValue forcé à 0
  }

  //incrémentation tourelle_y
  else if (yValue < -75 and tourelle_y > 67)  {                   //diminuer le chiffre de droite pour que le canon aie une meilleur dépression
    tourelle_y -= delay_ms/1000.0 * vitesse_verticale_tourelle;   //Calcul de la position du servomteur
  }

  else if (75 < yValue and tourelle_y < 135) {                    //augmenter le chiffre de droite pour que le canon aie une meilleur élévation
    tourelle_y += delay_ms/1000.0 * vitesse_verticale_tourelle;   //Calcul de la position du servomteur
  }

  servo.write(tourelle_y);                                        //Lancement du servomoteur à la position
  
  if(TransferLocal.available())                                   //Si nous avons recu des données via Bluetooth
  {
    uint16_t recSize = 0; 
    recSize = TransferLocal.rxObj(data, recSize);                 //Reception des données
  }


  xValue = data.VRY_Gauche_ServoMoteur2;  //Mise en commun de la variable xValue par celle correspondante lue dans la trame


  if (xValue > 400 and xValue < 600) {    //Si nous sommes au point neutre du joystick
    tourelle_x = 0;                       //tourelle_x forcé à 0
  }
  else if (xValue > 600) {                //Sinon si nous sommes supérieur à 600
    digitalWrite(D0_motor_2, LOW);        //Marche arrière du moteur en activant et
    digitalWrite(D0_motor_1, HIGH);       //désactivation des transistant du pont en H
    
    tourelle_x = map(xValue,0,400,255,0); //Mise à l'échelle de la valeur xValue à celle tourelle_x
  } 

  else if (xValue < 400) {                //Sinon si nous sommes inférieur à 400
    digitalWrite(D0_motor_1, LOW);        //Marche avant du moteur en activant et
    digitalWrite(D0_motor_2,HIGH);        //désactivation des transistant du pont en H
    
    tourelle_x = map(xValue,600,1023,0,255); //Mise à l'échelle de la valeur xValue à celle tourelle_x
  }
  analogWrite(PWM_ENABLE,tourelle_x * vitesse_rotation_tourelle); //écriture sur la pin PWM de driver la valeur mise à l'échelle
  //Affichage des valeurs de débugs sur le LCD
if ((data.VRX_Gauche_ServoMoteur1 == 510) and (data.VRY_Gauche_ServoMoteur2 == 520)){
  lcd.setCursor(0,0);                                     
  lcd.print("Waiting for a          ");
  lcd.setCursor(0,1);
  lcd.print("connexion...           ");
}
else {
  lcd.setCursor(0,0);                                     
  lcd.print("Let's go !        ");
  lcd.setCursor(0,1);
  lcd.print("Battery :  ");
  lcd.setCursor(11,1);
  lcd.print(String(Niveau_de_charge)+"%    " ); 
}  
  //--------------------------------- END Motors ---------------------------------//

  delay(delay_ms); //Délais pouvant etre choisis tout au début du programme, tout en haut
}
