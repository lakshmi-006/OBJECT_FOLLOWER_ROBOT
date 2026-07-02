#include <Wire.h>

#define TF_LUNA_ADDR 0x10
#define SDA_PIN 5
#define SCL_PIN 18

// Motor Pins
#define IN1 23
#define IN2 21
#define IN3 19
#define IN4 22

#define ENA 4
#define ENB 2

#define MIN_DIST 40
#define MAX_DIST 80

// -------- LiDAR Read --------
bool readBytes(uint8_t reg, uint8_t *buf, uint8_t len) {
  Wire.beginTransmission(TF_LUNA_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;

  uint8_t count = Wire.requestFrom(TF_LUNA_ADDR, len);
  if (count != len) return false;

  for (uint8_t i = 0; i < len; i++) {
    buf[i] = Wire.read();
  }
  return true;
}

// -------- Motor Control --------
void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopBot() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// -------- Setup --------
void setup() {
  Serial.begin(115200);
  delay(1500);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  analogWrite(ENA, 110);   // slightly reduced speed (smooth)
  analogWrite(ENB, 110);

  Serial.println("🔥 FINAL FOLLOW BOT STARTED 🔥");
}

// -------- Loop --------
void loop() {
  uint8_t data[6];

  if (readBytes(0x00, data, 6)) {

    uint16_t distance = data[0] | (data[1] << 8);

    // -------- Serial Output --------
    Serial.print("Distance: ");
    Serial.println(distance);

    if (distance >= MAX_DIST) {
      forward();
    }
    else if(distance <= MIN_DIST){
      backward();
      stopBot();
    }
    else if (distance >= MIN_DIST && distance <= MAX_DIST) {
      stopBot();  
    }

    else {
      stopBot();          // >80 cm → STOP
    }

  } else {
    Serial.println("❌ Read failed");
    stopBot();
  }

  delay(100);
}
