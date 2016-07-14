/* Digital Music Box v2

   David Johnson-Davies - www.technoblogy.com - 16th March 2016
   ATtiny85 @ 16 MHz (internal PLL; 4.3 V BOD)
   
   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license: 
   http://creativecommons.org/licenses/by/4.0/
*/

int Scale[] = { 
  680,  721,  764,  809,  857,  908,  962, 1020, 1080, 1144, 1212, 1284, 
 1361, 1442, 1528, 1618, 1715, 1817, 1925, 2039, 2160, 2289, 2425, 2569,
 2722, 2884, 3055, 3237, 3429, 3633, 3849, 4078 };
 
const int Channels = 4;
const int Tempo = 4;      // 4 = 4 beats per second
const int Decay = 9;      // Length of note decay; max 10

volatile unsigned int Acc[Channels];
volatile unsigned int Freq[Channels];
volatile unsigned int Amp[Channels];

// Play Happy Birthday

const uint32_t Tune[] PROGMEM = {
//_*_*__*_*_*__*_*__*_*_*__*_*__*_
//C D EF G A BC D EF G A BC D EF G
0b00000000000000000001000000000000,
0b00000000000000000001000000000000,

0b10000000000000000000010000000000,
0b00000000000000000000000000000000,
0b00001001000000000001000000000000,
0b00000000000000000000000000000000,
0b00000000000000000000000010000000,
0b00000000000000000000000000000000,

0b00100000000000000000000100000000,
0b00000000000000000000000000000000,
0b00000101000000000000000000000000,
0b00000000000000000000000000000000,
0b00000000000000000001000000000000,
0b00000000000000000001000000000000,

0b10000000000000000000010000000000,
0b00000000000000000000000000000000,
0b00000101000000000001000000000000,
0b00000000000000000000000000000000,
0b00000000000000000000000000100000,
0b00000000000000000000000000000000,

0b10000000000000000000000010000000,
0b00000000000000000000000000000000,
0b00001001000000000000000000000000,
0b00000000000000000000000000000000,
0b00000000000000000001000000000000,
0b00000000000000000001000000000000,

0b10000000000000000000000000000001,
0b00000000000000000000000000000000,
0b00100000000000000000000000001000,
0b00000000000000000000000000000000,
0b00001000000000000000000010000000,
0b00000000000000000000000000000000,

0b00000100000000000000000100000000,
0b00000000000000000000000000000000,
0b00000000000000000000010000000000,
0b00000000000000000000000000000000,
0b00000000000000000000000000000100,
0b00000000000000000000000000000100,

0b00000001000000000000000000001000,
0b00000000000000000000000000000000,
0b00000000000000000000000010000000,
0b00000000000000000000000000000000,
0b00000101000000000000000000100000,
0b00000000000000000000000000000000,

0b10001001000000000000000010000000,
0xFF}; // End of tune

//Globals persist throughout tune
int TunePtr = 0, Chan = 0;

// Watchdog interrupt plays notes
ISR(WDT_vect) {
  sei();   // Allow interrupts
  WDTCR |= 1<<WDIE;
  unsigned long Chord = pgm_read_dword(&Tune[TunePtr]);
  if (Chord == 0xFF) return;
  TunePtr++;
  // Read the bits in Chord
  for (int Note = 0; Note < 32; Note++) {
    if ((Chord & 0x80000000) != 0) {
      Freq[Chan] = Scale[Note];
      Amp[Chan] = 1<<(Decay+5);
      Chan = (Chan + 1) % Channels;
    }
    Chord = Chord<<1;
  }
}

// Generate square waves on 4 channels
ISR(TIMER0_COMPA_vect) {
  signed char Temp, Mask, Env, Note, Sum=0;
  for (int c = 0; c < Channels; c++) {
    Acc[c] = Acc[c] + Freq[c];  
    Amp[c] = Amp[c] - (Amp[c] != 0);
    Temp = Acc[c] >> 8;
    Mask = Temp >> 7;
    Env = Amp[c] >> Decay;
    Note = (Env ^ Mask) + (Mask & 1);
    Sum = Sum + Note;
  }
  OCR1B = Sum + 128;
}

void setup() {
  // Enable 64 MHz PLL and use as source for Timer1
  PLLCSR = 1<<PCKE | 1<<PLLE;     

  // Set up Timer/Counter1 for PWM output
  TIMSK = 0;                     // Timer interrupts OFF
  TCCR1 = 1<<CS10;               // 1:1 prescale
  GTCCR = 1<<PWM1B | 2<<COM1B0;  // PWM B, clear on match

  OCR1B = 128;
  DDRB = 1<<DDB4;                // Enable PWM output on pin 4

  // Set up Timer/Counter0 for 20kHz interrupt to output samples.
  TCCR0A = 3<<WGM00;             // Fast PWM
  TCCR0B = 1<<WGM02 | 2<<CS00;   // 1/8 prescale
  OCR0A = 99;                    // Divide by 100
  TIMSK = 1<<OCIE0A;             // Enable compare match, disable overflow
 
  // Set up Watchdog timer for 4 Hz interrupt for note output.
  WDTCR = 1<<WDIE | Tempo<<WDP0; // 4 Hz interrupt
}

void loop() {
}
