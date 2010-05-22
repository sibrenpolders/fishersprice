// digital pins
int PIN_SWITCH = 7;
int PIN_PUSHBUTTON = 9;
int PIN_ENCODER_A = 5;
int PIN_ENCODER_B = 6;
int PIN_BUZZER = 8;

// analog pins
int PIN_POTENTIO = 3;
int PIN_ACCEL_X = 0;
int PIN_ACCEL_Y = 4;
int PIN_ACCEL_Z = 5;

// values
int encoderPinALast = LOW;
int encoderRead = LOW;
int loopNr = 0;
 
int switch_on;
int push_on;
int buzz_triggered;
int encoder_position = 0;
int potentio;
int accelX;
int accelY;
int accelZ;

int incomingByte;

void setup() { 
   pinMode (PIN_ACCEL_X,INPUT);
   pinMode (PIN_ACCEL_Y,INPUT);
   pinMode (PIN_ACCEL_Z,INPUT);
   
   pinMode (PIN_ENCODER_A,INPUT);
   pinMode (PIN_ENCODER_B,INPUT);
   pinMode (PIN_SWITCH,INPUT);
   pinMode (PIN_PUSHBUTTON,INPUT);
   pinMode (PIN_BUZZER,OUTPUT);   
   
   Serial.begin (9600);
 } 
 
 void readEncoder(){
   encoderRead = digitalRead(PIN_ENCODER_A); 
   if ((encoderPinALast == LOW) && (encoderRead == HIGH)) {
     if (digitalRead(PIN_ENCODER_B) == LOW) {
       encoder_position--;
     } else {
       encoder_position++;
     }
   } 
   encoderPinALast = encoderRead;
 }
 
 void readPushButton(){
    push_on = digitalRead(PIN_PUSHBUTTON);  
 }
 
 void readSwitch(){
    switch_on = digitalRead(PIN_SWITCH);
 }
 
 void readPotentio(){
    potentio = analogRead(PIN_POTENTIO);
 }
 
 void readAccelero(){
  accelX = analogRead(PIN_ACCEL_X);
  accelY = analogRead(PIN_ACCEL_Y);
  accelZ = analogRead(PIN_ACCEL_Z);
 }
 
 void writeBuzzer(){
  tone(PIN_BUZZER, 500, 500);   
  /*PIN_BUZZER = HIGH;
  delay(100);
  PIN_BUZZER = LOW;
  */
 }
 
 void sendSerial(){
   Serial.print(switch_on);
   Serial.print(":");
   Serial.print(push_on);
   Serial.print(":");
   Serial.print(encoder_position);
   Serial.print(":");
   Serial.print(potentio);
   Serial.print(":");
   Serial.print(accelX);
   Serial.print(".");
   Serial.print(accelY);
   Serial.print(".");
   Serial.println(accelZ);
 }

 void receiveSerial() {
   if (Serial.available() > 0) {
     Serial.println("SUCCES");
    incomingByte = Serial.read();
    if (incomingByte == 'S') {
      incomingByte = Serial.read();
      if (incomingByte == 'H') {
        writeBuzzer();
      }
    }
  }
 }
 
 void loop() { 
  readEncoder();
  readPushButton();
  readSwitch();
  readPotentio();
  readAccelero();
        
  loopNr++;
  if(loopNr > 100){
    sendSerial();
    receiveSerial();
    loopNr = 0;
  }
 } 
