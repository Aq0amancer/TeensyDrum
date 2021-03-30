#include <MIDI.h>
#include <stdio.h>
#define PADS 6 // How many drum pads?

const int channel = 10; // General MIDI: channel 10 = percussion sounds
const int notes[PADS] = {36,38,42,45,49,50}; // General MIDI: note 38 = acoustic snare

int drums[PADS] = {A0,A1,A2,A3,A4,A5};;
const int thresholdMin = 60; // minimum reading, avoid noise and false starts
const int peakTrackMillis = 12;
const int aftershockMillis = 25; // aftershocks & vibration reject


void setup() {
Serial.begin(115200);
while (!Serial && millis() < 2500) /* wait for serial monitor */ ;
Serial.println("Piezo Peak Capture");
}

void peakDetect(int voltage, int PAD);
void peakDetect(int voltage, int PAD) {
// "static" variables keep their numbers between each run of this function
static int state; // 0=idle, 1=looking for peak, 2=ignore aftershocks
static int peak; // remember the highest reading
static elapsedMillis msec; // timer to end states 1 and 2

switch (state) {
// IDLE state: wait for any reading is above threshold. Do not set
// the threshold too low. You don't want to be too sensitive to slight
// vibration.
case 0:
if (voltage > thresholdMin) {
//Serial.print("begin peak track ");
//Serial.println(voltage);
peak = voltage;
msec = 0;
state = 1;
}
return;

// Peak Tracking state: capture largest reading
case 1:
if (voltage > peak) {
peak = voltage;
}
if (msec >= peakTrackMillis) {
//Serial.print("peak = ");
//Serial.println(peak);
int velocity = map(peak, thresholdMin, 1023, 1, 127);
usbMIDI.sendNoteOn(notes[PAD], velocity, channel);
msec = 0;
state = 2;
}
return;

// Ignore Aftershock state: wait for things to be quiet again.
default:
if (voltage > thresholdMin) {
msec = 0; // keep resetting timer if above threshold
} else if (msec > aftershockMillis) {
usbMIDI.sendNoteOff(notes[PAD], 0, channel);
state = 0; // go back to idle when
}
}
}

void loop() {
    for (int i = 0; i < PADS; i++) {
        int piezo = analogRead(drums[i]);
        peakDetect(piezo,i);
        }
// Add other tasks to loop, but avoid using delay() or waiting.
// You need loop() to keep running rapidly to detect Piezo peaks!

// MIDI Controllers should discard incoming MIDI messages.
// http://forum.pjrc.com/threads/24179-...ses-midi-crash
while (usbMIDI.read()) {
// ignore incoming messages
}
}
