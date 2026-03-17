#include <LiquidCrystal_I2C.h>

#define TRIG_PIN 9
#define ECHO_PIN 10

#define RED_LED 2
#define YELLOW_LED 3
#define GREEN_LED 4
#define BUZZER 5
#define RELAY_PIN 8 // Pump control

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int tankHeight = 400; // tank height in cm

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH); // Start with pump OFF (relay inactive)

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Water Level:");
}

void loop() {
  long duration;
  float distance, levelPercent;

  // Trigger ultrasonic
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  // Calculate water level
  levelPercent = ((tankHeight - distance) / tankHeight) * 100;
  levelPercent = constrain(levelPercent, 0, 100); // limit to 0-100%

  // Display on LCD
  lcd.setCursor(0, 1);
  lcd.print("Level: ");
  lcd.print((int)levelPercent);
  lcd.print("%   "); // pad to clear old digits

  // LED and buzzer indication
  if (levelPercent < 30) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BUZZER, HIGH);
  } else if (levelPercent < 60) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BUZZER, LOW);
  } else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(BUZZER, LOW);
  }

  // Relay-controlled pump logic
  if (levelPercent < 30) {
  digitalWrite(RELAY_PIN, LOW); // Turn pump ON
} else {
  digitalWrite(RELAY_PIN, HIGH); // Turn pump OFF
}

  delay(1000);
}
