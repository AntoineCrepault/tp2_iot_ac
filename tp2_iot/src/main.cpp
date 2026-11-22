#include <Arduino.h>
#include <Adafruit_NeoMatrix.h> 
#include <TM1637Display.h>
#include <ESP32Servo.h>
#include "firebaseHandler.h"


//J'aurais bien fait un gros refactoring et une division des fonctions dans des fichiers différents,
//  mais j'ai malheureusement oublié mon fil micro-USB à l'école (comme vous pouvez le voir dans ma démo). 
//  Changer beaucoup de code sans pouvoir le tester serait dangereuxc j'ai donc décidé de laisser tel quel.



const int MATRIS_OUT_PIN = 15; //matris output pin
const int CLK_PIN = 23; // display clock pin
const int DIO_PIN = 22; // display data pin
const int leftButtonPin = 19; // left button pin
const int rightButtonPin = 18; // right button pin
const int SERVO_PIN = 13; // servo pin

#define BRIGHTNESS 20

Adafruit_NeoMatrix matrix(8, 8, MATRIS_OUT_PIN,
NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS +
NEO_MATRIX_PROGRESSIVE, NEO_GRB + NEO_KHZ800);

TM1637Display display(CLK_PIN, DIO_PIN);

Servo servo; 

enum class State {
  START,
  PROG_SAISIE,
  READY_TO_CLOSE,
  CLOSED,
  UNLOCK
};

State currentState = State::START;

int code[4] = {0, 0, 0, 0};
int codeConfirmation[4] = {0, 0, 0, 0};
int codeTry[4] = {0, 0, 0, 0};

bool locked = false;
bool reset = true;


void setup() {
  Wifisetup();
  Serial.begin(115200);
  pinMode(leftButtonPin, INPUT_PULLUP);
  pinMode(rightButtonPin, INPUT_PULLUP);

  display.setBrightness(0x0f);
  display.clear();

  servo.attach(SERVO_PIN);

  matrix.begin();
  matrix.setBrightness(BRIGHTNESS);
}


//utility functions


bool isLeftButtonPressed() {
  if(digitalRead(leftButtonPin) == LOW) 
  {
    delay(50);
    if(digitalRead(leftButtonPin) == LOW)
    {
      return true;
    }
  }
  return false;
}

bool isRightButtonPressed() {
  if(digitalRead(rightButtonPin) == LOW) 
  {
    delay(50);
    if(digitalRead(rightButtonPin) == LOW)
    {
      return true;
    }
  }
  return false;
}

void blink(int r, int g, int b)
{
  static unsigned long timer = millis();
  static bool lightOn = false;

  if (millis() - timer >= 300) 
  {
    timer = millis();
    lightOn = !lightOn;
  }

  if(lightOn) 
  {
    matrix.fillScreen(matrix.Color(r, g, b));
    matrix.show(); 
  } else 
  {
    matrix.fillScreen(matrix.Color(0, 0, 0));
    matrix.show(); 
  }
}


//main functions


void start() {
  matrix.fillScreen(matrix.Color(0, 255, 0));
  matrix.show();  

  static unsigned long pressedAt = 0;
  static bool holding = false;

  if (isRightButtonPressed()) {
    if (!holding) {
      pressedAt = millis();
      holding = true;
    }

    if (millis() - pressedAt >= 2500) { //3000 trop long
      currentState = State::PROG_SAISIE;
      reset = true;
      holding = false;
    }
  } else {
    holding = false;
  }
}

bool codeEntry(int digits[])
{
  static int index = 0;

  //pour ne pas monté plusieur foi de digit par click
  static bool leftWasPressed = false;
  static bool rightWasPressed = true;

  bool leftPressed = isLeftButtonPressed();
  bool rightPressed = isRightButtonPressed();

  if (leftPressed && !leftWasPressed) 
  {
    if (digits[index] + 1 > 9) 
    {
      digits[index] = 0;
    } else {digits[index]++;}
  }

  if (rightPressed && !rightWasPressed) 
  {
    if (index < 3) 
    {
      index++;
    } 
    else
    {
      index = 0;

      leftWasPressed = leftPressed;
      rightWasPressed = rightPressed;

      display.clear();
      return true;
    }
  }

  leftWasPressed = leftPressed;
  rightWasPressed = rightPressed;

  display.showNumberDec(digits[index], false, 1, index);

  return false;
}

