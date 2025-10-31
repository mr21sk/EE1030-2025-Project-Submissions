#include "Arduino.h"
#include <LiquidCrystal.h>

int analogPin = A0;
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

const int numSamples = 1000;

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
}

void loop() {
  double sumT = 0.0;

  double a =   1.60009695e+00;
  double b =   3.98087309e-03;
  double c =  -4.05095182e-06;


  for (int i = 0; i < numSamples; i++) {
    double r = analogRead(analogPin);
    double V = (5.0 * r / 1023.0);

    double discriminant = b * b - 4 * c * (a - V);
    double T = 0;

      T = (-b + sqrt(discriminant)) / (2 * c);

    sumT += T;
    delay(1);
  }

  double avgT = sumT / numSamples;

  Serial.println(avgT, 6);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(avgT, 6);

  delay(1000);
  
}
