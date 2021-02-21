#include <SPI.h>
#include <SdFat.h>
#include <UTFT.h>
#include <URTouch.h>
#include <UTFT_SdRaw.h>

extern uint8_t SmallFont[];
extern uint8_t BigFont[];

#define SD_CHIP_SELECT  53  // SD chip select pin
// file system object
SdFat sd;

UTFT myGLCD(ILI9341_16, 38, 39, 40, 41);
URTouch  myTouch( 6, 5, 4, 3, 2);
UTFT_SdRaw myFiles(&myGLCD);

int x, y;
char currentPage, selectedUnit;
////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println(F("Initialising SD card..."));
  bool mysd = 0;
  // see if the card is present and can be initialized:
  while (!mysd)
  {
    if (!sd.begin(SD_CHIP_SELECT, SPI_FULL_SPEED)) {
      Serial.println(F("Card failed, or not present"));
      Serial.println(F("Retrying...."));
    }
    else
    {
      mysd = 1;
      Serial.println(F("Card initialised."));
    }
}
 Serial.println(F("Initialising LCD."));
  myGLCD.InitLCD();
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
  Serial.println(F("LCD initialised."));
  Serial.println(F("Initialising GPS."));
  Aceuille();
  currentPage = '0'; //indique que nous somme dans l'acceuille.
}
////////////////////////////////////////////////////////////////////////////////////////////////
// ===== initiateGame - Custom Function
void Aceuille() {
  // background
  myGLCD.fillScr(6, 43, 115);
  // texte info
  myGLCD.setColor(0, 0,0);
  myGLCD.setBackColor(6, 43, 115);
  myFiles.load(0,0,320,192,"FOND_ECRAN2.RAW", 1);
   myFiles.load(130,80,50,61,"Smill.RAW", 1);
  myGLCD.setFont(BigFont);
  myGLCD.print("Bienvenue",CENTER,25);
  myGLCD.setFont(SmallFont);
  myGLCD.print("sur l'ecran d'acceuil",CENTER,50);
  

  // Button1
  myGLCD.setColor(32,56,100); // Sets green color
  myGLCD.fillRoundRect (35, 170, 285, 210); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.drawRoundRect (35, 170, 285, 210); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont); // Sets the font to big
  myGLCD.setBackColor(32,56,100); // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print("Demarrer", CENTER, 182); // Prints the string
}

// Met en surbrillance le bouton lorsqu'il est enfoncé
void drawFrame ( int x1, int y1, int x2, int y2 ) {  
  myGLCD. setColor ( 255, 0, 0 ) ;
  myGLCD. drawRoundRect ( x1, y1, x2, y2 ) ; 
  while ( myTouch. dataAvailable ()) 
    myTouch.read () ;
    myGLCD. setColor ( 255, 255, 255 ) ;
    myGLCD. drawRoundRect ( x1, y1, x2, y2 ) ;
}

void Button (int x1, int y1, int x2, int y2, int z1,String s1){
   myGLCD.setColor(32,56,100); // Sets green color
  myGLCD.fillRoundRect (x1, y1, x2, y2); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.drawRoundRect (x1, y1, x2, y2); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(SmallFont); // Sets the font to big
  myGLCD.setBackColor(32,56,100); // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print(s1, CENTER, z1); // Prints the string 
}
///////////////////////////////////////////////////
void  MenuPrincipale(){
   // background
  myGLCD.fillScr(6, 43, 115);
  // texte info
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(6, 43, 115);
  myGLCD.setFont(BigFont);
  myFiles.load(0,0,320,192,"FOND_ECRAN2.RAW", 1);
  myFiles.load(0,0,50,50,"reglage.RAW", 1);

  // Button1.1 
  Button (35,50,285,90,62,"Telecommande");
 // Button1.2
  Button (35, 110, 285, 150,122,"Autonome (IA)");
  // Button1.3
   Button (35, 170, 285, 210,182,"Actions predefinies");
 
  myFiles.load(240,56,30,30,"LOGO2.RAW", 1);
  myFiles.load(50,118,30,30,"LOGO3.RAW", 1);
  myFiles.load(240,172,40,39,"LOGO4.RAW", 1);
  
}
void loop() {

 // Aceuill toutch
  if (currentPage == '0') {
    if (myTouch.dataAvailable()) {
      myTouch.read();
      x=myTouch.getX(); // X coordinate where the screen has been pressed
      y=myTouch.getY(); // Y coordinates where the screen has been pressed
      // If we press the Distance Sensor Button 
      if ((x>=35) && (x<=285) && (y>=30) && (y<=70)) {
        drawFrame(35, 170, 285, 210); // Custom Function -Highlighs the buttons when it's pressed
        currentPage = '1'; // Indicates that we are the first example
        MenuPrincipale(); // It is called only once, because in the next iteration of the loop, this above if statement will be false so this funtion won't be called. This function will draw the graphics of the first example.
}
  }
    }
}
