#include <LiquidCrystal.h>
#include <usb_midi.h>

LiquidCrystal lcd(5, 4, 3, 2, 1, 0);

int pedal_two = 0;
int pedal_one = 0;

int count_beats = 0;

String lastMessage = "";

String message = "";

unsigned long last_time_of_hit = 0;

unsigned long last_clock_time = 0; // For MIDI Clock timing
int interval = 0;

int bpm = 0;

int bpm_at_prev = 0;

int last_bpm = 0;

int cur_state = 0;

void setup() {
  lcd.begin(16, 2);
  
  pinMode(6, INPUT_PULLUP); // pedal
  pinMode(19, INPUT_PULLUP); // pedal
  
  pinMode(22, INPUT_PULLUP); // rotary switch
  pinMode(21, INPUT_PULLUP); // rotary switch
  pinMode(20, INPUT_PULLUP); // rotary switch
  Serial.begin(9600);

  last_time_of_hit = millis();
  last_clock_time = millis();
}

void loop() {
  // --------------- INPUT ---------------
  // --------- PEDALS INPUT --------
  int pedal1State = digitalRead(6);

  if (pedal1State == LOW) {
    pedal_one = 0;
  } else {
    pedal_one = 1;
  }

  int pedal2State = digitalRead(19);

  if (pedal2State == LOW) {
    pedal_two = 0;
  } else {
    pedal_two = 1;
  }

  // --------- ROTARY SWITCH INPUT --------

  // Make sure states are 0 when off and 1 when on
  int state0 = 1 - digitalRead(22); // right
  int state1 = 1 - digitalRead(21); // center
  int state2 = 1 - digitalRead(20);  // left

  // --------------- MAIN CODE ---------------
  // PRINT THE CURRENT STATE AND UPDATE

  if (state0 == 1 && state1 == 0 && state2 == 0) {
    message = "    WELCOME";
    lcd.setCursor(0, 1);
    lcd.print("                ");
    cur_state = 0;
  }
  else if (state0 == 0 && state1 == 1 && state2 == 0) { 
    message = "  FOLLOW MODE";
    cur_state = 1;
  }
  else if (state0 == 0 && state1 == 0 && state2 == 1) { 
    message = "   HOLD MODE";
    cur_state = 2;
  }

  if (message != lastMessage) {
    lcd.setCursor(0, 0);
    lcd.print("                "); // Clear previous message
    lcd.setCursor(0, 0);
    lcd.print(message);
    lastMessage = message;

    if (cur_state == 2) {
      last_bpm = bpm;
    }
  }

  if (cur_state != 0 && pedal_one == 1 && pedal_two == 1) { // if welcome, ignore the BPM
    // Give time to debounce. This, however, limits the max taps per second to 5, giving a max bpm of 300
    interval = millis() - last_time_of_hit;
    if (interval > 200) {
      last_time_of_hit = millis();
      bpm = 60000 / interval;

      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("BPM: ");
      lcd.setCursor(5, 1);
      lcd.print(bpm);
    }
  }

  if (cur_state == 2 && pedal_one == 0 && pedal_two == 0 && bpm != last_bpm) {
    // Go back to bpm before we started tapping the bpm
    bpm = last_bpm;
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("BPM: ");
    lcd.setCursor(5, 1);
    lcd.print(bpm);
  }

  // Send MIDI Clock messages
  if (bpm > 0) {
    unsigned long current_time = millis();
    int clock_interval = 60000 / (bpm * 24); // Calculate interval between MIDI Clock pulses
    if (current_time - last_clock_time >= clock_interval) {
      last_clock_time = current_time;
      usbMIDI.sendRealTime(usbMIDI.Clock); // Send MIDI Clock pulse
      Serial.println("MIDI Clock Pulse Sent");
    }
  }

  bpm_at_prev = bpm;
}
