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
  NON_PROGRAMME ,
  PROG_SAISIE,
  PROG_CONFIRMATION,
  OUVERT,
  FERME
};

State currentState = State::NON_PROGRAMME;


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
matrix.fillScreen(matrix.Color(0, 255, 0));
matrix.show();
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



void loop() {

  if (isLeftButtonPressed()) {
    Serial.println("Left button pressed");
    matrix.fillScreen(matrix.Color(200, 0, 200));
    matrix.show();
  }
  if (isRightButtonPressed()) {
    Serial.println("Right button pressed");
    matrix.fillScreen(matrix.Color(200, 200, 0));
    matrix.show();
  }

  


  switch (currentState) {
    case State::NON_PROGRAMME:

      break;
    case State::PROG_SAISIE:

      break;
    case State::PROG_CONFIRMATION:

      break;
    case State::OUVERT:

      break;
    case State::FERME:

      break;
  }
}


