// réglages des vitesses
int vitesse_tourelle = 60;    //définit la vitesse de rotation de la tourelle en deg/seconde
int vitesse_rotation = 1;     //définit la vitesse de rotation du tank (MAX 1)
int vitesse = 1;              //définit la vitesse maximale du tank (MAX 1)

// include the EduIntro library
#include <EduIntro.h>
#include <Stepper.h>
//joystick gauche => déplacement
#define VRx_PIN1         A0 //Pin du potentiomÃ¨tre du joystick axe X
#define VRy_PIN1         A1 //Pin du potentiomÃ¨tre du joystick axe Y
#define SW_PIN1          99  //Bouton du joystick
//joystick droit => tourelle
#define VRx_PIN2         A2 //Pin du potentiomÃ¨tre du joystick axe X
#define VRy_PIN2         A3 //Pin du potentiomÃ¨tre du joystick axe Y
#define SW_PIN2          88  //Bouton du joystick

ServoMotor servo_x(D10);		// creating the object 'servo' on pin D10
ServoMotor servo_y(D11);		// creating the object 'servo' on pin D10

float tourelle_x = 90;
float tourelle_y = 90;

int vitesse_chenille1 = 0;
int vitesse_chenille2 = 0;

const int stepsPerRevolution = 64;  // change this to fit the number of steps per revolution
// for your motor
// initialize the steppers:
Stepper myStepper1(stepsPerRevolution, 2, 3, 4, 5);
Stepper myStepper2(stepsPerRevolution, 6, 7, 8, 9);

void setup() {
  Serial.begin(9600);
}


void loop()
{  
  int VRx1_val = map(analogRead(VRx_PIN1), 0, 1023, -100, 100);
  int VRy1_val = map(analogRead(VRy_PIN1), 0, 1023, -100, 100);
  
  int VRx2_val = map(analogRead(VRx_PIN2), 0, 1023, -100, 100);
  int VRy2_val = map(analogRead(VRy_PIN2), 0, 1023, -100, 100);
  
  //configuration de la deadband du joystick 1
  if (VRx1_val > -25 and VRx1_val < 25) {
    VRx1_val = 0;
  }
  if (VRy1_val > -25 and VRy1_val < 25) {
    VRx1_val = 0;
  }
  
  //calcul vitesse chenille 1 et 2
  vitesse_chenille1 = (VRy1_val * vitesse + VRx1_val * vitesse_rotation)/2;
  vitesse_chenille2 = (VRy1_val * vitesse - VRx1_val * vitesse_rotation)/2;
  Serial.println(vitesse_chenille1);
  //Serial.print(VRx2_val);
  
  if (vitesse_chenille1 > 0) {
    myStepper1.setSpeed(vitesse_chenille1);
    // step 1/100 of a revolution:
    myStepper1.step(stepsPerRevolution / 100);
  }
  if (vitesse_chenille2 > 0) {
    myStepper2.setSpeed(vitesse_chenille2);
    // step 1/100 of a revolution:
    myStepper2.step(stepsPerRevolution / 100);
  }  
  //                                                  a voir: comment donner une vitesse négative
  
  //incrémentation tourelle_x
  if (VRx2_val < -50 and tourelle_x > 0)  {
    tourelle_x -= 0.025*vitesse_tourelle;
  }

  else if (50 < VRx2_val and tourelle_x < 180) {
    tourelle_x += 0.025*vitesse_tourelle;
  }
  
  //incrémentation tourelle_y 
  if (VRy2_val < -50 and tourelle_y > 0)  {
    tourelle_y -= 0.025*vitesse_tourelle;
  }

  else if (50 < VRy2_val and tourelle_y < 180) {
    tourelle_y += 0.025*vitesse_tourelle;
  }

  servo_x.write(tourelle_x);
  servo_y.write(tourelle_y);
  //Serial.println(tourelle_y);
  delay(25);
}
