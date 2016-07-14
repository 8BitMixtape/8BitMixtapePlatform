/*
    Copyright (C) 2014 <>< Charles Lohr


    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "ntscfont.h"

 uint8_t tmrt = 0;

void delay_ms(uint32_t time) {
  uint32_t i;
  for (i = 0; i < time; i++) {
    _delay_ms(1);
  }
}

#define NOOP asm volatile("nop" ::)

void NumToText( char * c, uint8_t a )
{
    c[0] = (a/100)+'0';
    c[1] = ((a/10)%10)+'0';
    c[2] = (a%10)+'0';
    c[3] = 0;
}
void NumToText4( char * c, uint16_t a )
{
    c[0] = (a/1000)+'0';
    c[1] = ((a/100)%10)+'0';
    c[2] = ((a/10)%10)+'0';
    c[3] = (a%10)+'0';
    c[4] = 0;
}


EMPTY_INTERRUPT(TIM0_OVF_vect );


int main( )
{
    cli();

    CLKPR = 0x80;    /*Setup CLKPCE to be receptive*/
    CLKPR = 0x00;    /*No scalar*/

    PLLCSR = _BV(PLLE) | _BV( PCKE );
//    PLLCSR |= _BV(LSM);

    DDRB = _BV(1);

    DDRB |= _BV(3);
    DDRB |= _BV(4);
    PORTB |= _BV(1);

    TCCR1 = _BV(CS10);// | _BV(CTC1); //Clear on trigger.
    GTCCR |= _BV(PWM1B) |  _BV(COM1B0);// | _BV(COM1B1);
    OCR1B = 2;
    OCR1C = 3;
    DTPS1 = 0;
    DT1B = _BV(0) | _BV(4);

    TCCR0A = 0;
    TCCR0B = _BV(CS01);
    TIMSK |= _BV(TOIE0);

//OSCCAL = 376 ;

//    OSCCAL=186;


#define POWERSET

#ifdef POWERSET
#define NTSC_HI  {    DDRB=0;}
#define NTSC_LOW {    DDRB=_BV(4); }
#define NTSC_VH  {    DDRB=_BV(3); }
#elif defined( POWERSET2 )
#define NTSC_VH  {    DDRB=0;}
#define NTSC_LOW {    DDRB=_BV(4)|_BV(3); }
#define NTSC_HI  {    DDRB=_BV(3); }
#elif defined( POWERSET3 )
#define NTSC_VH  {    DDRB=0; }
#define NTSC_HI   { DDRB=_BV(3); }
#define NTSC_LOW   {    DDRB=_BV(4)|_BV(3); }
#else

//Experimental mechanisms for changing power. Don't work.
#define NTSC_VH  {    OCR1C = 3; TCNT1 = 0; }
#define NTSC_HI  {    OCR1C = 6; TCNT1 = 0;}
#define NTSC_LOW {    OCR1C = 0; TCNT1 = 0;}

#endif
    uint8_t line, i;

    #define TIMEOFFSET .12 //.12
    #define CLKOFS .12

    uint8_t frame = 0, k, ctll;
    char stdsr[8*13];
    sprintf( stdsr, "Fr: " );
    sprintf( stdsr+8,  "HalfByte" );
    sprintf( stdsr+16, "  Blog  " );
    sprintf( stdsr+24, "        " );
    sprintf( stdsr+32, " Trinket" );
    sprintf( stdsr+40, "AdaFruit" );
    sprintf( stdsr+48, "        " );
    sprintf( stdsr+56, "        " );
    sprintf( stdsr+64, "        " );
    sprintf( stdsr+72, "        " );
    sprintf( stdsr+80, "        " );
    sprintf( stdsr+88, "        " );


    ADMUX =/* _BV(REFS1)  |  _BV(ADLAR) | */ 1; //1 = PB2
    ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADPS2) | _BV(ADPS1);

#define LINETIME 21 //Linetime of 7..20 is barely valid. So, 
//#define WAITTCNT while(TCNT0);
#define RESETCNT {TCNT0 = LINETIME; TIFR|=_BV(TOV0); GTCCR|=PSR0;sei();}

