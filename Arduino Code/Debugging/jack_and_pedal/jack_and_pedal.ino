void setup() {
  pinMode(1, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  int pedalState = digitalRead(1);

  if (pedalState == LOW) {
    Serial.println("Pedal is not pressed");
  } else {
    Serial.println("Pedal is pressed");
  }

  delay(200);
}
