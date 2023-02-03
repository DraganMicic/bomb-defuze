  int Tmin = 20;
int trazeniBr = 25;
String porukica = "trag ispod stola!";

#include <Wire.h>
#include <SevSeg.h>
#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>

const int buzzer = 7;
const int pResistor = A2;
const int R1 = A5;
const int R2 = A6;
int prek1 = 8;
int prek2 = 6;
int prek3 = 5;
int prek4 = 4;
int prek5 = 3;
int kofa = 0;
volatile long echo_start = 0;                      //    Records start of echo pulse
volatile long echo_end = 0;                         //   Records end of echo pulse
volatile long echo_duration = 0;                     //  Duration - difference between end and start
volatile int trigger_time_count = 0;                 //  Count down counter to trigger pulse time
volatile long range_flasher_counter = 0;              // Count down counter for flashing distance LED

#define trigPin 12
#define echoPin 2
#define echo_int 0
#define TIMER_US 50
#define TICK_COUNTS 4000
long duration, distance;
LiquidCrystal_I2C lcd(0x27, 16, 2);   //Set the LCD address to 0x27 for a 16 chars and 2 line display

int bomba = 0;// 1 znaci aktivna moze biti 0 neaktivirana i 2 deaktivirana
int tpocetak = millis();
int ostalomin = Tmin;
int ostaloss = 100;
int ostalos = 0;
String vreme[8];
int calHall;
int calFoto;
SevSeg myDisplay;
String kapija = "XXXXX";


void setup() {
  lcd.begin();  // initialize the LCD
  lcd.backlight();    //Turn on the blacklight and print a message.
  pinMode(buzzer, OUTPUT);
  Serial.begin (1000000);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pResistor, INPUT);
  pinMode(R1, INPUT);
  pinMode(R2, INPUT);
  Timer1.initialize(TIMER_US);               //          Initialise timer 1
  Timer1.attachInterrupt( timerIsr );          //        Attach interrupt to the timer service routine
  attachInterrupt(echo_int, echo_interrupt, CHANGE);  // Attach interrupt to the sensor echo input
  calHall = analogRead(0);
  calFoto = analogRead(2);
  int displayType = COMMON_ANODE; //Your display is either common cathode or common anod
  int digit1 = 22; //Pin 1
  int digit2 = 25; //Pin 10
  int digit3 = 26; //Pin 4
  int digit4 = 33; //Pin 6
  int segA = 23; //Pin 12
  int segB = 27; //Pin 11
  int segC = 31; //Pin 3
  int segD = 29; //Pin 8
  int segE = 28; //Pin 2
  int segF = 24; //Pin 9
  int segG = 32; //Pin 7
  int segDP = 30; //Pin 5
  int numberOfDigits = 4; //Do you have a 1, 2 or 4 digit display
  myDisplay.Begin(displayType, numberOfDigits, digit1, digit2, digit3, digit4, segA, segB, segC, segD, segE, segF, segG, segDP);
  myDisplay.SetBrightness(100); //Set the display to 100% brightness level
  pinMode(prek1, INPUT);
  pinMode(prek2, INPUT);
  pinMode(prek3, INPUT);
  pinMode(prek4, INPUT);
  pinMode(prek5, INPUT);
  lcd.setCursor(0, 0);
  lcd.print(stanjeBombe());
  tone(buzzer, 900);
  noTone(buzzer);
}

String stanjeVreme() {
  ostalos = Tmin* 60 - ((millis() - tpocetak) / 1000);
  ostalomin = ostalos / 60;
  ostaloss = 99 - (millis() / 10 % 100);
  vreme[0] = String(ostalomin  /10);
  vreme[1] = String(ostalomin % 10);
  vreme[2] = ":";
  vreme[3] = String((ostalos - ostalomin * 60) / 10);
  vreme[4] = String((ostalos - ostalomin * 60) % 10);
  vreme[5] = ":";
  vreme[6] = String((99 - ostaloss) / 10);
  vreme[7] = String((99 - ostaloss) % 10);
  return String(vreme[0] + vreme[1] + vreme[2] + vreme[3] + vreme[4] + vreme[5] + vreme[6] + vreme[7]);
}

String stanjeKapija() {
  return kapija;
}

void buzz() {
  if (ostaloss  /10 > 8) {
    tone(buzzer, 900); // Send 1KHz sound signal...
  }
  else {
    noTone(buzzer);      //Stop sound...
  }
}

String stanjeBombe() {
  if (bomba == 1) {
    return "Bomba aktivirana";
  }
  if (bomba == 0 ) {
    return "Bomba neaktivna";
  }
  if ( bomba == 2) {
    return "Deaktivirano";
  }
}
void ultrazvuk() {
  if (((echo_duration  /58) > 5) && ((echo_duration / 58) < 25)) {
    kapija[0] = '0';
  }
  else {
    kapija[0] = 'X';
  }
}

