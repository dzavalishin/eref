/**
 * ----------------------------------------------------------------------------
 * Simple ADXL reading, connected to ICP pin
 * ----------------------------------------------------------------------------
 * 
 * In answer to one of your other questions, here's some simple code which 
 * calculates the acceleration for the ADXL output. I have my accelerometer 
 * setup so that 1us = 0.001g, and the AVR setup so the timer runs at 1MHz 
 * 
**/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "delay.h"

volatile int up = 0x01;
volatile uint16_t duration;
volatile int GForce;
int ZEROVALUE = 3789;

SIGNAL (SIG_OVERFLOW1) //Timer has overflowed - takes 8ms
{
   // Shouldn't happen in the range the ADXL is spitting out
}

SIGNAL (SIG_INPUT_CAPTURE1 ) // Rising edge detected
{
   if(up ==1)
   {
      TCCR1B =  _BV (CS11);   // 1/8 Prescaler, rising edge detected
      up = 0;
      TCNT1 = 0;
   }
   else
   {
      TCCR1B =  _BV (CS11) | _BV (ICES1);   // 1/8 Prescaler, falling edge detected
      up = 1;
      duration = TCNT1;
   }

}

void ioinit (void)
{
   // Timer 1 is setup at 1/8 prescaler, with input capture enabled.
   TCCR1B =  _BV (CS11) | _BV (ICES1) | _BV (ICNC1);   // 1/8 Prescaler, input capture + noise cancelling

   timer_enable_int (_BV (TOIE1) | _BV (TICIE1)); // enable timer 1

   // enable interrupts
   sei ();
}

int main (void)
{
   ioinit();
   PORTB = 0xFF;

   /* loop forever, the interrupts are doing the rest */

   while(1)
   {
      if(duration < 5000)               // Only if the value has changed
      {
         GForce = duration - ZEROVALUE;   // Zero value is the value the ADXL is putting out at 0g
                                 // Ideally this should be set through some sort of calibration sequence
         duration = 5000;            // Using a ADXL202, the acceleration should never read this high,
                                 // so it's a good invalid value to indicate whether the value has changed
      }
   }
   return (0);
}