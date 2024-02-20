#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <NewPing.h>

#define SENSOR  27
#define TRIGGER_PIN  12
#define ECHO_PIN     11
#define MAX_DISTANCE 200

#define PIN_RED    13
#define PIN_YELLOW 14  
#define PIN_GREEN  12

#define RED_TIME     500 // RED time in millisecond
#define YELLOW_TIME  500 // YELLOW time in millisecond
#define GREEN_TIME   500 // GREEN time in millisecond

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD setup
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

float offsetX = 0.0;
float offsetY = 0.0;
float offsetZ = 0.0;

volatile byte pulseCount;
byte pulse1Sec = 0;
float calibrationFactor = 4.5;
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);
  setupAccelerometer();
  setupFlowSensor();
  setupLight();
  lcd.begin();  
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("TMAB");
  lcd.setCursor(1, 5);
  lcd.print("Is Working!!");
  digitalWrite(PIN_RED, HIGH);   // turn on
  digitalWrite(PIN_YELLOW, HIGH); // turn off
  digitalWrite(PIN_GREEN, HIGH);  // turn off
  delay(2000);
  lcd.clear();
  
}

void loop() {
  readAccelerometer();
  readFlowSensor();
  float distance = readUltrasonicSensor();
  
  if (flowRate > 60 && distance < 20 && offsetX < 10 && offsetY < 10 && offsetZ < 10) { // Check if flow rate > 60, distance < 20, and XYZ < 10
    lcd.clear();  
    lcd.setCursor(0, 0);  
    lcd.print("X:");  
    lcd.print(offsetX);  
    lcd.print(" Y:");  
    lcd.print(offsetY);  
    lcd.print(" Z:");  
    lcd.println(offsetZ);  
    lcd.setCursor(1, 1);  
    lcd.print("Danger");
    digitalWrite(PIN_RED, HIGH);   // turn on
    digitalWrite(PIN_YELLOW, LOW); // turn off
    digitalWrite(PIN_GREEN, LOW);  // turn off
    delay(RED_TIME); // keep red light on during a period of time  
  }else if (flowRate > 40 && distance < 10 && offsetX < 5 && offsetY < 5 && offsetZ < 5) {
    lcd.clear();  
    lcd.setCursor(0, 0);  
    lcd.print("X:");  
    lcd.print(offsetX);  
    lcd.print(" Y:");  
    lcd.print(offsetY);  
    lcd.print(" Z:");  
    lcd.println(offsetZ);  
    lcd.setCursor(1, 1);  
    lcd.print("Warning");
    digitalWrite(PIN_RED, LOW);    // turn off
    digitalWrite(PIN_YELLOW, HIGH); // turn on
    digitalWrite(PIN_GREEN, LOW); // turn off
    delay(YELLOW_TIME); // keep yellow light on during a period of time
  }else{
    lcd.clear();  
    lcd.setCursor(0, 0);  
    lcd.print("X:");  
    lcd.print(offsetX);  
    lcd.print(" Y:");  
    lcd.print(offsetY);  
    lcd.print(" Z:");  
    lcd.println(offsetZ);  
    lcd.setCursor(1, 1);  
    lcd.print("Normal");
    digitalWrite(PIN_RED, LOW);    // turn off
    digitalWrite(PIN_YELLOW, LOW); // turn off
    digitalWrite(PIN_GREEN, HIGH); // turn on
    delay(GREEN_TIME); // keep green light on during a period of time
  }
  
  // Construct the message
  String message = "X:" + String(offsetX) + " Y:" + String(offsetY) + " Z:" + String(offsetZ) +
                   " Flow Rate:" + String(int(flowRate)) + "L/min" +
                   " Liquid Quantity:" + String(totalMilliLitres) + "mL / " + String(totalMilliLitres / 1000) + "L" +
                   " Distance:" + String(distance) + "cm";
  
  // Send the message via serial
  Serial.println(message);

  delay(1000);
}

void setupAccelerometer() {
  if (!accel.begin()) {
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while (1);
  }
  calibrate();
}

void calibrate() {
  Serial.println("Calibrating...");
  const int numSamples = 1000;
  float sumX = 0.0;
  float sumY = 0.0;
  float sumZ = 0.0;

  for (int i = 0; i < numSamples; i++) {
    sensors_event_t event;
    accel.getEvent(&event);
    sumX += event.acceleration.x;
    sumY += event.acceleration.y;
    sumZ += event.acceleration.z;
    delay(10);
  }

  offsetX = sumX / numSamples;
  offsetY = sumY / numSamples;
  offsetZ = sumZ / numSamples;

  Serial.println("Calibration done.");
}

void readAccelerometer() {
  sensors_event_t event;
  accel.getEvent(&event);

  float calibratedX = event.acceleration.x - offsetX;
  float calibratedY = event.acceleration.y - offsetY;
  float calibratedZ = event.acceleration.z - offsetZ;

  // Print the values to serial monitor
  Serial.print("X: "); Serial.print(calibratedX); Serial.print("  ");
  Serial.print("Y: "); Serial.print(calibratedY); Serial.print("  ");
  Serial.print("Z: "); Serial.print(calibratedZ); Serial.print("  ");
  Serial.println("m/s^2 ");
  delay(500);
}

void setupFlowSensor() {
  pinMode(SENSOR, INPUT_PULLUP);
  pulseCount = 0;
  totalMilliLitres = 0;
  previousMillis = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
}

void readFlowSensor() {
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    pulse1Sec = pulseCount;
    pulseCount = 0;
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;
  }
}

void setupLight(){
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
}

float readUltrasonicSensor() {
  unsigned int distance = sonar.ping_cm();
  return distance;
}
