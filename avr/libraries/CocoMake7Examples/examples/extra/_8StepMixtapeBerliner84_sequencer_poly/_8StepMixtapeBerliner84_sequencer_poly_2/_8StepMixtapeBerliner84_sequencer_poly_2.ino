#include <TeenySynth.h>

#include <util/delay.h>
TeenySynth synth;

const byte pot = A3, tonePin = 6, clock = 6, gatePin = 6, controlVoltagePin = 5;
const int ledPinMapping[16] = {9, 10, 8, 7, 0, 1, 2, 4};
const int MAX_NOTE_LENGTH = 32000, MAX_FREQ = 255, NUMBER_OF_STEPS = 8, POT_THRESHOLD = 35;
static uint16_t toSecondsCounter  = 0;
static uint16_t timeCalibrationCounter  = 0;
unsigned long lastTime = 0;            // variable to store the last time we sent a chord
int stepFreqs[] = {100,100,100,100,100,100,100,100};


int melody[] = {
    NOTE_D4, 0, NOTE_F4, NOTE_D4, 0, NOTE_D4, NOTE_G4, NOTE_D4, NOTE_C4,
    NOTE_D4, 0, NOTE_A4, NOTE_D4, 0, NOTE_D4, NOTE_AS4, NOTE_A4, NOTE_F4,
    NOTE_D4, NOTE_A4, NOTE_D5, NOTE_D4, NOTE_C4, 0, NOTE_C4, NOTE_A3, NOTE_E4, NOTE_D4,
    0
};

uint8_t a = 0;

void setFrequency()
{        
      unsigned long m = synth.millis();
      unsigned long lasm = 0;

      for(byte a=0; a<NUMBER_OF_STEPS; a++){

      while((analogRead(pot)>POT_THRESHOLD)){

     int freq = (analogRead(pot)-POT_THRESHOLD) >> 2;
     for (int i = 0; i <= freq>>5; i++){
       digitalWrite(ledPinMapping[i], HIGH);
              digitalWrite(ledPinMapping[a], LOW);

       _delay_us(1);
       
              digitalWrite(ledPinMapping[a], HIGH);

     };
     
      for (int i = (freq>>5)+1; i < NUMBER_OF_STEPS; i++){
       digitalWrite(ledPinMapping[i], LOW); 
      }
      
          unsigned long m = synth.millis();
          
     
           if (m-lasm >= 500 ){
           lasm = synth.millis(); // cant use millis from arduino - iox
           synth.mTrigger(0,freq);
           synth.mTrigger(1,stepFreqs[a]+20);
           
                      stepFreqs[a] = freq;

           }


      }

        _delay_us(1000);

     } 
}

void setup() {
    pinMode(pot, INPUT);
  digitalWrite(pot, HIGH);
  pinMode(gatePin, OUTPUT);
  digitalWrite(gatePin, LOW);
    analogWrite(controlVoltagePin, 255);

    for (int i = 0; i < NUMBER_OF_STEPS; i++){
    pinMode(ledPinMapping[i], OUTPUT);
  };
  // put your setup code here, to run once:
  for (int i = 0; i <= NUMBER_OF_STEPS; i++){
    digitalWrite(ledPinMapping[i], HIGH);
    _delay_ms(30);
  };
  
  for (int i = 0; i <= NUMBER_OF_STEPS; i++){
    digitalWrite(ledPinMapping[i], LOW);
    _delay_ms(30);
  };
  
    synth.begin();
    synth.setupVoice(0,NOISE,60,ENVELOPE1,40,64);
    synth.setupVoice(1,SQUARE,62,ENVELOPE0,70,64);
    synth.setupVoice(2,RAMP,64,ENVELOPE2,110,64);
    synth.setupVoice(3,NOISE,67,ENVELOPE0,60,64);
        unsigned long m = synth.millis();

      for (int i = 0; i < 28; i++){
lastTime = synth.millis(); // cant use millis from arduino - iox
synth.mTrigger(0,melody[i]+50);
synth.mTrigger(1,melody[i]+30);
_delay_us(125);
      }
      
    setFrequency();

}

void loop(){
        unsigned long m = synth.millis();
    
      
    if (m-lastTime >= analogRead(A3) ){
      int freq = stepFreqs[a%NUMBER_OF_STEPS];
      
           for (int i = 0; i <= a; i++){
       digitalWrite(ledPinMapping[i], HIGH);
     };
     
      for (int i = a; i < NUMBER_OF_STEPS; i++){
       digitalWrite(ledPinMapping[i], LOW); 
      }
      
      
           lastTime = synth.millis(); // cant use millis from arduino - iox
           synth.mTrigger(0,stepFreqs[a]);
           synth.mTrigger(1,stepFreqs[a]+20);
           
           a++;
           
           if (a>(NUMBER_OF_STEPS-1)) a = 0;
    }

}
