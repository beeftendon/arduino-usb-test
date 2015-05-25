int ledPin = 13;
int state=0;
void setup() {
    pinMode(ledPin, OUTPUT); // pin will be used to for output
    Serial.begin(115200); // same as in your c++ script
}

void loop() {
  if (Serial.available() > 0)
  {
    state = Serial.read(); // used to read incoming data
    Serial.println("1234567 1234567 1234567");
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
