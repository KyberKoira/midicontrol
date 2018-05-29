#include <MIDI.h>
#include "Controller.h"
#include <SFE_BMP180.h>
#include <Wire.h>
#include <LiquidCrystal.h>

SFE_BMP180 pressure;

const int rs = 12, en = 13, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
//const int rs = 12, en = 13, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define ALTITUDE 1655.0

int asteikko_arvo = 0;
int savel_counter = 0;
int asteikko_arvo_vanha = 0;
int savel_counter_vanha = 0;


/*
 * 0 = Normi
 * 1 = Ionian
 * 2 = Dorian
 * 3 = Phrygian
 * 4 = Lydian
 * 5 = Mixolydian
 * 6 = Aeolian
 * 7 = Locrian
 */

int ASTEIKOT[7][7] = {{2, 4, 5, 7, 9, 11, 12}, {2, 3, 5, 7, 9, 10, 12}, {1, 3, 5, 7, 8, 10, 12}, {2, 4, 6, 7, 9, 11, 12}, {2, 4, 5, 7, 9, 10, 12}, {2, 3, 5, 7, 8, 10, 12}, {1, 3, 5, 6, 8, 10, 12}};
char NIMI_ASTEIKKO[7][12] = {"Ionian", "Dorian", "Phrygian", "Lydian", "Mixolydian", "Aeolian", "Locrian"};
char NIMI_SAVEL[12][3] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

/*************************************************************
  MIDI CONTROLLER

  by Notes and Volts
  www.notesandvolts.com

  Version 1.2 **Arduino UNO ONLY!**
 *************************************************************/

MIDI_CREATE_DEFAULT_INSTANCE();

//************************************************************
//***SET THE NUMBER OF CONTROLS USED**************************
//************************************************************
//---How many buttons are connected directly to pins?---------
byte NUMBER_BUTTONS = 15;
//---How many potentiometers are connected directly to pins?--
byte NUMBER_POTS = 6;
//---How many buttons are connected to a multiplexer?---------
byte NUMBER_MUX_BUTTONS = 0;
//---How many potentiometers are connected to a multiplexer?--
byte NUMBER_MUX_POTS = 0;
//************************************************************

//***ANY MULTIPLEXERS? (74HC4067)************************************
//MUX address pins must be connected to Arduino UNO pins 2,3,4,5
//A0 = PIN2, A1 = PIN3, A2 = PIN4, A3 = PIN5
//*******************************************************************
//Mux NAME (OUTPUT PIN, , How Many Mux Pins?(8 or 16) , Is It Analog?);


//Mux M1(10, 16, false); //Digital multiplexer on Arduino pin 10
//Mux M2(A5, 8, true); //Analog multiplexer on Arduino analog pin A0
//*******************************************************************


//***DEFINE DIRECTLY CONNECTED POTENTIOMETERS************************
//Pot (Pin Number, Command, CC Control, Channel Number)
//**Command parameter is for future use**

Pot PO1(A5, 1, 1, 1);
Pot PO2(A3, 1, 10, 2);
Pot PO3(A2, 1, 0, 3);
Pot PO4(A1, 1, 22, 4);
Pot PO5(A0, 1, 30, 5);
Pot PO6(A4, 1, 300, 6);

//*******************************************************************
//Add pots used to array below like this->  Pot *POTS[] {&PO1, &PO2, &PO3, &PO4, &PO5, &PO6};
Pot *POTS[] {&PO1, &PO2, &PO3, &PO4, &PO5, &PO6};
//*******************************************************************


//***DEFINE DIRECTLY CONNECTED BUTTONS*******************************
//Button (Pin Number, Command, Note Number, Channel, Debounce Time)
//** Command parameter 0=NOTE  1=CC  2=Toggle CC **


Button BU1(37, 0, 60, 1, 5 );
Button BU2(35, 0, 62, 1, 5 );
Button BU3(33, 0, 64, 1, 5 );
Button BU4(31, 0, 65, 1, 5 );
Button BU5(29, 0, 67, 1, 5 );
Button BU6(27, 0, 69, 1, 5 );
Button BU7(25, 0, 72, 1, 5 );

