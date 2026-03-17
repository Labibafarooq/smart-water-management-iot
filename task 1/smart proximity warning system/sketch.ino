// Pin Definitions
const int trigPin = 9;
const int echoPin = 10;
const int ledPin = 6;
const int buzzerPin = 7;

// Timing variables
unsigned long previousLEDBlinkMillis = 0;
unsigned long previousBuzzerMillis = 0;
bool ledState = false;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // Trigger the ultrasonic pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo duration and convert to distance
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  // Debug output
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  unsigned long currentMillis = millis();

  if (distance <= 10) {
    // Zone 1: Critical Danger (0–10 cm)
    digitalWrite(ledPin, HIGH);    // LED stays ON
    tone(buzzerPin, 1000);         // Buzzer continuously ON
  } 
  else if (distance > 10 && distance <= 50) {
    // Zone 2: High Alert (11–50 cm)

    // LED: Blink every 200 ms
    if (currentMillis - previousLEDBlinkMillis >= 200) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      previousLEDBlinkMillis = currentMillis;
    }

    // Buzzer: Beep every 500 ms
    if (currentMillis - previousBuzzerMillis >= 500) {
      tone(buzzerPin, 1000);
      delay(100);
      noTone(buzzerPin);
      previousBuzzerMillis = currentMillis;
    }
  } 
  else if (distance > 50 && distance <= 150) {
    // Zone 3: Caution (51–150 cm)

    // LED: Blink every 1000 ms
    if (currentMillis - previousLEDBlinkMillis >= 1000) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      previousLEDBlinkMillis = currentMillis;
    }

    // Buzzer: Beep every 2000 ms
    if (currentMillis - previousBuzzerMillis >= 2000) {
      tone(buzzerPin, 1000);
      delay(100);
      noTone(buzzerPin);
      previousBuzzerMillis = currentMillis;
    }
  } 
  else {
    // Zone 4: Safe Zone (>150 cm)
    digitalWrite(ledPin, LOW);     // LED OFF
    noTone(buzzerPin);             // Buzzer OFF
  }

  delay(50); // Short delay for stability
}
