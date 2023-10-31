// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// buttons setup
const int buttonApin = 7;
const int buttonBpin = 6;
bool buttonA, buttonB, pbuttonA, pbuttonB;

// menu setup
const int Nmenu = 2; // number of menu options
int selectMenu = 0; // current menu item selected on screen
int menu = 0; // current confirmed menu item
String menuItems[Nmenu] = {"Mode", "Timer"};
bool inMenu = true; // true when scrolling menu items, false when in selected menu item

const int Nmodes = 4; // number of modes
int selectMode = 0; // current mode selected on screen
String modes[Nmodes] = {"Bounce", "Mouse", "Manual", "Off"};

int selectTimer = 0; // current timer value selected on screen (in mins)
int timer = 0; // current confirmed timer value
const int timerMax = 10; // max selectable timer value
unsigned long timerTime = 0; // time that timer started in millis
int minsElapsed = 0;
int pminsElapsed = 0;
byte timerAnim[] = {byte('-'), byte(7), byte('|'), byte('/')};

// custom LCD characters for animations
byte backslash[8] = {
  0b00000,
  0b10000,
  0b01000,
  0b00100,
  0b00010,
  0b00001,
  0b00000,
  0b00000
};

byte mouth[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b00100,
  0b10101,
  0b01010
};

byte headL[8] = {
  0b00010,
  0b00101,
  0b00100,
  0b01000,
  0b01000,
  0b01000,
  0b00100,
  0b00011
};

byte headR[8] = {
  0b01000,
  0b10100,
  0b00100,
  0b00010,
  0b00010,
  0b00010,
  0b00100,
  0b11000
};

byte eyeL[8] = {
  0b01110,
  0b11101,
  0b11101,
  0b10001,
  0b10001,
  0b10001,
  0b01110,
  0b00000
};

byte eyeR[8] = {
  0b01110,
  0b10001,
  0b10001,
  0b10001,
  0b10111,
  0b10111,
  0b01110,
  0b00000
};

byte tailU[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b10001,
  0b00000,
  0b00000,
  0b00000
};

byte tailD[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b10001,
  0b01110,
  0b00000,
  0b00000
};

#include "wolterMover.h"

// Laser
const int laserPin = 8;

wolterMover mover;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A4));

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // custom characters for animation
  lcd.createChar(0, mouth);
  lcd.createChar(1, headR);
  lcd.createChar(2, headL);
  lcd.createChar(3, eyeR);
  lcd.createChar(4, eyeL);
  lcd.createChar(5, tailU);
  lcd.createChar(6, tailD);
  lcd.createChar(7, backslash);
  
  pinMode(buttonA, INPUT);
  pinMode(buttonB, INPUT);

  mover.init();

  // Laser
  pinMode(laserPin, OUTPUT);

  Serial.println("Start");
}

