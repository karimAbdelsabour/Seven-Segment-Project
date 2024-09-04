#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "BIT_MATH.h"
#include "STD_Types.h"
#include "GPIO_interface.h"
#include "LED_interface.h"
#include "SevenSegment_interface.h"

volatile unsigned char reset_requested = 0;
volatile unsigned char button_state = 0; 

ISR(INT0_vect)
{
	reset_requested = 1;
}

int main(void)
{
	// Initialize GPIO and 7-segment display
	DDRC = 0b11111100;
	PORTC = 0;
	
	DDRD &= ~(1 << PD2);
	PORTD |= (1 << PD2);
	MCUCR |= (1 << ISC01);
	GICR |= (1 << INT0);
	sei();
	
	SevenSegment_t S1;
	S1.Type = COMMON_ANODE;
	
	for (int i = 0; i < 8; i++)
	{
		S1.Segments[i].port = GPIO_PortC;
		S1.Segments[i].pin = i;
	}
	
	HSevenSegment_voidInit(&S1);
	
	u8 count = 0;
	
	while (1)
	{
		
		if (reset_requested)
		{
			reset_requested = 0;
			count = 0;
			HSevenSegment_voidDisplayDigit(&S1, count);
		}
		 
		else if (GET_BIT(PIND, 0) == 1)
		{
			_delay_ms(10);
			if (GET_BIT(PIND, 0) == 1)
			{
				count++;
				count %= 10;
				HSevenSegment_voidDisplayDigit(&S1, count);
				_delay_ms(500);
			}
		}
		else if (GET_BIT(PIND, 1) == 1)
		{
			_delay_ms(10);
			if (GET_BIT(PIND, 1) == 1)
			{
				// Decrement count and display on 7-segment
				if (count == 0)
				count = 9;
				else
				count--;
				HSevenSegment_voidDisplayDigit(&S1, count);
				_delay_ms(500);
			}
		
		}
		
	}
	
	return 0;
}