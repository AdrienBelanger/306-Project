#include <LiquidCrystal.h>
#include <usb_midi.h>

LiquidCrystal lcd(5, 4, 3, 2, 1, 0);

int pedal_two = 0;
int pedal_one = 0;

int count_beats = 0;

String lastMessage = "";
String message = "";

unsigned long last_time_of_hit = 0;
unsigned long second_last_time_of_hit = 0; 
unsigned long last_beat_time = 0;
unsigned long last_clock_time = 0; 
unsigned long square_display_start_time = 0;

int interval = 0;
int bpm = 0;
int bpm_at_prev = 0;
int last_bpm = 0;
int cur_state = 0;

bool square_displayed = false;

void setup() {
  lcd.begin(16, 2);
  
  pinMode(6, INPUT_PULLUP); 
  pinMode(19, INPUT_PULLUP);
  
  pinMode(22, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  pinMode(20, INPUT_PULLUP);

  Serial.begin(9600);

  last_time_of_hit = millis();
  last_clock_time = millis();
}

void loop() {
  

  // ############################## INPUT ##############################
    int pedal1State = digitalRead(6);
    pedal_one = (pedal1State == LOW) ? 0 : 1;

    int pedal2State = digitalRead(19);
    pedal_two = (pedal2State == LOW) ? 0 : 1;

    
    int state0 = 1 - digitalRead(22); 
    int state1 = 1 - digitalRead(21); 
    int state2 = 1 - digitalRead(20); 
  
  // ############################## MODE SWITCHING ##############################
    if (state0 == 1 && state1 == 0 && state2 == 0) {
      message = "    WELCOME";
      lcd.setCursor(0, 1);
      lcd.print("                ");


      if (cur_state != 0) { // Reset variables only when entering WELCOME state
      bpm = 0;
      count_beats = 0;
      last_time_of_hit = 0;
      second_last_time_of_hit = 0;
      last_bpm = 0;
      square_displayed = false;
      }

    cur_state = 0;


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
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print(message);
      lastMessage = message;

      if (cur_state == 2) {
        last_bpm = bpm;
      }
    }

  // ############################## BPM CALCULATION ##############################
    if (cur_state != 0 && pedal_one == 1 && pedal_two == 1) { 
      unsigned long current_time = millis();
      interval = current_time - last_time_of_hit;
      if (interval > 200) {
        second_last_time_of_hit = last_time_of_hit; 
        last_time_of_hit = current_time;

        if (second_last_time_of_hit > 0) { 
          bpm = 60000 / (last_time_of_hit - second_last_time_of_hit);

          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.print("BPM: ");
          lcd.setCursor(5, 1);
          lcd.print(bpm);
        }
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

  // ############################## MIDI CLOCK ##############################
    unsigned long current_time = millis();

  
    if (bpm > 0) {
      int clock_interval = 60000 / (bpm * 24); 
      if (current_time - last_clock_time >= clock_interval) {
        last_clock_time = current_time;
        usbMIDI.sendRealTime(usbMIDI.Clock); 
      }

      int beat_interval = 60000 / bpm; 
      if (current_time - last_beat_time >= beat_interval) {
        last_beat_time = current_time;
        count_beats += 1;
        Serial.print("Beats at this BPM: ");
        Serial.println(count_beats);

        
        
          square_display_start_time = current_time;
          square_displayed = true;
          lcd.setCursor(15, 1);
          lcd.print("\xFF");
        
        delay(10);
        
      }

  
    if (square_displayed && current_time - square_display_start_time >= 200) {
      lcd.setCursor(15, 1);
      lcd.print(" "); 
      square_displayed = false;
    }
  }

  bpm_at_prev = bpm;
}