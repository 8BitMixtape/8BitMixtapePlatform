/*---------------------------------------------------------------------------------------------

  Gnusbuino MIDI Library 2012 by Michael Egger

  SEND NOTES EXAMPLE
  Send a chord of three notes to the host over MIDI

  This example code is in the public domain.


  --------------------------------------------------------------------------------------------- */

#include <CocoMidi.h>        // you MUST include the CocoMidi library
int note = 0;

unsigned long lastTime;            // variable to store the last time we sent a chord

void setup() {
  CocoMidi.init();
}


// the loop routine runs over and over again forever:
void loop() {
  note = random(12, 96);
  CocoMidi.send(MIDI_NOTEON, note, 127);
  CocoMidi.delay(50);
  CocoMidi.send(MIDI_NOTEOFF, note, 127);
  CocoMidi.delay(50);
  CocoMidi.update();

}