//#define WAITTCNT while(!(TIFR&_BV(TOV0)));
#define WAITTCNT sleep_cpu();

//#define WAITTCNT fintcnt();

     sleep_enable();
    sei();

    uint16_t ovax; //0..1024 = 0...5v
    uint8_t  msd;
    uint8_t  lsd;

    while(1)
    {
//        OSCCAL = 750;
       // OSCCAL = 494;
        OSCCAL = ADC;

//if (tmrt > 1)
//{
// OSCCAL++;
// tmrt = 0; 
//}
//
//tmrt++;

        frame++;
        //H = 1./15734.264 = 63.555 / 2 = 31.7775
        for( line = 0; line < 6; line++ )
        { NTSC_LOW; _delay_us(2.3-TIMEOFFSET); NTSC_HI; _delay_us(29.5-TIMEOFFSET-CLKOFS); }
        for( line = 0; line < 6; line++ )
        { NTSC_LOW; _delay_us(27.1-TIMEOFFSET); NTSC_HI; _delay_us(4.7-TIMEOFFSET-CLKOFS); }
        for( line = 0; line < 6; line++ )
        { NTSC_LOW; _delay_us(2.3-TIMEOFFSET); NTSC_HI; _delay_us(29.5-TIMEOFFSET-CLKOFS); }

        for( line = 0; line < 39; line++ )
        {
            RESETCNT;
            NTSC_LOW;
            _delay_us(4.7-TIMEOFFSET);
            NTSC_HI;

            //Do whatever you want.
            //sprintf( stdsr, "%d", frame );
            switch (line)
            {
            case 0:
                NumToText( stdsr+4, frame );
                break;
            case 1:
                ovax = ADC;
                ovax = ovax * 49 + (ovax>>1);
                ovax/=10;
                break;
            case 2:
                NumToText( stdsr+24, ovax/1000 );
                stdsr[27] = '.';
                break;
            case 5:
                NumToText4( stdsr+27, ovax );
                stdsr[27] = '.';

                break;
            }

            WAITTCNT;
            //_delay_us(58.8-TIMEOFFSET-CLKOFS);
        }

        for( line = 0; line < 2; line++ )
        {
            RESETCNT;
            NTSC_LOW;
            _delay_us(4.7-TIMEOFFSET);
            NTSC_HI;
            WAITTCNT;
        }

        for( line = 0; line < 220; line++ )
        {
            RESETCNT;
            NTSC_LOW; _delay_us(4.7-TIMEOFFSET); 
            NTSC_HI; _delay_us(8-TIMEOFFSET-CLKOFS);

//#define LINETEST
#ifdef LINETEST
            NTSC_VH; _delay_us(8-TIMEOFFSET-CLKOFS);
            NTSC_HI; _delay_us(44.5);
#else
            ctll = line>>2;
     
            for( k = 0; k < 8; k++ )
            {
      // draw the character, one pixel at a time
            uint8_t ch = pgm_read_byte( &font_8x8_data[(stdsr[k+((ctll>>3)<<3)]<<3)] + (ctll&0x07) );
            for( i = 0; i < 8; i++ )
            {
                if( (ch&1) ) //if pixel is dark...
                {
                    NTSC_VH;
                }
                else
                {
                    NTSC_HI;  // pixel is lit
                    NOOP;
                }
                ch>>=1;
                NOOP; NOOP; NOOP; NOOP;
            }
                    NTSC_HI;

            }

            NTSC_HI; //_delay_us(4.7-TIMEOFFSET-CLKOFS);
            WAITTCNT;
#endif

//        NTSC_HI; _delay_us(46-TIMEOFFSET-CLKOFS);

//            NTSC_VH; _delay_us(32-TIMEOFFSET-CLKOFS);
//            NTSC_HI; _delay_us(19.8-TIMEOFFSET-CLKOFS);
        }
    }
    
    return 0;
} 
