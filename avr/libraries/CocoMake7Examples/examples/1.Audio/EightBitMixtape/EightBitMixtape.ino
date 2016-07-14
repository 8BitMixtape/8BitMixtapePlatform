#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/power.h>

#define true 1
#define false 0

/*
 
 
         ##          ##
           ##      ##        
         ##############
       ####  ######  ####
     ######################
     ##  ##############  ##    
     ##  ##          ##  ##
           ####  ####
 
 
*/

volatile unsigned long t; // long
volatile unsigned long u; // long
volatile uint8_t snd; // 0...255

volatile uint8_t pot1 = 100; // 0...255
volatile uint8_t pot2 = 20; // 0...255
volatile uint8_t songs = 0;

void timer_init()
{
    //no prescale
    clock_prescale_set(clock_div_1);

    //PWM SOUND OUTPUT
    TCCR0A |= (1<<WGM00)|(1<<WGM01); //Fast pwm
    //TCCR0A |= (1<<WGM00) ; //Phase correct pwm
    TCCR0A |= (1<<COM0A1); //Clear OC0A/OC0B on Compare Match when up-counting.
    TCCR0B |= (1<<CS00);//no prescale
        
    //TIMER1 SOUND GENERATOR @ 44100hz
    //babygnusb attiny85 clock frequency = 16.5 Mhz
    
    //TIMER SETUP
    TCCR1 |= _BV(CTC1); //clear timer on compare
    TIMSK |= _BV(OCIE1A); //activate compare interruppt
    TCNT1 = 0; //init count

    //TIMER FREQUENCY
    //TCCR1 |= _BV(CS10); // prescale 1
    //TCCR1 |= _BV(CS11); // prescale 2
    TCCR1 |= _BV(CS10)|_BV(CS12); // prescale 16    
    //TCCR1 |= _BV(CS11)|_BV(CS12); // prescale 32
    //TCCR1 |= _BV(CS10)|_BV(CS11)|_BV(CS12); // prescale 64
    //TCCR1 |= _BV(CS13); // prescale 128
    //TCCR1 |= _BV(CS10) | _BV(CS13); // prescale 256    
    
    //SAMPLE RATE
    OCR1C = 120; // (16500000/16)/8000 = 128
    //OCR1C = 45; // (16500000/16)/11025 = 93
    //OCR1C = 22; // (16500000/16)/22050 = 46
    //OCR1C = 23; // (16500000/16)/44100 = 23

    // babygnusb led pin
    DDRB |= (1<<PB0); //pin connected to led
    
}


int main(void)
{
    timer_init();// initialize timer & Pwm	
    sei(); //enable global interrupt
    
    // run forever
    
    while(1)
    {

    }

    return 0;
}

ISR(TIMER1_COMPA_vect)
{
    
    //sound generator pwm out

    switch (songs)
    {
        case 0:
        snd = (pot1*5&t>>7)|(t*pot2&t>>10);
        break;
        case 1:
        snd = (pot1|(t>>9|t>>7))*t&(pot2>>11|t>>9);
        break;
        case 2:
        snd = (pot1*9&t>>4|t*5&t>>7|pot2*3&t/1024)-1;
        break;
        case 3:
        snd = (t>>6|t|t>>(t>>16))*pot1+((t>>11)&pot2);
        break;
        case 4:
        snd = t*(t^t+(t>>15|pot1)^(t-(1280-pot2)^t)>>10);
        break;
        case 5:
        snd = ((t&4096)?((t*(t^t%255)|(t>>(4+pot1)))>>1):(t>>pot2)|((t&8192)?t<<2:t));
        break;
        case 6:
        snd = (t&t>>12)*(t>>(4+pot1)|t>>(8+pot2))^t>>6;
        break;
        case 7:
        snd = t*(((t>>12)|(t>>(8+pot1)))&((63+pot2)&(t>>4)));
        break;                       
    }


    OCR0A = snd;
    t++;
}