Button BU8(39, 0, 71, 1, 5 );
Button BU9(41, 0, 74, 1, 5 );
Button BU10(43, 0, 76, 1, 5 );
Button BU11(45, 0, 77, 1, 5 );
Button BU12(47, 0, 79, 1, 5 );
Button BU13(49, 0, 81, 1, 5 );
Button BU14(51, 0, 83, 1, 5 );
Button BU15(53, 0, 84, 1, 1 );

//*******************************************************************
//Add buttons used to array below like this->  Button *BUTTONS[] {&BU1, &BU2, &BU3, &BU4, &BU5, &BU6, &BU7, &BU8};
Button *BUTTONS[] {&BU1, &BU2, &BU3, &BU4, &BU5, &BU6, &BU7, &BU8, &BU9, &BU10, &BU11, &BU12, &BU13, &BU14, &BU15};

//*******************************************************************


//***DEFINE BUTTONS CONNECTED TO MULTIPLEXER*************************
//Button::Button(Mux mux, byte muxpin, byte command, byte value, byte channel, byte debounce)
//** Command parameter 0=NOTE  1=CC  2=Toggle CC **

//Button MBU1(M1, 0, 0, 70, 1, 5);
//Button MBU2(M1, 1, 1, 71, 1, 5);
//Button MBU3(M1, 2, 2, 72, 1, 5);
//Button MBU4(M1, 3, 0, 73, 1, 5);
//Button MBU5(M1, 4, 0, 74, 1, 5);
//Button MBU6(M1, 5, 0, 75, 1, 5);
//Button MBU7(M1, 6, 0, 76, 1, 5);
//Button MBU8(M1, 7, 0, 77, 1, 5);
//Button MBU9(M1, 8, 0, 78, 1, 5);
//Button MBU10(M1, 9, 0, 79, 1, 5);
//Button MBU11(M1, 10, 0, 80, 1, 5);
//Button MBU12(M1, 11, 0, 81, 1, 5);
//Button MBU13(M1, 12, 0, 82, 1, 5);
//Button MBU14(M1, 13, 0, 83, 1, 5);
//Button MBU15(M1, 14, 0, 84, 1, 5);
//Button MBU16(M1, 15, 0, 85, 1, 5);
//*******************************************************************
////Add multiplexed buttons used to array below like this->  Button *MUXBUTTONS[] {&MBU1, &MBU2, &MBU3, &MBU4, &MBU5, &MBU6.....};
Button *MUXBUTTONS[] {};

//*******************************************************************


//***DEFINE POTENTIOMETERS CONNECTED TO MULTIPLEXER*******************
//Pot::Pot(Mux mux, byte muxpin, byte command, byte control, byte channel)
//**Command parameter is for future use**

//Pot MPO1(M2, 0, 0, 1, 1);
//Pot MPO2(M2, 1, 0, 7, 1);
//Pot MPO3(M2, 2, 0, 50, 1);
//Pot MPO4(M2, 3, 0, 55, 2);
//Pot MPO5(M2, 4, 0, 50, 1);
//Pot MPO6(M2, 5, 0, 55, 2);
//Pot MPO7(M2, 6, 0, 50, 1);
//Pot MPO8(M2, 7, 0, 55, 2);
//Pot MPO9(M2, 8, 0, 50, 1);
//Pot MPO10(M2, 9, 0, 55, 2);
//Pot MPO11(M2, 10, 0, 50, 1);
//Pot MPO12(M2, 11, 0, 55, 2);
//Pot MPO13(M2, 12, 0, 50, 1);
//Pot MPO14(M2, 13, 0, 55, 2);
//Pot MPO15(M2, 14, 0, 50, 1);
//Pot MPO16(M2, 15, 0, 55, 2);
//*******************************************************************
//Add multiplexed pots used to array below like this->  Pot *MUXPOTS[] {&MPO1, &MPO2, &MPO3, &MPO4, &MPO5, &MPO6.....};
Pot *MUXPOTS[] {};
//*******************************************************************

void asteikko(){
  for (int i = 1; i < NUMBER_BUTTONS; i = i + 1) {
    if(i<8){
      BUTTONS[i]->Bvalue = BUTTONS[0]->Bvalue + ASTEIKOT[asteikko_arvo][i - 1];
    }
    else{
      BUTTONS[i]->Bvalue = BUTTONS[0]->Bvalue + 12 + ASTEIKOT[asteikko_arvo][i - 8];
    }
  }
}

