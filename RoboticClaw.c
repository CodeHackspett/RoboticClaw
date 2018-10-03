/*
 * RoboticClaw.c
 *
 * Created: 2014-10-15 10:07:00
 *  Author: Reaz, Mattia and Dawit
 */ 

/************************************* Include Files *************************************/
#define F_CPU 8000000U

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "i2cmaster.h"
#include "PWMdriver.h"
#include "LibJoyStick.h"
#include "LibStepper.h"
#include "UserInterface.h"
/*****************************************************************************************/

/*************************************** Constants ***************************************/

//User interface I2C bus address
#define SERVO_TILT_OUT 0
#define SERVO_GRIP_OUT 3
#define EEPROM_ADD_TILT 0
#define EEPROM_ADD_GRIP 1

#define GRIP_INDICATOR PD6
#define YAW_INDICATOR PD7


#define USER_BUTTON_DDR DDRB
#define USER_BUTTON_PORT PORTB
#define USER_BUTTON_PIN PINB
#define MODE_MANUAL_PIN PB6
#define MODE_AUTO_PIN PB7
#define MODE_AUTO_LEVEL_PIN PB0
#define MODE_COOPERATION_PIN PB3

#define MANUAL 0
#define AUTOMATIC 1
#define AUTO_LEVEL 2
#define COOPERATION 3


/*****************************************************************************************/
#define SETBIT(ADDRESS,BIT)		(ADDRESS |= (1<<BIT))
#define CLEARBIT(ADDRESS,BIT)	(ADDRESS &= ~(1<<BIT))


/*********************************** Global Variables ************************************/

volatile int prev_tilt = 330; //to be used with eeprom
volatile int prev_grip = 290; //to be used with eeprom
volatile int grip_select = 0; 
volatile int cur_tilt_pwm = 330;
volatile int cur_grip_pwm = 290;
/*****************************************************************************************/



/************************************ Main Function **************************************/
/* After making all the necessary initial configurations by calling the initialize 
 * function, the program displays the Main UI to the user.
*/
int main(void)
{
    initialize();
	//_delay_ms(5000);
	while(1) //joystick_value != 505
	{
		int mode = get_mode();
		if(mode == MANUAL)
		{
			LCD_clearScreen();
			LCD_goToPosition(1);
			LCD_printString("Manual");
			_delay_ms(10);
			manual_mode();
		}
		
		else if(mode ==  AUTOMATIC)
		{
			LCD_clearScreen();
			LCD_goToPosition(1);
			LCD_printString("Automatic");
			_delay_ms(10);
			automatic_mode(180, 135, 255);
		}
		
		//else if(mode == AUTO_LEVEL)
		//{
			//;
		//}
		//
		//else(mode == COOPERATION)
		//{
			//;
		//}
	}	
}

/*****************************************************************************************/


/******************************* Initialization Functions ********************************/


/*
 * Function Name: initialize
 * Access: public
 * Params: none
 * Returns: void
 * Description: It initializes all the processors so that the core functions will work. 
 *				This function in turn calls four other functions which initialize the 
 *				the i2c communication, UI, IR and motor modules respectively. 
 */
void initialize()
{
	DDRD |= (1<<GRIP_INDICATOR)|(1<<YAW_INDICATOR);
	CLEARBIT(PORTD, GRIP_INDICATOR);
	SETBIT(PORTD, YAW_INDICATOR);
	
	i2c_init();
	_delay_ms(1000);
	lcd_init();
	PWMdriver_init();
	stepper_init();
	joystick_init();
	user_input_button_init();
	
	servo_setPos(SERVO_TILT_OUT, cur_tilt_pwm, SERVO_TILT_MIN, SERVO_TILT_MAX);	// test
	servo_setPos(SERVO_GRIP_OUT, SERVO_GRIP_MAX, SERVO_GRIP_MIN, SERVO_GRIP_MAX);	// test
}

void lcd_init()
{
	LCD_clearScreen();
	LCD_setBacklight(TRUE);	
	LCD_setCursor(HIDE);
	_delay_ms(50);
}

void user_input_button_init()
{
	USER_BUTTON_DDR &= ~((1<<MODE_MANUAL_PIN)|(1<<MODE_AUTO_PIN)|(1<<MODE_AUTO_LEVEL_PIN)|(1<<MODE_COOPERATION_PIN)); // set as input
	USER_BUTTON_PORT |= (1<<MODE_MANUAL_PIN)|(1<<MODE_AUTO_PIN)|(1<<MODE_AUTO_LEVEL_PIN)|(1<<MODE_COOPERATION_PIN); // enable pull up
}

