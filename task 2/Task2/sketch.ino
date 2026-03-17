#include <Wire.h>
#include <Servo.h>

// MPU6050 Registers
const int MPU = 0x68;
int16_t AcX, AcY, AcZ;
float angleX, angleY;

// Servo setup
Servo servoX;
Servo servoY;

// OLED SSD1306 basic I2C
#define OLED_ADDR 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

void oledCommand(byte c) {
  Wire.beginTransmission(OLED_ADDR);
  Wire.write(0x00); // command
  Wire.write(c);
  Wire.endTransmission();
}

void oledData(byte d) {
  Wire.beginTransmission(OLED_ADDR);
  Wire.write(0x40); // data
  Wire.write(d);
  Wire.endTransmission();
}

void oledInit() {
  oledCommand(0xAE); // Display off
  oledCommand(0xA8); oledCommand(0x3F); // Multiplex
  oledCommand(0xD3); oledCommand(0x00); // Display offset
  oledCommand(0x40); // Display start line
  oledCommand(0xA1); // Segment remap
  oledCommand(0xC8); // COM Output Scan Direction
  oledCommand(0xDA); oledCommand(0x12);
  oledCommand(0x81); oledCommand(0x7F); // Contrast
  oledCommand(0xA4); // Display RAM
  oledCommand(0xA6); // Normal display
  oledCommand(0xD5); oledCommand(0x80);
  oledCommand(0x8D); oledCommand(0x14);
  oledCommand(0xAF); // Display on
  delay(100);
  oledClear();
}

void oledClear() {
  for (int i = 0; i < 8; i++) {
    oledCommand(0xB0 + i);
    oledCommand(0x00);
    oledCommand(0x10);
    for (int j = 0; j < 128; j++) {
      oledData(0x00);
    }
  }
}

// Just shows horizontal lines for visual feedback
void oledBar(int x, int y, int length) {
  oledCommand(0xB0 + y); // page
  oledCommand(0x00 + (x & 0x0F));
  oledCommand(0x10 + ((x >> 4) & 0x0F));
  for (int i = 0; i < length; i++) {
    oledData(0xFF);
  }
}

void setup() {
  Wire.begin();
  Serial.begin(9600);

  // MPU6050 init
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // power management
  Wire.write(0);
  Wire.endTransmission(true);

  // OLED init
  oledInit();

  // Servo init
  servoX.attach(9);
  servoY.attach(10);
}

void loop() {
  // Read MPU6050 data
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // read 6 bytes

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  // Calculate angles (in degrees)
  angleX = atan2(AcY, AcZ) * 180 / PI;
  angleY = atan2(-AcX, sqrt(AcY * AcY + AcZ * AcZ)) * 180 / PI;

  // Map angles to servo range
  int servoXVal = map((int)angleX, -45, 45, 0, 180);
  int servoYVal = map((int)angleY, -45, 45, 0, 180);
  servoXVal = constrain(servoXVal, 0, 180);
  servoYVal = constrain(servoYVal, 0, 180);

  // Move servos
  servoX.write(servoXVal);
  servoY.write(servoYVal);

  // OLED visual feedback (simple bars)
  oledClear();
  oledBar(10, 1, map(servoXVal, 0, 180, 0, 100)); // X axis bar
  oledBar(10, 3, map(servoYVal, 0, 180, 0, 100)); // Y axis bar

  Serial.print("Roll(X): ");
  Serial.print(angleX);
  Serial.print("  Pitch(Y): ");
  Serial.println(angleY);

  delay(100);
}
