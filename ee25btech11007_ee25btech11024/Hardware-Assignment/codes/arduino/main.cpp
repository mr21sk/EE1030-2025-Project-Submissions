#include <LiquidCrystal.h>

const int rs = 7;
const int en = 8;
const int d4 = 9;
const int d5 = 10;
const int d6 = 11;
const int d7 = 12;


LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int sensorPin = A0;


const float VREF = 5.0; 
const int ADC_MAX = 1023;

void setup() {
  Serial.begin(9600);  
  lcd.begin(16, 2);    
  lcd.clear();
  lcd.print("PT100 Reader");
  delay(1500);         
  lcd.clear();
}

void loop() {
  int sensorValue = analogRead(sensorPin);
  float voltage = sensorValue * (VREF / ADC_MAX);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ADC:");
  lcd.print(sensorValue);

  lcd.setCursor(0, 1);
  lcd.print("V=");
  lcd.print(voltage, 3);  
  lcd.print(" V");

  Serial.print("ADC = ");
  Serial.print(sensorValue);
  Serial.print(", Voltage = ");
  Serial.print(voltage, 3);
  Serial.println(" V");

  delay(1000);  
}