void setup() {
  MIDI.begin(MIDI_CHANNEL_OFF);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(3, OUTPUT);

  asteikko();
  
  pressure.begin();
  lcd.begin(20, 4);
  lcd.setCursor(1, 1);
  lcd.print(String(NIMI_SAVEL[savel_counter]));
  lcd.setCursor(10, 1);
  lcd.print(String(NIMI_ASTEIKKO[asteikko_arvo]));
}

//Tarvitaan näitä että saadaan homma toimimaan:

long time = 0;
long debounce = 1;

/*
 * 0 = Normi
 * 1 = Ionian
 * 2 = Dorian
 * 3 = Phrygian
 * 4 = Lydian
 * 5 = Mixolydian
 * 6 = Aeolian
 * 7 = Locrian
 */

//Näitä myös:

int previous_savel_up = HIGH;
int previous_savel_down = HIGH;

int previous_asteikko_up = HIGH;
int previous_asteikko_down = HIGH;


//asteikko vaihtuu perustuen asteikko_arvo-arvoon



// isOK-funktio tarkistaa että onko mennyt tarpeeksi aikaa viime outputista
int isOK(){
  if (millis() - time > debounce) {
    return HIGH;
    time = millis();
  }
  else{
    return LOW;
  }
}

void loop() {

  if(asteikko_arvo != asteikko_arvo_vanha || savel_counter != savel_counter_vanha){
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print(String(NIMI_SAVEL[savel_counter]));
    lcd.setCursor(10, 1);
    lcd.print(String(NIMI_ASTEIKKO[asteikko_arvo]));
    
  }

  char status;
  double T,P,p0,a,S,Y;

  status = pressure.startTemperature();
  delay(status);
  status = pressure.getTemperature(T);
  status = pressure.startPressure(3);
  delay(status);
  status = pressure.getPressure(P,T);

  S = P - 1017;
  Y = map(S, 0, 65, 0, 255);
  if (S > 65)
  {
    Y = 127;
  }

  if (S < 0)
  {
    Y = 0;
  }

  analogWrite(3, Y);

//Tutkitaan mitä dataa löytyy:
  
  int savel_up = digitalRead(4);
  int savel_down = digitalRead(5);
  int asteikko_up = digitalRead(7);
  int asteikko_down = digitalRead(6);

//Sävelenvaihtologiikka:

  if(savel_up == LOW && previous_savel_up == HIGH && previous_savel_down == HIGH  && savel_down == HIGH && isOK() == HIGH && savel_counter < 11){
    for (int i = 0; i < NUMBER_BUTTONS; i = i + 1) {
      BUTTONS[i]->Bvalue += 1;
    }
    savel_counter_vanha = savel_counter;
    savel_counter += 1;
  }

  if(savel_down == LOW && previous_savel_down == HIGH && previous_savel_up == HIGH  && savel_up == HIGH  && isOK() == HIGH && savel_counter >= 1){
    for (int i = 0; i < NUMBER_BUTTONS; i = i + 1) {
      BUTTONS[i]->Bvalue -= 1;
    }
    savel_counter_vanha = savel_counter;
    savel_counter -= 1;
  }

//Asteikonvaihtologiikka:

  if(asteikko_up == LOW && previous_asteikko_up == HIGH && previous_asteikko_down == HIGH  && asteikko_down == HIGH  && isOK() == HIGH){
    if(asteikko_arvo < 6 && asteikko_arvo >= 0){
      asteikko_arvo_vanha = asteikko_arvo;
      asteikko_arvo += 1;
      asteikko();
    }
    
  }

  if(asteikko_down == LOW && previous_asteikko_down == HIGH && previous_asteikko_up == HIGH  && asteikko_up == HIGH  && isOK() == HIGH){
    if(asteikko_arvo < 8 && asteikko_arvo >= 1){
      asteikko_arvo_vanha = asteikko_arvo;
      asteikko_arvo -= 1;
      asteikko();
    }
  }

//Tallennetaan edelliset arvot:

  previous_asteikko_up = asteikko_up;
  previous_asteikko_down = asteikko_down;
  previous_savel_up = savel_up;
  previous_savel_down = savel_down;
  
  if (NUMBER_BUTTONS != 0) updateButtons();
  if (NUMBER_POTS != 0) updatePots();
  if (NUMBER_MUX_BUTTONS != 0) updateMuxButtons();
  if (NUMBER_MUX_POTS != 0) updateMuxPots();
  
  
}


