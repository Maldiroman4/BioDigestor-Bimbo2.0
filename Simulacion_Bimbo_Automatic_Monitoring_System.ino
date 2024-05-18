int pinBuzzer = 3;
int pinLED = 13;
int pinServo = 6;
int pinMotor = 5; // Pin para el motor adicional
int pinServo360 = 9; // Pin para el servo 360
int pinGasAlarmLED = 2; // Pin para el LED de alarma del sensor de gas

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
Servo myServo;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  BIMBO AUTOMATIC  ");
  lcd.setCursor(0, 1);
  lcd.print(" MONITORING SYSTEM ");
  delay(5000);
  lcd.clear();

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinLED, OUTPUT);
  pinMode(pinMotor, OUTPUT);
  pinMode(pinGasAlarmLED, OUTPUT);

  myServo.attach(pinServo);
  myServo.write(90);
}

void loop() {
  int potValue1 = analogRead(A0);
  int potValue2 = analogRead(A1);
  int potValue3 = analogRead(A2);
  int potValue4 = analogRead(A3);

  float pHValue = mapToPH(potValue1);
  float temperatureValue = mapToTemperature(potValue2);
  float pressureValue = mapToPressure(potValue3);
  float gasValue = mapToGas(potValue4);

  Serial.print("pH: ");
  Serial.println(pHValue, 1);

  Serial.print("Temperatura: ");
  Serial.println(temperatureValue);

  Serial.print("Presion: ");
  Serial.println(pressureValue);

  Serial.print("Gas: ");
  Serial.println(gasValue);

  lcd.setCursor(0, 0);
  lcd.print("pH: ");
  lcd.print(pHValue, 1);

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temperatureValue, 1);

  lcd.setCursor(0, 2);
  lcd.print("Pres: ");
  lcd.print(pressureValue, 2);

  lcd.setCursor(0, 3);
  lcd.print("Gas: ");
  lcd.print(gasValue);

  if (pHValue < 6.8 || pHValue > 7.2) {
    showAlert(1, pHValue);
    digitalWrite(pinMotor, HIGH);
  } else {
    digitalWrite(pinMotor, LOW);
  }

  if (temperatureValue < 32 ) {
    showAlert(2, temperatureValue);
    myServo.attach(pinServo);
    myServo.write(0);
    delay(1000);
    myServo.write(180);
    delay(1000);
    myServo.write(90);
    myServo.detach();
  }

  if (pressureValue > 8.0) {
    showAlert(3, pressureValue);
    Servo servo360;
    servo360.attach(pinServo360);
    servo360.write(0);
    delay(1000);
    servo360.write(180);
    delay(1000);
    servo360.write(90);
    servo360.detach();
  }

  if (gasValue > 230000) {
    digitalWrite(pinGasAlarmLED, HIGH);
  } else {
    digitalWrite(pinGasAlarmLED, LOW);
  }

  delay(250);
}

float mapToPH(int sensorValue) {
  float pH = map(sensorValue, 0, 1023, 10, 140);
  return pH / 10.0;
}

float mapToTemperature(int sensorValue) {
  float temperature = map(sensorValue, 0, 1023, 0, 800);
  return temperature / 10.0;
}

float mapToPressure(int sensorValue) {
  return map(sensorValue, 0, 1023, 0, 100) / 10.0;
}

float mapToGas(int sensorValue) {
  return map(sensorValue, 0, 1023, 0, 250000);
}

void showAlert(int sensorNumber, float sensorValue) {
  tone(pinBuzzer, 1000);
  digitalWrite(pinLED, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ALERTA! Sensor P");
  lcd.print(sensorNumber);
  lcd.setCursor(0, 1);
  lcd.print("Excedio el rango");
  lcd.print(": ");
  lcd.setCursor(0, 2);
  lcd.print(sensorValue);
  delay(250);
  noTone(pinBuzzer);
  digitalWrite(pinLED, LOW);
  delay(250);
  lcd.clear();
}

