int incomingByte;
int buzzer;

void setup() {
  Serial.begin(9600);
  buzzer = 'L';
}

void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    if (incomingByte == 'P') {
      buzzer = Serial.read();
    } else if (incomingByte == 'C') {
      Serial.print('S');
      Serial.print(buzzer, BYTE);
    } else {
      Serial.print(incomingByte, BYTE);
    }
  }
  //delay(30);
}