void loop() {
  // read previous and current button states
  pbuttonA = buttonA;
  pbuttonB = buttonB;
  buttonA = digitalRead(buttonApin);
  buttonB = digitalRead(buttonBpin);

  // quick and dirty button debounce
  // button A scrolls the current selected menu/mode/timer value
  if(!buttonA && pbuttonA) {
    if(inMenu) {
      selectMenu = (selectMenu+1)%Nmenu;
    } else if(menu==0) {
      selectMode = (selectMode+1)%Nmodes;
    } else if(menu==1) {
      selectTimer = (selectTimer+1)%(timerMax+1);
    }
    lcd.clear();
  }
  // button B confirms the current selected menu/mode/timer value
  if(!buttonB && pbuttonB) {
    if(inMenu) {
      menu = selectMenu;
      inMenu = false;
    } else if(menu==0) {
      mover.setMode(selectMode);
      // turn on laser, except in off mode
      digitalWrite(laserPin, HIGH);
      if(selectMode==3) digitalWrite(laserPin, LOW);
      inMenu = true;
    } else if(menu==1) {
      timer = selectTimer;
      inMenu = true;
      timerTime = millis();
    }
    lcd.clear();
  }

  // display selection arrow
  lcd.setCursor(0,0);
  if(!inMenu) lcd.setCursor(8,0);
  lcd.print('>');

  // display menu items
  lcd.setCursor(1,0);
  if(inMenu) {
    lcd.print(menuItems[selectMenu]);
    lcd.setCursor(1,1);
    lcd.print(menuItems[(selectMenu+1)%Nmenu]);
  } else {
    lcd.print(menuItems[menu]);
  }
  
  // display timer/mode items
  lcd.setCursor(9,0);
  if(!inMenu && menu==0) {
    lcd.print(modes[selectMode]);
    lcd.setCursor(9,1);
    lcd.print(modes[(selectMode+1)%Nmodes]);
  } else if(!inMenu && menu==1) {
    lcd.print(String(selectTimer) + " min");
  } else {
    lcd.print(modes[mover.getMode()]);
  }

  // timer 
  if(timer>0) {
    pminsElapsed = minsElapsed;
    minsElapsed = floor((millis()-timerTime)/60000.0);

    // display timer bar and animation
    if(pminsElapsed != minsElapsed) lcd.clear(); // redraw timer bar when minute elapses
    for(int i=17-timer+minsElapsed; i!=16; ++i) {
      lcd.setCursor(i,1);
      lcd.write(0b11111111);
    }
    lcd.setCursor(16-timer+minsElapsed,1);
    int anim = (millis()/250)%4;
    lcd.write(timerAnim[anim]);

    // turn robot off when timer runs out
    if(minsElapsed >= timer) {
      digitalWrite(laserPin, LOW);
      mover.setMode(3);
      timer = 0;
      lcd.clear();
    }
  } else if(inMenu) {
    // if no timer is running and the menu is selected, there is space on screen for an animation!
    if(mover.getMode()==0) {
      byte anim = byte(3);
      if(millis()%1000>500) anim = byte(4);
      lcd.setCursor(10,1);
      lcd.write(byte(2));
      lcd.setCursor(11,1);
      lcd.write(anim);
      lcd.setCursor(12,1);
      lcd.write(byte(0));
      lcd.setCursor(13,1);
      lcd.write(anim);
      lcd.setCursor(14,1);
      lcd.write(byte(1));
    } else if(mover.getMode()==1) {
      byte anim[] = {byte(5),byte(6)};
      if(millis()%1000>500) {anim[0] = byte(6); anim[1] = byte(5);}
      lcd.setCursor(10,1);
      lcd.write('<');
      lcd.setCursor(11,1);
      lcd.write('\"');
      lcd.setCursor(12,1);
      lcd.write('_');
      lcd.setCursor(13,1);
      lcd.write(')');
      lcd.setCursor(14,1);
      lcd.write(anim[0]);
      lcd.setCursor(15,1);
      lcd.write(anim[1]);
    } else if(mover.getMode()==2) {
      byte anim[] = {byte(3),byte(4)};
      if(millis()%1000>500) {anim[0] = byte(4); anim[1] = byte(3);}
      lcd.setCursor(10,1);
      lcd.write(byte(2));
      lcd.setCursor(11,1);
      lcd.write(anim[0]);
      lcd.setCursor(12,1);
      lcd.write(byte(0));
      lcd.setCursor(13,1);
      lcd.write(anim[1]);
      lcd.setCursor(14,1);
      lcd.write(byte(1));
    } else if(mover.getMode()==3) {
      lcd.setCursor(10,1);
      lcd.write(byte(2));
      lcd.setCursor(11,1);
      lcd.write(byte(6));
      lcd.setCursor(12,1);
      lcd.write(byte(0));
      lcd.setCursor(13,1);
      lcd.write(byte(6));
      lcd.setCursor(14,1);
      lcd.write(byte(1));
    }
    
  }
  
  mover.debug();
  mover.move();
  mover.setServos();
}
