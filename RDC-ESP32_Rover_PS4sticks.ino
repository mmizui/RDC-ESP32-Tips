// include the library code:
#include <PS4Controller.h>
#include <STEMDu.h>
#include <I2CLiquidCrystal.h>
#include <Wire.h>

// initialize the library
// uncomment next line if you are using a LCD from Straberry Linux
I2CLiquidCrystal lcd(40, false); //6V Battery box >> ”false”
                  //  |    +--- set true if the power suply is 5V, false if it is 3.3V
                  //  +-------- contrast (0-63)
// uncomment next line if you are using a LCD from Akizuki denshi
// I2CLiquidCrystal lcd

//PS4 analog sticks strings >> int
int analogLY;
int analogRY;

//DeadBand:Noise reduction when analog stick is neutral position
//No reaction below this value (absolute)
int deadBand = 20;

//Motor driver pin assign for RDC-ESP32 M1/M2
int M1_1 = 2;
int M1_2 = 4;
int M1_PWM = 5;
int M2_1 = 25;
int M2_2 = 26;
int M2_PWM = 27;
int L;

void setup() {
  // Replace the "1a:2b:3c:01:01:01" with the MAC address
  // the controller wants to pair to
  // Note: MAC address must be unicast
  Serial.begin(115200);
  PS4.begin("1a:2b:3c:01:01:01");
  Serial.println("Ready.");

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  
  // Print pin numbers
  lcd.print(" L_stick R_stick");

  //initialize the digital pin as an output
  pinMode(M1_1, OUTPUT);
  pinMode(M1_2, OUTPUT);
  pinMode(M1_PWM, OUTPUT);
  pinMode(M2_1, OUTPUT);
  pinMode(M2_2, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
}

void loop() {
// Bluetooth connect check, true:LED on
  if (PS4.isConnected()) {
    pinMode(19, OUTPUT);
    digitalWrite(19,HIGH);
  }

//TOP speed limt from slider(A7)
//PWM Max val:  255 << slider(A7) >> 0
L = analogRead(A7);
L = map(L, 0, 1023, 0, 100);

//Converting strings to int from PS4 analog sticks
int analogLY = static_cast<int>(PS4.LStickY());
int analogRY = static_cast<int>(PS4.RStickY());

// set the cursor to column 0, line 1
// (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  if (PS4.isConnected()) {
    if (PS4.LStickY()) {
      lcd.print( PS4.LStickY());
      }
  lcd.print(' ');
  lcd.print(analogLY * 2);
  lcd.print(' ');
  if (PS4.RStickY()) {
      lcd.print( PS4.RStickY());
    }
  lcd.print(' ');
  lcd.print(analogRY * 2);
    
// DeadBand:Noise reduction when analog stick is neutral position
if (abs(analogLY) < deadBand) {
      analogLY = 0;
      }

if (abs(analogRY) < deadBand) {
      analogRY = 0;
      }

/*
DC motor control sample for RDC.
6.2018 Atsushi Hasegawa
Connect the motor to the M1/M2 socket.
Motor driver setting H:HIGH L:LOW

IN1 IN2 PWM
H H H/L short brake
L H H CCW(counterclockwise) /PWM = L short brake
H L H CW(clockwise) /PWM = L short brake
L L H stop

PS4 LRstick val : -128 ~ 128
PWM max val : 255
*/
if (analogLY > 0) {
    digitalWrite(M1_1,HIGH);
    digitalWrite(M1_2,LOW);
    analogWrite(M1_PWM, analogLY * L / 50);
    }
    else{
      digitalWrite(M1_1,LOW);
      digitalWrite(M1_2,HIGH);
      analogWrite(M1_PWM, abs(analogLY * L / 50));
    }

  if (analogRY > 0) {
    digitalWrite(M2_1,HIGH);
    digitalWrite(M2_2,LOW);
    analogWrite(M2_PWM, analogRY * L / 50);
    }
    else{
      digitalWrite(M2_1,LOW);
      digitalWrite(M2_2,HIGH);
      analogWrite(M2_PWM, abs(analogRY * L / 50));
    }
  delay(150);
}
