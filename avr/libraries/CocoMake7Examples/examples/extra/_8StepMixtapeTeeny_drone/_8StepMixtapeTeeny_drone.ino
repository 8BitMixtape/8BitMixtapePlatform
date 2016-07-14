#include <TeenySynth.h>
#include <util/delay.h>

TeenySynth synth;

#define POT_PIN A3
#define TONE_PIN 6
#define CLOCK_PIN 6
#define GATE_PIN 6
#define NUMBER_OF_STEPS 8
#define POT_THRESHOLD 35
#define INTRO_SONG 1
#define CV_PIN 5

const byte pot = A3, tonePin = 6, clock = 6, gatePin = 6;
const int MAX_NOTE_LENGTH = 32000, MAX_FREQ = 255;
const uint8_t ledPinMapping[8] = {9, 10, 8, 7, 0, 1, 2, 4};
int stepFreqs[] = {100,100,100,100,100,100,100,100};

int melody[] =
{
    NOTE_D4, 0, NOTE_F4, NOTE_D4, 0, NOTE_D4, NOTE_G4, NOTE_D4, NOTE_C4,
    NOTE_D4, 0, NOTE_A4, NOTE_D4, 0, NOTE_D4, NOTE_AS4, NOTE_A4, NOTE_F4,
    NOTE_D4, NOTE_A4, NOTE_D5, NOTE_D4, NOTE_C4, 0, NOTE_C4, NOTE_A3, NOTE_E4, NOTE_D4,
    0
};

uint8_t a = 0;
unsigned long lastTime = 0; // variable to store the last time we sent a chord

static inline void soundTrigger(uint8_t a)
{
    synth.mTrigger(0,stepFreqs[a]+60);
    synth.mTrigger(1,stepFreqs[a]+30);
}

void setFrequency()
{
    unsigned long m = synth.millis();
    unsigned long lasm = 0;

    for(byte a=0; a<NUMBER_OF_STEPS; a++)
        {

            while((analogRead(pot)>POT_THRESHOLD))
                {

                    int freq = (analogRead(pot)-POT_THRESHOLD) >> 2;

                    for (int i = 0; i <= freq>>5; i++)
                        {
                            digitalWrite(ledPinMapping[i], HIGH);
                            digitalWrite(ledPinMapping[a], LOW);
                            _delay_us(1);
                            digitalWrite(ledPinMapping[a], HIGH);

                        };

                    for (int i = (freq>>5)+1; i < NUMBER_OF_STEPS; i++)
                        {
                            digitalWrite(ledPinMapping[i], LOW);
                        }

                    unsigned long m = synth.millis();

                    stepFreqs[a] = freq >> 1;


                    if (m-lasm >= 500 )
                        {
                            lasm = synth.millis(); // cant use millis from arduino - iox
                            soundTrigger(a);
                        }


                }

            _delay_ms(100);

        }
}

void setup()
{
    pinMode(POT_PIN, INPUT);
    digitalWrite(POT_PIN, HIGH);
    pinMode(GATE_PIN, OUTPUT);
    digitalWrite(GATE_PIN, LOW);
    analogWrite(CV_PIN, 255);

    for (uint8_t i = 0; i < NUMBER_OF_STEPS; i++)
        {
            pinMode(ledPinMapping[i], OUTPUT);
        };
    // put your setup code here, to run once:
    for (uint8_t i = 0; i <= NUMBER_OF_STEPS; i++)
        {
            digitalWrite(ledPinMapping[i], HIGH);
            _delay_ms(30);
        };

    for (uint8_t i = 0; i <= NUMBER_OF_STEPS; i++)
        {
            digitalWrite(ledPinMapping[i], LOW);
            _delay_ms(30);
        };

    synth.begin();
    synth.setupVoice(0,NOISE,70,ENVELOPE1,80,64);
    synth.setupVoice(1,SQUARE,62,ENVELOPE0,100,64);
    synth.setupVoice(2,NOISE,64,ENVELOPE2,110,64);
    synth.setupVoice(3,SQUARE,67,ENVELOPE0,110,64);
    synth.bendTimer(255); //drone echo effect - tweak me!

#if INTRO_SONG == 1
    for (int i = 0; i < 28; i++)
        {
            synth.mTrigger(0,melody[i]+50);
            synth.mTrigger(1,melody[i]+30);
            _delay_us(125);
        }
#endif

    setFrequency();
}


void delayHack(int a)
{
    for (int i = 0 ; i < a; i++)
    {
        _delay_us(1);
    }
}

void loop()
{

    unsigned long m = synth.millis();


    if (m-lastTime >= analogRead(A3) )
        {
          int freq = stepFreqs[a%NUMBER_OF_STEPS];

          for (int i = 0; i <= a; i++)
              {
                  digitalWrite(ledPinMapping[i], HIGH);
              };

          for (int i = a; i < NUMBER_OF_STEPS; i++)
              {
                  digitalWrite(ledPinMapping[i], LOW);
              }


            lastTime = synth.millis(); // cant use millis from arduino - iox
          soundTrigger(a);

          a++;

          if (a>(NUMBER_OF_STEPS-1)) a = 0;
        }


           // delayHack((analogRead(A3)-POT_THRESHOLD));
}
