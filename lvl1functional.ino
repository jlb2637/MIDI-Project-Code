#include "MIDIUSB.h"
#include <FastLED.h>

#define LED_CONTROL 6
#define LED_COUNT   30

//MIDI_CREATE_DEFAULT_INSTANCE();

CRGB leds[LED_COUNT];

const int N_BUTTONS = 30;
const int N_REGISTERS = 4;

int load = 3;
int dataIn = 4;
int clockIn = 2;

//this seems to be unused?
const int totalInputs = 30;
int inArr[totalInputs] = {};

//MIDI protocol setup
byte midiCh = 1;
byte note = 60;
//int notes[] {76, 75, 74, 73, 72, 71, 69, 67, 65, 64, 62, 60};
int buttonCState[N_BUTTONS] = {};        // stores the button current value
int buttonPState[N_BUTTONS] = {};        // stores the button previous value

unsigned long Lbuttons = 0;

// debounce
unsigned long lastDebounceTime[N_BUTTONS] = {0};  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    //* the debounce time; increase if the output flickers

void setup() {
   // Setup Serial Monitor
  //MIDI.begin(MIDI_CHANNEL_OMNI);
  //Serial.begin(115200);

  // Setup 74HC165 connections
  pinMode(load, OUTPUT);
  //pinMode(clockEnablePin, OUTPUT);
  pinMode(clockIn, OUTPUT);
  pinMode(dataIn, INPUT);

  //set output pins
  digitalWrite(load, HIGH);
  digitalWrite(clockIn, LOW);
  //digitalWrite(ClockEnablePin, LOW);
  //set inital input array to zero
  for(int i = 0; i < totalInputs; i++){
    inArr[i] = LOW;
  }

  //led setup
  FastLED.addLeds<NEOPIXEL, LED_CONTROL>(leds, LED_COUNT);

}

void loop() {
    
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
            //Serial.println(i);
            leds[i] = CRGB::Blue;
            //MIDI.sendNoteOn(i, 127, 1);
            noteOn(0, note + i, 64);
            MidiUSB.flush();
          }else {
            //buton is released
            Serial.println("button released!");
            leds[i] = CRGB::Black;
            //leds[i].nscale8( 192 );
            //MIDI.sendNoteOff(i, 0, 1);
            noteOff(0, note + i, 64);
            MidiUSB.flush();
          }
          buttonPState[i] = buttonCState[i];
        }
      }
   }
   FastLED.show();
}

//for lvl 1 functionality I'm just making all the buttons into notes

//TODO
//midi note or sample button function

//midi cc button function


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

void controlChange(byte channel, byte control, byte velocity){
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, velocity};
  MidiUSB.sendMIDI(event);
}
