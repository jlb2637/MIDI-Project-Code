#include <MIDI.h> //by Francois Best

const int N_BUTTONS = 12;
const int N_REGISTERS = 2;
int load = 46;
//int clockEnablePin = 50;
int dataIn = 38;
int clockIn = 42;
//need to manually do this for some reason but it should be 8 * # of shift registers in use
const int totalInputs = 16;
int inArr[totalInputs] = {};

//MIDI protocol setup
byte midiCh = 1;
byte note = 60;
int notes[] {76, 75, 74, 73, 72, 71, 69, 67, 65, 64, 62, 60};
int buttonCState[N_BUTTONS] = {};        // stores the button current value
int buttonPState[N_BUTTONS] = {};        // stores the button previous value

MIDI_CREATE_DEFAULT_INSTANCE();

// debounce
unsigned long lastDebounceTime[N_BUTTONS] = {0};  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    //* the debounce time; increase if the output flickers

//extra for one pot
//Potentiometers
const int N_POTS = 1; //* total numbers of pots (slide & rotary)
const int POT_ARDUINO_PIN[N_POTS] = {A0}; //* pins of each pot connected straight to the Arduino

int potCState[N_POTS] = {0}; // Current state of the pot
int potPState[N_POTS] = {0}; // Previous state of the pot
int potVar = 0; // Difference between the current and previous state of the pot

int midiCState[N_POTS] = {0}; // Current state of the midi value
int midiPState[N_POTS] = {0}; // Previous state of the midi value

const int TIMEOUT = 300; //* Amount of time the potentiometer will be read after it exceeds the varThreshold
const int varThreshold = 10; //* Threshold for the potentiometer signal variation
boolean potMoving = true; // If the potentiometer is moving
unsigned long PTime[N_POTS] = {0}; // Previously stored time
unsigned long timer[N_POTS] = {0}; // Stores the time that has elapsed since the timer was reset

byte cc = 1; //lowest midi cc value
int ccA[] = {1};//MUST correspond to # of pots, sets what each pot controls

void setup() {
  // Setup Serial Monitor
  MIDI.begin(1);
  Serial.begin(115200);

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
}

void loop() {
  // Write pulse to load pin
  digitalWrite(load, LOW);
  delayMicroseconds(5);
  digitalWrite(load, HIGH);
  delayMicroseconds(5);

  //shift in 8 * number of shift registers connected
  digitalWrite(clockIn, HIGH);
  //digitalWrite(clockEnablePin, LOW);

  //TODO turn this into a for loop for variabel amt of shift registers
  byte incoming1 = shiftIn(dataIn, clockIn, LSBFIRST);
  byte incoming2 = shiftIn(dataIn, clockIn, LSBFIRST);
  digitalWrite(clockIn, LOW);
  //digitalWrite(clockEnablePin, HIGH);
  //print out the array?
  //Serial.println("reg 1");
 //Serial.println(incoming1, BIN);
  //Serial.println("reg 2");
  //Serial.println(incoming2, BIN);

  //trying to concatenate all bytes together into a single unsigned long
  //for the button loop to be easier to go through
  unsigned long buttons = 0;
  buttons = ((long)incoming2 << 8) + ((long)incoming1);
  Serial.println("concatenated");
  Serial.println(buttons, BIN);

  
  //for testing lets only let this loop run like every 5 seconds?
  delay(5000);

  //so far everything above here went great, adding some new stuff to test below
  
  for(int i = 0; i < N_BUTTONS; i++){
        //set the midi state to be the button state
        //in order to not constantly send MIDI signals
        //we need to check whether or not the button states changed
        //do this by keeping an array of the previoius states and the new states to compare
        int currentBit = bitRead(buttons,i);
        buttonCState[i] = currentBit;
        note = notes[i];
      
          //debouncing, i only kind of understand this lol
          if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonCState[i] == 1) {


            MIDI.sendNoteOn(note, 127, midiCh); // note, velocity, channel
            //Serial.println("sending note on!");
           // Serial.println(i, DEC);
            //Serial.println(currentBit, DEC);
        }
        else {

          // Sends the MIDI note OFF accordingly to the chosen board

          MIDI.sendNoteOn(note, 0, midiCh); // note, velocity, channel
          //Serial.println("sending note off");
        }
          
          //update the past state
          buttonPState[i] = buttonCState[i];
        }
      }
   }


  //one pot mod wheel for a showcase lol
  potentiometers();
}

void potentiometers(){
  
  for (int i = 0; i < N_POTS; i++) { //loop through all pots
      
      potCState[i] = analogRead(POT_ARDUINO_PIN[i]);

      midiCState[i] = map(potCState[i], 0, 1023, 0, 127);// Maps the reading of the potCState to a value usable in midi

      potVar = abs(potCState[i] - potPState[i]);// Calculates the absolute value between the difference between the current and previous state of the pot

      if(potVar > varThreshold) {//opens gate if pot variation is over the threshold
          PTime[i] = millis(); //store the previous time
        }
      timer[i] = millis() - PTime[i]; //resets timer

      if (timer[i] < TIMEOUT) {//if the timer is less than the max allowed time the pot is still moving
        potMoving = true;
      } else {
        potMoving = false;
      }

      if (potMoving ==true){ //if pot is moving send the cc (change control){
        if (midiPState[i] != midiCState[i]){
          cc = ccA[i];
          MIDI.sendControlChange(cc, midiCState[i], midiCh);
        }

        potPState[i] = potCState[i]; // Stores the current reading of the potentiometer to compare with the next
        midiPState[i] = midiCState[i];
      }
    }  
  }
