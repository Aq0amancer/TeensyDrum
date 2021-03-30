/* Midi Piezoelecric Drum Pads
 */
#include <MIDI.h> // Requires Arduino Midi Library
#include <Arduino.h>
#include <SerialFlash.h>
#include <SPI.h>

#define PADS 6 // How many drum pads?
#define CHANNEL 1 // MIDI Channel
#define DEBOUNCE 20// Debounce time (in milli-seconds)

// Connect pads in order starting at A0

//MIDI_CREATE_DEFAULT_INSTANCE();
int sensitivity[PADS] = {100,100,100,100,100}; // Maximum input range
int threshold[PADS] = {20,20,20,20,20}; // Minimu m input range

// PADS 4 = srednji, 
int pad_numbers[PADS] = {21,20,19,18,17,16};
unsigned long timer[PADS];
bool playing[PADS];
int highScore[PADS];
int note[PADS] = {67,69,73,75,82,85}; // Set drum pad notes here

int noteON = 144; //144 = 10010000 in binary, note on command
int noteOFF = 128;

// Define functions
void playNote(int pad, int volume);
void playNote(int pad, int volume) {
  float velocity = ((volume) / float(sensitivity - threshold)) * 127;
  if (velocity > 127) velocity = 127;
  if (velocity > highScore[pad]) highScore[pad] = velocity;
}

void setup() {
  Serial.begin(38400);

  //MIDI.begin(MIDI_CHANNEL_OFF);
  for (int i = 0; i < PADS; i++) {
    playing[i] = false;
    highScore[i] = 0;
    timer[i] = 0;
  }
}

//send MIDI message
void MIDImessage(int command, int MIDInote, int MIDIvelocity);
void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  Serial.write(command);//send note on or note off command 
  Serial.write(MIDInote);//send pitch data
  Serial.write(MIDIvelocity);//send velocity data
}



void loop() {
  for (int i = 0; i < PADS; i++) {
    int x = pad_numbers[i];
    int volume = analogRead(x);
    if (volume >= threshold[i] && playing[i] == false) {
      if (millis() - timer[i] >= DEBOUNCE) {
        playing[i] = true;
        playNote(i, volume);
      }
    }
    else if (volume >= threshold[i] && playing[i] == true) {
      playNote(i, volume);
    }
    else if (volume < threshold[i] && playing[i] == true) {
      //MIDI.sendNoteOn(note[i], highScore[i], CHANNEL);
      MIDImessage(noteON,note[i], 100);
      MIDImessage(noteOFF, note[i], 0);//turn note off
      highScore[i] = 0;
      playing[i] = false;
      timer[i] = millis();
    }
  }
}

