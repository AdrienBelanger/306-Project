#include <LiquidCrystal.h>
#include <usb_midi.h>

LiquidCrystal lcd(5, 4, 3, 2, 1, 0);

int pedal_two = 0;
int pedal_one = 0;

int count_beats = 0;

String lastMessage = "";
String message = "";

unsigned long last_time_of_hit = 0;
unsigned long last_beat_time = 0;
unsigned long last_clock_time = 0; // For MIDI Clock timing
unsigned long square_display_start_time = 0;

int interval = 0;
int bpm = 0;
int bpm_at_prev = 0;
int last_bpm = 0;
int cur_state = 0;

bool square_displayed = false; // Tracks if the square is currently displayed

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
  // PEDALS INPUT
  int pedal1State = digitalRead(6);
  pedal_one = (pedal1State == LOW) ? 0 : 1;

  int pedal2State = digitalRead(19);
  pedal_two = (pedal2State == LOW) ? 0 : 1;

  // ROTARY SWITCH INPUT
  int state0 = 1 - digitalRead(22); // right
  int state1 = 1 - digitalRead(21); // center
  int state2 = 1 - digitalRead(20); // left

  // MAIN CODE
  if (state0 == 1 && state1 == 0 && state2 == 0) {
    message = "    WELCOME";
    lcd.setCursor(0, 1);
    lcd.print("                ");
    cur_state = 0;
  } else if (state0 == 0 && state1 == 1 && state2 == 0) { 
    message = "  FOLLOW MODE";
    cur_state = 1;
  } else if (state0 == 0 && state1 == 0 && state2 == 1) { 
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
    bpm = last_bpm;
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("BPM: ");
    lcd.setCursor(5, 1);
    lcd.print(bpm);
  }

  unsigned long current_time = millis();

  // MIDI Clock sending
  if (bpm > 0) {
    int clock_interval = 60000 / (bpm * 24); // Calculate interval between MIDI Clock pulses
    if (current_time - last_clock_time >= clock_interval) {
      last_clock_time = current_time;
      usbMIDI.sendRealTime(usbMIDI.Clock); // Send MIDI Clock pulse
    }

    // Beat counting and square display
    int beat_interval = 60000 / bpm; // Calculate time per beat in milliseconds
    if (current_time - last_beat_time >= beat_interval) {
      last_beat_time = current_time;
      count_beats += 1;
      Serial.print("Beats at this BPM: ");
      Serial.println(count_beats);

      usbMIDI.sendNoteOn(60, 127, 1); // Note C4, Velocity 127, Channel 1
      if (bpm < 180) {
        square_display_start_time = current_time;
        square_displayed = true;
        lcd.setCursor(15, 1);
        lcd.print("\xFF"); // Display a square character
      }
      delay(10);
      usbMIDI.sendNoteOff(60, 0, 1); // Note C4, Velocity 0, Channel 1
    }

    // Remove square after 200ms
    if (square_displayed && current_time - square_display_start_time >= 200) {
      lcd.setCursor(15, 1);
      lcd.print(" "); // Clear the square
      square_displayed = false;
    }
  }

  bpm_at_prev = bpm;
}
