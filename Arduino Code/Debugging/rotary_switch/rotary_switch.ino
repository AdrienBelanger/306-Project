void setup() {
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  // make sure states are 0 when off and 1 when on
  int state0 = 1 - digitalRead(0); // right
  int state1 = 1 - digitalRead(1); // center
  int state2 = 1- digitalRead(2);  // left

  Serial.print("Pin 0: ");
  Serial.print(state0);
  Serial.print(" | Pin 1: ");
  Serial.print(state1);
  Serial.print(" | Pin 2: "); 
  Serial.println(state2);

  delay(1);
}