void hall() {
  int raw = analogRead(0);
  if (raw > (calHall + 20)||  raw < (calHall - 15)) {
    kapija[1] = '0';
  }
  else {
    kapija[1] = 'X';
  }
}
void fotootpornik() {
  int value = analogRead(pResistor);
  if (value > (calFoto + 300)) {
    kapija[2] = '0';
  }
  else {
    kapija[2] = 'X';
  }
}
void potenciometri() {
  int prvi = analogRead(R1);
  int drugi = analogRead(R2);
  if (prvi > 400 && prvi < 600 && drugi > 800 && drugi < 1000) {
    kapija[3] = '0';
  }
  else {
    kapija[3] = 'X';
  }
}
void prekidaci() {
  int trenBr[5];
  if (digitalRead(prek1) == HIGH) {
    trenBr[1] = 1;
  } else {
    trenBr[1] = 0;
  }
  if (digitalRead(prek2) == HIGH) {
    trenBr[2] = 1;
  } else {
    trenBr[2] = 0;
  }
  if (digitalRead(prek3) == HIGH) {
    trenBr[3] = 1;
  } else {
    trenBr[3] = 0;
  }
  if (digitalRead(prek4) == HIGH) {
    trenBr[4] = 1;
  } else {
    trenBr[4] = 0;
  }
  if (digitalRead(prek5) == HIGH) {
    trenBr[5] = 1;
  } else {
    trenBr[5] = 0;
  }

  int brojSada = (trenBr[5] * 1 + trenBr[4]*  2 + trenBr[3] * 4 + trenBr[2] * 8 + trenBr[1] * 16);
  char bebe[5]; bebe[0] = brojSada / 1000; bebe[1] = brojSada  /100 % 10; bebe[2] = brojSada / 10 % 100; bebe[3] = brojSada % 10;
  myDisplay.DisplayString(bebe, 0);
  if (brojSada == trazeniBr) {
    kapija[4] = '0';
  }
  else {
    kapija[4] = 'X';
  }
}
void proveriStanaja() {
  ultrazvuk();
  hall();
  fotootpornik();
  potenciometri();
  prekidaci();
  lcd.setCursor(11, 1);
  lcd.print(stanjeKapija());
}

String StaraKapija = "XXXXX";
int StaraBomba = bomba;

void presekStanja() {
  if (bomba == 0) {
    Serial.println("Presek");
    Serial.println("bomba je nekativna");
  }
  if (bomba == 1) {
    Serial.println("Presek");
    Serial.println("bomba je aktivna");
    Serial.print("Kapije" );
    Serial.println(String(kapija));
    Serial.print("Vreme" );
    Serial.println(stanjeVreme());
  }
  if (bomba == 2) {
    Serial.println("Presek");
    Serial.println("bomba je deaktivirana");
  }
}

void proveriStanjeBombe() {
  byte procitano;
  if (Serial.available() > 0) {
    procitano = Serial.read() - 48;
    //Serial.println(procitano);
  }
  if (procitano == 1) {
    bomba = 0;
    lcd.clear();
    Serial.println(stanjeBombe());
  }
  if (procitano == 2) {
    bomba = 1;
    lcd.clear();
    Serial.println(stanjeBombe());
  }
  if (procitano == 3) {
    bomba = 2;
    lcd.clear();
    kofa = 0;
    Serial.println(stanjeBombe());
  }
  if (procitano == 5) {
    presekStanja();
  }

  if (kapija[0] == '0' && kapija[1] == '0' && kapija[2] == '0' && kapija[3] == '0' && kapija[4] == '0') {
    bomba = 2; lcd.clear(); kofa = 0; Serial.println(stanjeBombe());kapija="XXXXX";
  }
}

void loop() {
  proveriStanjeBombe();

  if (bomba == 0) {
    lcd.setCursor(0, 0);//neaktivna
    lcd.print(stanjeBombe());
  }

  if (bomba == 1) {
    buzz();
    proveriStanaja();

    if (bomba != StaraBomba) {
      lcd.setCursor(0, 0);
      lcd.print(stanjeBombe());
    }
    if (kapija != StaraKapija) {
      Serial.print("Kapije");
      Serial.println(String(kapija));
      lcd.setCursor(11, 1);
      lcd.print(stanjeKapija());
    }
    lcd.setCursor(0, 1);
    lcd.print(stanjeVreme());


  }

  if (bomba == 2) {
    lcd.setCursor(0, 0);
    lcd.print(stanjeBombe());

    if (kofa == 0) {
      tone(buzzer, 900);
      delay(1000);
      noTone(buzzer);
    }
    kofa = 1;

    lcd.setCursor(0, 1);
    lcd.print(porukica);

  }
  StaraKapija = kapija;
  StaraBomba = bomba;
}

void timerIsr()
{
  trigger_pulse();                         //         Schedule the trigger pulses
  distance_flasher();                         //      Flash the onboard LED distance indicator
}

void trigger_pulse()
{
  static volatile int state = 0;               //   State machine variable

  if (!(--trigger_time_count))                 //   Count to 200mS
  {  //Time out - Initiate trigger pulse
    trigger_time_count = TICK_COUNTS;          //  Reload
    state = 1;                                 //  Changing to state 1 initiates a pulse
  }

  switch (state)                               //   State machine handles delivery of trigger pulse
  {
    case 0:                                     //  Normal state does nothing
      break;

    case 1 :                                    //  Initiate pulse
      digitalWrite(trigPin, HIGH);              // Set the trigger output high
      state = 2;                                // and set state to 2
      break;

    case 2  :                                    // Complete the pulse
    default:
      digitalWrite(trigPin, LOW);               // Set the trigger output low
      state = 0;                                // and return state to normal 0
      break;
  }
}
void echo_interrupt()
{
  switch (digitalRead(echoPin))                    //  Test to see if the signal is high or low
  {
    case HIGH:                                     //  High so must be the start of the echo pulse
      echo_end = 0;                               //   Clear the end time
      echo_start = micros();                      //   Save the start time
      break;

    case LOW  :                                    //  Low so must be the end of hte echo pulse
      echo_end = micros();                          // Save the end time
      echo_duration = echo_end - echo_start;         //Calculate the pulse duration
      break;
  }
}
void distance_flasher()
{
  if (--range_flasher_counter <= 0)               //  Decrement and test the flash timer
  { // Flash timer time out
    if (echo_duration < 25000)                    // If the echo duration is within limits
    {
      range_flasher_counter = echo_duration * 2;   //Reload the timer with the current echo duration
    }
    else
    {
      range_flasher_counter = 25000;              // If out of range use a default
    }

  }
}
