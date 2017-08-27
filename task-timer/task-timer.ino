#include <LiquidCrystal.h>

/*** Task related variables ***/
String current_task = "";
unsigned long task_ticks = 0;

/*** Work / Free time timer variables ***/
#define STOP 0
#define PAUSE 1
#define WORK 2
#define FREE 3
byte state = STOP;

const byte SWITCH = 11;
const short ALARM_TRIGGER = 7200; //7200 seconds = 2 hours working time
const byte ALARM_LED = 13;
unsigned long alarm_ticks = 0;
const short FREE_TRIGGER = 900; //900 seconds = 15 min free time
const byte FREE_LED = 12;
unsigned long free_ticks = 0;

/*** LCD related variable ***/
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

/*** Serial handling variables ***/
char inChar = ' ';
String input_string = "";
boolean string_complete = false;

/*** Timer 1 Functions ***/
ISR(TIMER1_COMPA_vect){
  if (state == WORK){
      task_ticks++;
      update_lcd(current_task, task_ticks);
      alarm_ticks++;
      if (alarm_ticks == ALARM_TRIGGER)
        digitalWrite(ALARM_LED, HIGH);
  }
  if (state == FREE){
      free_ticks++;
      update_lcd("FREETIME", free_ticks);
      if (free_ticks == FREE_TRIGGER)
        digitalWrite(FREE_LED, HIGH);
  }
}

void set_timer(){
  //sets timer1 interrupt at 1Hz
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 15624;
  TCCR1B |= (1 << WGM12); //CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10); //1024 prescaler
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

/*** LCD Related Functions ***/
void init_lcd(){
 lcd.begin(16, 2);
 lcd.print("task-timer v0.2");
}

void update_lcd(String text, unsigned long seconds){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(text);
  lcd.setCursor(0,1);
  lcd.print(seconds/3600);
  lcd.setCursor(2,1);
  lcd.print(":");
  lcd.print((seconds/60)%60);
  lcd.setCursor(5,1);
  lcd.print(":");
  lcd.print(seconds%60);
  lcd.setCursor(8,1);
}

/*** General Functions ***/
void complete_task(){
  //Send task information over serial and resets timer
  if (current_task != ""){
    Serial.print(current_task);
    Serial.print("-");
    Serial.println(task_ticks);
    task_ticks = 0;
  } 
}

void set_stop(){
  //Set STOP state
  state = STOP;
  init_lcd();
  complete_task();
  current_task = "";
  alarm_ticks = 0;
  digitalWrite(FREE_LED, LOW);
  digitalWrite(ALARM_LED, LOW);
}

void set_pause(){
  //Set PAUSE state
  state = PAUSE;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(current_task);
  lcd.setCursor(0,1);
  lcd.print("PAUSED");
}

void unset_pause(){
  //Unset pause state
  state = WORK;
}

void set_free(){
  //Set FREE state with timer
  alarm_ticks = 0;
  digitalWrite(ALARM_LED, LOW);
  state = FREE;
  update_lcd("FREETIME", free_ticks);
}

void unset_free(){
  //Unset FREE state
  free_ticks = 0;
  digitalWrite(FREE_LED, LOW);
  state = WORK;
}

void setup() {
 set_timer();
 Serial.begin(9600);

 pinMode(ALARM_LED, OUTPUT);
 pinMode(FREE_LED, OUTPUT);
 pinMode(SWITCH, INPUT);
 digitalWrite(ALARM_LED, LOW);
 digitalWrite(FREE_LED, LOW);

 init_lcd();
}

void serialEvent() {
  while (Serial.available()) {
    inChar = (char)Serial.read();
    if (inChar == '\n') {
      string_complete = true;
      break;
    }
    input_string += inChar;
  }
}

void loop(){
  if (string_complete) {
    // Command from serial
    if (input_string == ","){
      if (state == PAUSE) 
        unset_pause();
      else
        set_pause();
    }
    else if (input_string == "."){
      set_stop();
    }
    else {
      complete_task();
      current_task = input_string;
      update_lcd(current_task, 0);
      state = WORK;
    }
    input_string = "";
    string_complete = false;
  }
  if ((digitalRead(SWITCH) == HIGH) && (state == WORK) ) {
    // Going to free time
    set_free();
    delay(500);
  }
  if ((digitalRead(SWITCH) == HIGH) && (state == FREE) ){
    // Coming back from free time
    unset_free();
    delay(500);
  }
}
