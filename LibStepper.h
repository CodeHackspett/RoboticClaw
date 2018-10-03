/*
 * LibStepper.h
 *
 * Created: 2014-10-17 09:27:03
 *  Author: mdre0003
 */ 


#define  SM_STEP_DDR DDRB
#define  SM_STEP_PORT PORTB
#define  SM_STEP_PIN_NUM PB1

#define  SM_DIR_DDR DDRB
#define  SM_DIR_PORT PORTB
#define  SM_DIR_PIN_NUM PB2

#define  STEP_CW 0
#define  STEP_CCW 1

void stepper_init()
{
	SM_STEP_DDR |= (1<<SM_STEP_PIN_NUM);
	SM_DIR_DDR |= (1<<SM_DIR_PIN_NUM);
	SM_STEP_PORT &= ~(1<<SM_STEP_PIN_NUM);
	SM_DIR_PORT &= ~(1<<SM_DIR_PIN_NUM);
}

void send_step_pulse()
{
	SM_STEP_PORT |= (1<<SM_STEP_PIN_NUM);
	_delay_ms(2);
	SM_STEP_PORT &= ~(1<<SM_STEP_PIN_NUM);
	_delay_ms(2);
}


void rotate_stepper(int steps, int8_t direction)
{
	if (direction == STEP_CW)
	{
		SM_DIR_PORT &= ~(1<<SM_DIR_PIN_NUM);
	}
	
	else
	{
		SM_DIR_PORT |= (1<<SM_DIR_PIN_NUM);
	}
	
	for (int i = 0; i < ((steps* 8)+1); i++)
	{
		send_step_pulse();
	}
}