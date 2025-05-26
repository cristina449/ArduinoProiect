#define LED_PIN 2
#define LM35_PIN 34
#define WATER_SENSOR_PIN 35

#include <EEPROM.h>

const int EEPROM_SIZE = 512;
int messageIndex = 0;
int floodIndex = 100;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(WATER_SENSOR_PIN, INPUT);
  EEPROM.begin(EEPROM_SIZE);
}

void loop() {
  int analogValue = analogRead(LM35_PIN);
  float voltage = analogValue * (3.3 / 4095.0);
  float temperatureC = voltage * 100.0;
  Serial.print("Temperature: ");
  Serial.println(temperatureC);

  int waterState = digitalRead(WATER_SENSOR_PIN);
  if (waterState == HIGH) {
    Serial.println("Flood");
    storeFloodEvent();
    delay(500);
  }

  if (Serial.available()) {
    char command = Serial.read();
    if (command == 'A') {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ON");
    }
    if (command == 'S') {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED OFF");
    }
    if (command == 'M') {
      String message = Serial.readStringUntil('\n');
      storeMessage(message);
    }
  }
  delay(1000);
}

void storeMessage(String msg) {
  int addr = messageIndex;
  for (int i = 0; i < msg.length() && (addr + i) < 100; i++) {
    EEPROM.write(addr + i, msg[i]);
  }
  messageIndex = (messageIndex + 50) % 100;
  EEPROM.commit();
}

void storeFloodEvent() {
  String event = "Flood event\n";
  int addr = floodIndex;
  for (int i = 0; i < event.length() && (addr + i) < 400; i++) {
    EEPROM.write(addr + i, event[i]);
  }
  floodIndex = 100 + (floodIndex + 50) % 400;
  EEPROM.commit();
}
