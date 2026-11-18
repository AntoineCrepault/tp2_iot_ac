#include <Arduino.h>
#include <Adafruit_NeoMatrix.h> 
#include <TM1637Display.h>
#include <ESP32Servo.h>

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
  PROG_CONFIRMATION,
  OUVERT,
  FERME
};

State currentState = State::START;

int code;


void setup() {
  Serial.begin(115200);
  pinMode(leftButtonPin, INPUT_PULLUP);
  pinMode(rightButtonPin, INPUT_PULLUP);

  display.setBrightness(0x0f);
  display.clear();

  servo.attach(SERVO_PIN);
  servo.write(90);

  matrix.begin();
  matrix.setBrightness(BRIGHTNESS);
}

bool isLeftButtonPressed() {
  if(digitalRead(leftButtonPin) == LOW) {
    delay(50);
    if(digitalRead(leftButtonPin) == LOW)
    {
      return true;
    }
  }
  return false;
}

bool isRightButtonPressed() {
  if(digitalRead(rightButtonPin) == LOW) {
    delay(50);
    if(digitalRead(rightButtonPin) == LOW)
    {
      return true;
    }
  }
  return false;
}

void Start() {
  servo.write(0);
  matrix.fillScreen(matrix.Color(0, 255, 0));
  matrix.show();  

  static unsigned long pressedAt = 0;
  static bool holding = false;

  if (isRightButtonPressed()) {
    if (!holding) {
      pressedAt = millis();
      holding = true;
      Serial.println("pressedAt: " + String(pressedAt));
    }

    if (millis() - pressedAt >= 3000) {
      currentState = State::PROG_SAISIE;
      holding = false;
    }
  } else {
    holding = false;
  }
}

void progSaisie() {
  static unsigned long timer = millis();
  static bool lightOn = false;

  if (millis() - timer >= 500) {
    timer = millis();
    lightOn = !lightOn;
  }

  if(lightOn) {
    matrix.fillScreen(matrix.Color(255, 150, 0));
    matrix.show(); 
  } else {
    matrix.fillScreen(matrix.Color(0, 0, 0));
    matrix.show(); 
  }

  static int digits[] = {0, 0, 0, 0};
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
  } else {index = 0;}
}

leftWasPressed = leftPressed;
rightWasPressed = rightPressed;

  display.showNumberDec(digits[index], false, 1, index);
}




void loop() {

  switch (currentState) {
    case State::START:
      Start();
      break;
    case State::PROG_SAISIE:
      progSaisie();
      break;
    case State::PROG_CONFIRMATION:

      break;
    case State::OUVERT:

      break;
    case State::FERME:

      break;
  }
}


