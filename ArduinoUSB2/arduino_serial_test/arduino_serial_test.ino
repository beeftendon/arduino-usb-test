int ledPin = 13;
char state1[20];
char state2[20];
char state3[20];
void setup() {
    pinMode(ledPin, OUTPUT); // pin will be used to for output
    Serial.begin(500000); // same as in your c++ script
}

void loop() {
  if (Serial.available() > 0)
  {
    state1 = Serial.read(); // used to read incoming data
    state2 = Serial.read(); // used to read incoming data
    state3 = Serial.read(); // used to read incoming data
    Serial.println(state1);
    Serial.println(state2);
    Serial.println(state3);
    /*
    switch(state)// see what was sent to the board
    {
      case '1': // if the the one was sent
        digitalWrite(ledPin,HIGH);
        //Serial.println("HIGH!");
      break;
      case '0': // if 0 was sent
        digitalWrite(ledPin,LOW);
        //Serial.println("LOW!");
      break;
      default:
      break;
    }
    */
  }
}
