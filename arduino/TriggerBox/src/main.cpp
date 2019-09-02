#include <Arduino.h>

//Pin connected to an led
const int ledPin = 13;

//Recording pulse is high while recording
//_______|---------|______
const int recordingPulsePin = 12;

//clock out is triggering while
//recording pulse is high
//_______|-|_|-|_|-|______
const int clockOutputPin = 11;
//The clock input
//-|_|-|_|-|_|-|_|-|_|-|_
const int clockInputPin = 10;
//Input connected to a switch with two states: on or off
const int switchInputPin = 9;

//Midi channel note of and on messages react to
const int midiListeningChannel = 13;


//The dial pin
const int dialPin =  8;

//Currently recording?
boolean isRecording = false;
//How long ago was the last state change?
elapsedMillis sinceChange;

//Two timers to indicate feedback
IntervalTimer dialTimer;
IntervalTimer blinkTimer;

//Move the dial on the front panel
void moveDial(){
  analogWrite(dialPin, 130);
  delay(3);
  digitalWrite(dialPin, LOW);
}

//Invert the LED state
void blink(){
  digitalWrite(ledPin, !digitalRead(ledPin));
}

//Start a recording: set pins to high and
//change the recording state
void startRecording(boolean sendMidiNoteMessage){
  digitalWrite(recordingPulsePin, HIGH);

  isRecording = true;
  sinceChange = 0;

  //send midi note message when button is changed
  if(sendMidiNoteMessage){
    usbMIDI.sendNoteOn(70,100,midiListeningChannel);
  }

  //move the dail every second
  dialTimer.begin(moveDial, 1000000);

  blinkTimer.end();
  //blink faster than during not recording
  blinkTimer.begin(blink,100000);

  moveDial();

}

//stop the recording: set pinst o low and
//change te recording state
void stopRecording(boolean sendMidiNoteMessage){
  digitalWrite(recordingPulsePin, LOW);

  isRecording = false;
  sinceChange = 0;

  if(sendMidiNoteMessage){
    usbMIDI.sendNoteOff(70,0,midiListeningChannel);
  }

  blinkTimer.end();
  //Blink slowly when recording is stopped
  blinkTimer.begin(blink,1000000);

  dialTimer.end();
}

//When a note on is recieved start the recording:
//the idea is that while a note is 'pressed' the recording takes place
void OnNoteOn(byte channel, byte note, byte velocity) {
  startRecording(false);
}

//When a note off is recieved stop the recording:
//The idea is that while a note is 'pressed' the recording takes place
void OnNoteOff(byte channel, byte note, byte velocity) {
  stopRecording(false);
}

//If we are recording set the clock input pin to
void passClockIfRecording(){
  if(isRecording){
    //pass the clock input to the output
    digitalWrite(clockOutputPin,digitalRead(clockInputPin));
  }else{
    //if not recording make sure the pin
    //is set to low and remains low
    digitalWrite(clockOutputPin,LOW);
  }
}

void changeRecordingState(){
  //debounce switch with 30 ms
  if(sinceChange > 30){
    int newState = digitalRead(switchInputPin);
    if(newState == LOW && !isRecording){
      startRecording(true);
    } else if(newState == HIGH && isRecording){
      stopRecording(true);
    } //else ignore the command we are
    //already in the correct recording state (set by a MIDI command)
  }
}

//Initializes interrupts and pins
void setup() {
  pinMode(recordingPulsePin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(clockOutputPin, OUTPUT);

  //If not connected pull to high.
  pinMode(clockInputPin, INPUT_PULLUP);
  pinMode(switchInputPin, INPUT_PULLUP);

  //An interrupt that is copies the clock input to an output if we are recording
  attachInterrupt(digitalPinToInterrupt(clockInputPin), passClockIfRecording, CHANGE);

  //The switch modifies the recording state
  attachInterrupt(digitalPinToInterrupt(switchInputPin), changeRecordingState, CHANGE);

  //Every two seconds blink the LED
  //switch the led every second
  //It indicates that the device is alive
  blinkTimer.begin(blink,1000000);

  //Handle note on and off events
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
}

//read incoming midi data
void loop() {
  usbMIDI.read(midiListeningChannel);
}
