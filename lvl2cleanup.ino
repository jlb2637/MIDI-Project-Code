/*
 * Custom MIDI controller software by Jake Butler
 * Inspired by the MIDI-fighter-64
 * Code base branched off of Francois Beat (I think)
*/
#include "MIDIUSB.h"
#include <FastLED.h>

#define LED_CONTROL 6
#define LED_COUNT   30

/************* Object creation ****************/

//led strip used to backlight buttons
CRGB leds[LED_COUNT];

/************* Constants ****************/
const int N_BUTTONS = 30;
const int N_REGISTERS = 4;

/************* Arduino Pin Defines ***********/
int load = 3;
int dataIn = 4;
int clockIn = 2;


//this is used to set all digital inputs low on startup
int inArr[30] = {};

//for leds on the page set buttons
//this gets set to the button value so it can be directly used to turn off the last used
//page
int currentPage = 0;

/************* MIDI Protocol Setup ***********/
byte midiCh = 1;
byte noteSet = 0;

int buttonCState[N_BUTTONS] = {};        // stores the button current value
int buttonPState[N_BUTTONS] = {};        // stores the button previous value
/*this is the final inoming data from the shift register board, each bit represents the state of a button (on/off)*/
unsigned long Lbuttons = 0;

// debounce
unsigned long lastDebounceTime[N_BUTTONS] = {0};  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    //* the debounce time; increase if the output flickers

/************* Setup Function **************/
void setup() {

  // Setup 74HC165 connections
  pinMode(load, OUTPUT);
  //pinMode(clockEnablePin, OUTPUT);
  pinMode(clockIn, OUTPUT);
  pinMode(dataIn, INPUT);

  //set output pins
  digitalWrite(load, HIGH);
  digitalWrite(clockIn, LOW);

  //set inital input array to zero, I changed this back to hardcoded to cut down on variables after final button layout was chosen
  for(int i = 0; i < 30; i++){
    inArr[i] = LOW;
  }

  //led setup
  FastLED.addLeds<NEOPIXEL, LED_CONTROL>(leds, LED_COUNT);
  delay(100); //bad?
  startupAnimation();
  leds[currentPage] = CRGB::Red;
  FastLED.show();

}

/*********** Main Loop Function **********/
void loop() {

  Lbuttons = getInput();

  //thinking about moving to a separate function
  for(int i = 0; i < N_BUTTONS; i++){
    //check each bit for high or low
    int currentBit = bitRead(Lbuttons,i);
    //update the current button state array
    buttonCState[i] = currentBit;

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      //check if the past state is different from current state
      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonCState[i] == 1) {
            //button is pressed
            Serial.println("button pressed!");

            switch (i) {
              case 0:
                leds[currentPage] = CRGB::Black;
                leds[i] = CRGB::Red;
                noteSet = 0;
                currentPage = 0;
              break;
              case 11:
                leds[currentPage] = CRGB::Black;
                leds[i] = CRGB::Red;
                noteSet = 25;
                currentPage = 11;
              break;
              case 12:
                leds[currentPage] = CRGB::Black;
                leds[i] = CRGB::Red;
                noteSet = 50;
                currentPage = 12;
              break;
              case 23:
                leds[currentPage] = CRGB::Black;
                leds[i] = CRGB::Red;
                noteSet = 75;
                currentPage = 23;
              break;
              case 24:
                leds[currentPage] = CRGB::Black;
                leds[i] = CRGB::Red;
                noteSet = 100;
                currentPage = 24;
              break;
              default:
              leds[i] = CRGB::Blue;
              noteOn(0, noteSet + i, 64);
              MidiUSB.flush();
              break;
            }
          }else {
            //buton is released
            switch (i){
              case 0:
              break;
              case 11:
              break;
              case 12:
              break;
              case 23:
              break;
              case 24:
              break;
              default:
                Serial.println("button released!");
                leds[i] = CRGB::Black;
                noteOff(0, noteSet + i, 64);
                MidiUSB.flush();
              break;
            }
          }
          buttonPState[i] = buttonCState[i];
        }
      }
   }
   FastLED.show();
}


//TODO
//midi cc button function

/******** Helper Functions *******/

//separating the input out of the main loop for clarity
unsigned long getInput(){
  
  // Write pulse to load pin
  digitalWrite(load, LOW);
  delayMicroseconds(5);
  digitalWrite(load, HIGH);
  delayMicroseconds(5);

  byte incoming1 = shiftIn165(dataIn, clockIn, LSBFIRST);
  byte incoming2 = shiftIn165(dataIn, clockIn, LSBFIRST);
  byte incoming3 = shiftIn165(dataIn, clockIn, LSBFIRST);
  byte incoming4 = shiftIn165(dataIn, clockIn, LSBFIRST);

 //concatenating all the incoming bytes
  Lbuttons = ((uint32_t)incoming4 << 24) | ((uint32_t)incoming3 << 16) | ((uint32_t)incoming2 << 8) | (uint32_t)incoming1;
  return Lbuttons;
}


//the builtin function did the clocking backwards?!?!
uint8_t shiftIn165(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
  uint8_t value = 0;
  uint8_t i;

  for (i = 0; i < 8; ++i) {
    digitalWrite(clockPin, LOW);
    if (bitOrder == LSBFIRST) {
      value |= digitalRead(dataPin) << i;
    }
    else {
      value |= digitalRead(dataPin) << (7 - i);
    }
    digitalWrite(clockPin, HIGH);
  }
  return value;
}

void noteOn(byte channel, byte pitch, byte velocity){
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity){
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

/*** LED Animations ***/
void startupAnimation(){
  for(int i = 0; i < 30; i++){
    leds[i] = CRGB::Green;
    FastLED.show();
    delay(50);  
  }

  //cleanup loop
  for(int i = 0; i < 30; i++){
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}
