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



void loop() {
}