int get_mode()
{
	if (bit_is_clear(USER_BUTTON_PIN, MODE_AUTO_LEVEL_PIN))
	{
		return AUTO_LEVEL;
	}
	
	else if (bit_is_clear(USER_BUTTON_PIN, MODE_COOPERATION_PIN))
	{
		return COOPERATION;
	}
	
	else if (bit_is_clear(USER_BUTTON_PIN, MODE_AUTO_PIN))
	{
		return AUTOMATIC;
	}
	else 
	{
		return MANUAL;
	}
}

/*****************************************************************************************/


/******************************* User Interface Functions ********************************/


/*****************************************************************************************/


/************************************ Core Functions *************************************/



void manual_mode()
{
	int tilt_value = joystick_val(1);
	int yaw_grip_value = joystick_val(0);
	if (!( 510 >= yaw_grip_value && yaw_grip_value >= 500))
	{
		if(grip_select)
		{
			//manual_grip(grip_duty_cycle(yaw_grip_value));
			manual_grip(yaw_grip_value);
		}
		else
		{
			manual_yaw(yaw_grip_value);
		}
	}
	if (!( 510 >= tilt_value && tilt_value >= 500))
	{
		//manual_tilt(tilt_duty_cycle(tilt_value));
		
		if(!grip_select)
		{
			manual_tilt(tilt_value);
		}
	}
	yaw_grip_selection();
}


void manual_tilt(int value)
{	
	if (value > 510)
	{
		if (cur_tilt_pwm < SERVO_TILT_MAX)
		{
			//cur_tilt_pwm++;
			cur_tilt_pwm += 4;
		}
		servo_setPos(SERVO_TILT_OUT, cur_tilt_pwm, SERVO_TILT_MIN, SERVO_TILT_MAX);
	}
	else if (value < 500)
	{
		if (cur_tilt_pwm > SERVO_TILT_MIN)
		{
			//cur_tilt_pwm--;
			cur_tilt_pwm -= 4;
		}
		servo_setPos(SERVO_TILT_OUT, cur_tilt_pwm, SERVO_TILT_MIN, SERVO_TILT_MAX);
	}
	_delay_ms(50);
	//prev_tilt = value;
}



void manual_grip(int value)
{
	if (value > 520)
	{
		if (cur_grip_pwm < SERVO_GRIP_MAX)
		{
			//cur_grip_pwm++;
			cur_grip_pwm += 2;
		}
		servo_setPos(SERVO_GRIP_OUT, cur_grip_pwm, SERVO_GRIP_MIN, SERVO_GRIP_MAX);
	}
	else if (value < 490)
	{
		if (cur_grip_pwm > SERVO_GRIP_MIN)
		{
			//cur_grip_pwm--;
			cur_grip_pwm -= 2;
		}
		servo_setPos(SERVO_GRIP_OUT, cur_grip_pwm, SERVO_GRIP_MIN, SERVO_GRIP_MAX);
	}
	_delay_ms(20);
	//prev_grip = value;
}

void manual_yaw( int value)
{
	if (value > 510)
	{
		rotate_stepper(1, STEP_CW);
	}
	else if (value < 500)
	{
		rotate_stepper(1, STEP_CCW);
	}	
	_delay_ms(10);
}

void yaw_grip_selection()
{
	if(joystick_button_status())
	{
		if(grip_select == 1)
		{
			grip_select = 0;
			SETBIT(PORTD, YAW_INDICATOR);
			CLEARBIT(PORTD, GRIP_INDICATOR);
		}

		else
		{
			grip_select = 1;
			CLEARBIT(PORTD, YAW_INDICATOR);
			SETBIT(PORTD, GRIP_INDICATOR);
		}

		//_delay_ms(200);
		
		while(joystick_button_status())
		{
			;			
		}
	}
}

//ISR(INT0_vect)
//{
		//if(grip_select == 1)
		//grip_select = 0;
		//else
		//grip_select = 1;
		//cli();
		//_delay_ms(200);
		//sei();
//}

void tilt_from_to(int from, int to)
{
	if (from <= to)
	{
		for(int i = from; i <= to; i++)
		{
			servo_setPos(SERVO_TILT_OUT, i, SERVO_TILT_MIN, SERVO_TILT_MAX);
			_delay_ms(100);
			prev_tilt = i;
		}
	}
	else
	{
		for(int i = from; i >= to; i--)
		{
			servo_setPos(SERVO_TILT_OUT, i, SERVO_TILT_MIN, SERVO_TILT_MAX);
			_delay_ms(100);
			prev_tilt = i;
		}
	}	
	eeprom_write_byte((uint8_t*)EEPROM_ADD_TILT, prev_tilt);
}

