/*
 * IncFile1.h
 *
 * Created: 2014-10-15 14:39:49
 *  Author: mdre0003
 */ 

#define JOYSTICK_BUTTON_DDR DDRD
#define JOYSTICK_BUTTON_PORT PORTD
#define JOYSTICK_BUTTON_PIN PIND
#define JOYSTICK_BUTTON_PIN_NUM PD2 //


void joystick_init()
{
	JOYSTICK_BUTTON_DDR &= ~(1 << JOYSTICK_BUTTON_PIN_NUM);
	JOYSTICK_BUTTON_PORT|= (1 << JOYSTICK_BUTTON_PIN_NUM); 
	

	
	adc_init();
}

void adc_init()
{
	ADMUX = (1<<REFS0); // set AVcc as reference
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // adc enable. prescaler = 128
}

uint16_t joystick_val(uint8_t ch)
{
	ch &= 0b00000111;
	ADMUX = (ADMUX & 0xF8)|ch;
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));

	return (ADC);
}

uint8_t joystick_button_status()
{
	if (bit_is_clear(JOYSTICK_BUTTON_PIN,JOYSTICK_BUTTON_PIN_NUM))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}