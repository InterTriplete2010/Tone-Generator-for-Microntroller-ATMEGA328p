#ifndef F_CPU
#define F_CPU 16000000UL	//16 MHz; this is the system clock of ATMEGA328p
#endif

#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>

int pre_sacalar = 1;	//Pre-scalar for the F_CPU. It works best when the pre-scaler is set to 1;
int freq = 1000;		//Frequency of the tone
unsigned long duration = 500;	//Duration of the tone
long toggle_count;	//Keeps track of the duration of the tone
const int ISI = 1000;	//Inter-Stimulus-Interval expressed in ms;
int rep_stim = 3;	//Sweeps
int rep_stim_reset = rep_stim;

//Setup the timer0
void setup_timer() 
{

	// OCR1A is assigned the value to have a specific frequency
	OCR1A = (F_CPU/(2*pre_sacalar*freq)) - 1;

	TCCR1A = (1 << WGM12) | (1 << COM1A0); // Set on compare match and the TOP is OCR1A
	TCCR1B = (1 << WGM12) | (1 << CS10); // 1 is the prescaler and the TOP is OCR1A

}


// ISR for timer compare match
ISR(TIMER1_COMPA_vect)
{

if (toggle_count != 0)
{

	PORTD ^= (1 << PORTD7);
	toggle_count--;
	
}

if (toggle_count == 0)
{

	PORTD |= (1 << PORTD6);	//Turn the LED on
	_delay_ms(ISI);
	PORTD &= ~(1 << PORTD6);	//Turn the LED off
	rep_stim--;
	//Reset the toggle_count
	toggle_count = 2*freq * duration / 1000;
	
}

if (rep_stim == 0)
{

	PORTD &= ~(1 << PORTD7);
	PORTB |= (1 << PORTB0);
	TIMSK1 = 0x00;
	
	while((PIND & (1 << PIND5)));
	
	TIMSK1 = 0x02;	//Enable the interrupt
	PORTB &= ~(1 << PORTB0);
	rep_stim = rep_stim_reset;
		
}
	
}


int main(void)
{

//Force the stimulus to be at least 0.1 ms
if (duration > 0)
{

	toggle_count = 2*freq * duration / 1000;

}

else
{
	
	duration = 0.1;
	toggle_count = 2*freq * duration / 1000;
	
}

	DDRD |= (1 << DDD7) | (1 << DDD6) | (0 << DDD5);	//Ports used for the tone and for the LED + PD5 used as a reset button	
	PORTD |= (1 << PORTD5);
	DDRB |= (1 << DDB0);
	
	setup_timer();	
	TIMSK1 = 0x02;	//Enable the interrupt
	sei();
	
	while (1)
	{
		
		
	}

}

}