//*****************************************************************
void updateButtons() {

  // Cycle through Button array
  for (int i = 0; i < NUMBER_BUTTONS; i = i + 1) {
    byte message = BUTTONS[i]->getValue();

    //  Button is pressed
    if (message == 0) {
      switch (BUTTONS[i]->Bcommand) {
        case 0: //Note
          MIDI.sendNoteOn(BUTTONS[i]->Bvalue, 127, BUTTONS[i]->Bchannel);
          break;
        case 1: //CC
          MIDI.sendControlChange(BUTTONS[i]->Bvalue, 127, BUTTONS[i]->Bchannel);
          break;
        case 2: //Toggle
          if (BUTTONS[i]->Btoggle == 0) {
            MIDI.sendControlChange(BUTTONS[i]->Bvalue, 127, BUTTONS[i]->Bchannel);
            BUTTONS[i]->Btoggle = 1;
          }
          else if (BUTTONS[i]->Btoggle == 1) {
            MIDI.sendControlChange(BUTTONS[i]->Bvalue, 0, BUTTONS[i]->Bchannel);
            BUTTONS[i]->Btoggle = 0;
          }
          break;
      }
    }

    //  Button is not pressed
    if (message == 1) {
      switch (BUTTONS[i]->Bcommand) {
        case 0:
          MIDI.sendNoteOff(BUTTONS[i]->Bvalue, 0, BUTTONS[i]->Bchannel);
          break;
        case 1:
          MIDI.sendControlChange(BUTTONS[i]->Bvalue, 0, BUTTONS[i]->Bchannel);
          break;
      }
    }
  }
}
//*******************************************************************
void updateMuxButtons() {

  // Cycle through Mux Button array
  for (int i = 0; i < NUMBER_MUX_BUTTONS; i = i + 1) {

    MUXBUTTONS[i]->muxUpdate();
    byte message = MUXBUTTONS[i]->getValue();

    //  Button is pressed
    if (message == 0) {
      switch (MUXBUTTONS[i]->Bcommand) {
        case 0: //Note
          MIDI.sendNoteOn(MUXBUTTONS[i]->Bvalue, 127, MUXBUTTONS[i]->Bchannel);
          break;
        case 1: //CC
          MIDI.sendControlChange(MUXBUTTONS[i]->Bvalue, 127, MUXBUTTONS[i]->Bchannel);
          break;
        case 2: //Toggle
          if (MUXBUTTONS[i]->Btoggle == 0) {
            MIDI.sendControlChange(MUXBUTTONS[i]->Bvalue, 127, MUXBUTTONS[i]->Bchannel);
            MUXBUTTONS[i]->Btoggle = 1;
          }
          else if (MUXBUTTONS[i]->Btoggle == 1) {
            MIDI.sendControlChange(MUXBUTTONS[i]->Bvalue, 0, MUXBUTTONS[i]->Bchannel);
            MUXBUTTONS[i]->Btoggle = 0;
          }
          break;
      }
    }
    //  Button is not pressed
    if (message == 1) {
      switch (MUXBUTTONS[i]->Bcommand) {
        case 0:
          MIDI.sendNoteOff(MUXBUTTONS[i]->Bvalue, 0, MUXBUTTONS[i]->Bchannel);
          break;
        case 1:
          MIDI.sendControlChange(MUXBUTTONS[i]->Bvalue, 0, MUXBUTTONS[i]->Bchannel);
          break;
      }
    }
  }
}
//***********************************************************************
void updatePots() {
  for (int i = 0; i < NUMBER_POTS; i = i + 1) {
    byte potmessage = POTS[i]->getValue();
    if (potmessage != 255) MIDI.sendControlChange(POTS[i]->Pcontrol, potmessage, POTS[i]->Pchannel);
  }
}
//***********************************************************************
void updateMuxPots() {
  for (int i = 0; i < NUMBER_MUX_POTS; i = i + 1) {
    MUXPOTS[i]->muxUpdate();
    byte potmessage = MUXPOTS[i]->getValue();
    if (potmessage != 255) MIDI.sendControlChange(MUXPOTS[i]->Pcontrol, potmessage, MUXPOTS[i]->Pchannel);
  }
}