void progSaisie()
{
  static bool codeEntered = false;
  static bool delay = false;
  static bool identical = false;
  static unsigned long timer = 0;

  if(reset)
  {
    for (int i = 0; i < 4; i++) 
    {
      code[i] = 0;
      codeConfirmation[i] = 0;
    }
      reset = false;
  }

  if (delay)
  {
    if (identical) 
    {
      blink(0, 255, 0);
    } 
    else 
    {
      blink(255, 0, 0);
    }

    if (millis() - timer >= 3000)
    {
      if (identical) 
      {
        matrix.fillScreen(matrix.Color(0, 255, 0));
        matrix.show();
        postLog("CHANGEMENT_CODE");
        currentState = State::READY_TO_CLOSE;
      }
      
      delay = false;
      codeEntered = false;

      //reset
      if(!identical)
      {
        for (int i = 0; i < 4; i++) 
        {
        code[i] = 0;
        codeConfirmation[i] = 0;
        }
      }
    }

    return;
  }

  blink(255, 150, 0);

  if (!codeEntered)
  {
    codeEntered = codeEntry(code);
  }
  else
  {
    if (codeEntry(codeConfirmation))
    {
      identical = true;

      for (int i = 0; i < 4; i++)
      {
        if (codeConfirmation[i] != code[i]) 
        {
          identical = false;
        }
      }

      timer = millis();
      delay = true;
    }
  }
}

// Utiliser un délai dans l'utilisation du servomoteur, car je ne veux aucune interaction avec le coffre pendant la fermeture ou l'ouverture.
void lock()
{
  servo.write(180);
  delay(1000);
  servo.write(90);

  for (int i = 0; i < 4; i++) {
    codeTry[i] = 0;
  }

  display.clear();
  locked = true;
  postLog("FERMETURE");
  currentState = State::CLOSED;
}

void unlock()
{
  servo.write(0);
  delay(500);
  servo.write(90);

  locked = false;
  postLog("OUVERTURE");
  currentState = State::READY_TO_CLOSE;
}


void readyToClose()
{
  matrix.fillScreen(matrix.Color(0, 255, 0));
  matrix.show();

  if(isLeftButtonPressed() && !locked)
  {
    display.clear();
    locked = true; 
  }

  if(locked)
  {
    lock();
  }
}

void manageClosed()
{
  static bool delay = false;
  static bool identical = false;
  static unsigned long timer = 0;

  if (delay)
  {
    if (identical) {
      blink(0, 255, 0);
    } else {
      blink(255, 0, 0);
    }

    if (millis() - timer >= 3000)
    {
      if (identical) {
        currentState = State::UNLOCK;
      }

      delay = false;
      
      //reset
      for (int i = 0; i < 4; i++) {
        codeTry[i] = 0;
      }
    }

    return;
  }

  matrix.fillScreen(matrix.Color(255, 0, 0));
  matrix.show();

  if (codeEntry(codeTry))
  {
    identical = true;

    for (int i = 0; i < 4; i++)
    {
      Serial.print(codeTry[i]);
      Serial.print(code[i]);
      if (codeTry[i] != code[i]) {
        identical = false;
      }
    }

    timer = millis();
    delay = true;
  }
}



void loop() {

  switch (currentState) 
  {
    case State::START:
      start();
      break;
    case State::PROG_SAISIE:
      progSaisie();
      break;
    case State::READY_TO_CLOSE:
      readyToClose();
      start();
      break;
    case State::CLOSED:
      manageClosed();
      break;
    case State::UNLOCK:
      unlock();
      break;
  }
}


