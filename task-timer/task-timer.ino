#include <LiquidCrystal.h>

/*** Task related variables ***/
String current_task = "";
unsigned long task_ticks = 0;

/*** Work / Free time timer variables ***/
boolean state = true; //true = working / false = freetime
const byte SWITCH = 11;
const short ALARM_TRIGGER = 7200; // 2 hours working time
const byte ALARM_LED = 13;
unsigned long alarm_ticks = 0;
const short FREE_TRIGGER = 900; // 15 min free time
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
  if (current_task != ""){ //Don't update unless working on something
    if (state){
      alarm_ticks++;
      if (alarm_ticks == ALARM_TRIGGER)
        digitalWrite(ALARM_LED, HIGH);
    }
    else{
      free_ticks++;
      if (free_ticks == FREE_TRIGGER)
        digitalWrite(FREE_LED, HIGH);
    }
    task_ticks++;
    update_lcd();
  }
}

//set timer1 interrupt at 1Hz
void set_timer(){
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
 lcd.print("task-timer v0.1");
}

void update_lcd(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(current_task);
  lcd.setCursor(0,1);
  lcd.print(task_ticks/3600);
  lcd.setCursor(2,1);
  lcd.print(":");
  lcd.print((task_ticks/60)%60);
  lcd.setCursor(5,1);
  lcd.print(":");
  lcd.print(task_ticks%60);
  lcd.setCursor(8,1);
}

/*** General Functions ***/
void complete_task(){
  if (current_task != ""){
    Serial.print(current_task);
    Serial.print("-");
    Serial.println(task_ticks);
    task_ticks = 0;
  } 
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
    complete_task();
    current_task = input_string;
    input_string = "";
    string_complete = false;
    update_lcd();
  }
  if (digitalRead(SWITCH) == HIGH && state) {
    // Going to free time
    alarm_ticks = 0;
    digitalWrite(ALARM_LED, LOW);
    state = !state;
    complete_task();
    current_task = "freetime";
    delay(500);
  }
  if (digitalRead(SWITCH) == HIGH && !state){
    // Coming back from free time
    free_ticks = 0;
    digitalWrite(FREE_LED, LOW);
    state = !state;
    complete_task();
    current_task = "";
    init_lcd();
    delay(500);
  }
}
