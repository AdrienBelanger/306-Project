#include <LiquidCrystal.h>

LiquidCrystal lcd(5, 4, 3, 2, 1, 0);

void setup() {
  lcd.begin(16, 2);
  lcd.print("Hello, World!");
  pinMode(6, INPUT_PULLUP);// pedal
  pinMode(19, INPUT_PULLUP); //pedal
  
  pinMode(22, INPUT_PULLUP); // rotary switch
  pinMode(21, INPUT_PULLUP); // rotary switch
  pinMode(20, INPUT_PULLUP); // rotary switch
  Serial.begin(9600);
}

void loop() {
  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);

  int pedal1State = digitalRead(6);

  if (pedal1State == LOW) {
    Serial.println("Pedal 1 is not pressed");
  } else {
    Serial.println("Pedal 1 is pressed");
  }

  int pedal2State = digitalRead(19);

  if (pedal2State == LOW) {
    Serial.println("Pedal 2 is not pressed");
  } else {
    Serial.println("Pedal 2 is pressed");
  }


  // make sure states are 0 when off and 1 when on
  int state0 = 1 - digitalRead(22); // right
  int state1 = 1 - digitalRead(21); // center
  int state2 = 1- digitalRead(20);  // left

  Serial.print("Pin 0: ");
  Serial.print(state0);
  Serial.print(" | Pin 1: ");
  Serial.print(state1);
  Serial.print(" | Pin 2: "); 
  Serial.println(state2);
}