void grip(int to)
{
	int from = eeprom_read_byte(EEPROM_ADD_GRIP);
	if(from <= to) //if the current gap is smaller 
	{
		for(int i = from; i <= (to + 10); i++) //increase the gap
		{
			servo_setPos(SERVO_GRIP_OUT, i, SERVO_GRIP_MIN, SERVO_GRIP_MAX);
			_delay_ms(100);
			prev_grip = i;
		}
		
		for(int i = prev_grip; i >= to; i--) //decrease the gap
		{
			servo_setPos(SERVO_GRIP_OUT, i, SERVO_GRIP_MIN, SERVO_GRIP_MAX);
			_delay_ms(100);
			prev_grip = i;
		}
	}
	else
	{
		for(int i = from; i >= to; i--) //decrease the gap
		{
			servo_setPos(SERVO_GRIP_OUT, i, SERVO_GRIP_MIN, SERVO_GRIP_MAX);
			_delay_ms(100);
			prev_grip = i;
		}
	}	
	eeprom_write_byte((uint8_t*)EEPROM_ADD_GRIP, prev_grip);	
}

void release()
{
	int from = eeprom_read_byte(EEPROM_ADD_GRIP);
	for(int i = from; i <= (from + 15); i++) //increase the gap
	{
		servo_setPos(SERVO_GRIP_OUT, i, SERVO_GRIP_MIN, SERVO_GRIP_MAX);
		_delay_ms(100);
		prev_grip = i;
	}
	eeprom_write_byte((uint8_t*)EEPROM_ADD_GRIP, prev_grip);
}

int tilt_duty_cycle(int joystick_position)
{
	//return (joystick_position / 1023)  * 430  + 120;
	//return (joystick_position * (4300 / 1023) + 1200 ) / 10;
	//return map_value(joystick_position, 120, 550, 0, 1023);
		
}	
int grip_duty_cycle(int joystick_position)
{
	return (joystick_position * (2200 / 1023) + 1800 ) / 10;
}

int map_value(int x, int in_min, int in_max, int out_min, int out_max)	// arduino map
{
	
	float value = ((float)(x - in_min) * (float)(out_max - out_min)) / (float)(in_max - in_min) + (float)out_min;
	
	return (int)(value);
	//return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void automatic_mode(int yaw, int tilt, int grip)
{
	//automatic_yaw(yaw);
	//_delay_ms(2000);
	//automatic_tilt(tilt);
	//automatic_grip(grip);
	
	//Initialize 
	automatic_grip(400);
	automatic_tilt(90);
	prev_grip = 400;
	prev_tilt = 90;
	
	
	//Tilt down
	automatic_tilt(180);
	
	//Grip the object
	automatic_grip(grip);
	
	//Tilt up
	automatic_tilt(tilt);
	
	//Rotate by yaw angles
	automatic_yaw(yaw);
	
	//Tilt down
	automatic_tilt(180);
	
	//Release the object
	automatic_grip(400);
	
	//Tilt up
	automatic_tilt(tilt);
	
	//Rotate by -yaw angles
	automatic_yaw(-yaw);
}

void automatic_yaw(int yaw)
{
	if(yaw >= 0)
	{
		int yaw_angle = map_value(yaw, 0, 360, 0, 400);
		rotate_stepper(yaw_angle, STEP_CCW);
	}
	
	else
	{
		int yaw_angle = map_value(-yaw, 0, 360, 0, 400);
		rotate_stepper(yaw_angle, STEP_CW);
	}
	_delay_ms(2000);
}
void automatic_tilt(int tilt)
{
	//if(tilt >= prev_tilt)
	//{
		//for(int i = 0; i < (tilt - prev_tilt); i = i + 2)
		//{
			//int tilt_angle = map_value((prev_tilt + i), 90, 180,316, 550);
			//servo_setPos(SERVO_TILT_OUT, tilt_angle, SERVO_TILT_MIN, SERVO_TILT_MAX);
			//_delay_ms(100);
		//}
	//}
	//else
	//{
		//for(int i = 0; i < (prev_tilt - tilt); i = i + 2)
		//{
			//int tilt_angle = map_value((prev_tilt - i), 90, 180,316, 550);
			//servo_setPos(SERVO_TILT_OUT, tilt_angle, SERVO_TILT_MIN, SERVO_TILT_MAX);
			//_delay_ms(100);
		//}
	//}				
		
	
	int tilt_angle = map_value(tilt, 90, 180,316, 550);
	servo_setPos(SERVO_TILT_OUT, tilt_angle, SERVO_TILT_MIN, SERVO_TILT_MAX);
	_delay_ms(2000);
}

void automatic_grip(int grip)
{
	servo_setPos(SERVO_GRIP_OUT, grip, SERVO_GRIP_MIN, SERVO_GRIP_MAX);
	_delay_ms(2000);
}

/*****************************************************************************************/