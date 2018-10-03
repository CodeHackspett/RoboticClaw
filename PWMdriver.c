
// PWMdriver.c
// John Berge, 2013
// Utility functions for PWM driver board (Adafruit) based on PCA9685

#include "i2cmaster.h"
#include "PWMdriver.h"


/*	servo_setPos: Function for controlling output channels of PWM driver board
*	Arguments:
*		output: Output channel, 0-15
*		servo_pos: Sets the duty cycle (pulse width), 0-4095, will be bounded by min/max
*		min: minimum safe servo_pos setting for servo, given as constant defined in PWMdriver.h
*		max: maximum safe servo_pos setting for servo, given as constant defined in PWMdriver.h
*	
*	Usage example 1: Setting position of tilt servo, connected to output channel 3
*		servo_setPos(3, my_desired_pos, SERVO_TILT_MIN, SERVO_TILT_MAX);
*	Usage example 2: Setting position of grip servo, connected to output channel 7			
*		servo_setPos(7, my_desired_pos, SERVO_GRIP_MIN, SERVO_GRIP_MAX);
*/
uint16_t servo_setPos(uint8_t output, uint16_t servo_pos, uint16_t min, uint16_t max){
	if(servo_pos > max){
		servo_pos = max;
		}else if(servo_pos < min){
		servo_pos = min;
	}
	PWMdriver_setPWM(output, 0, servo_pos);
	return servo_pos;
}

/* Init function, this needs to be called once */
void PWMdriver_init(void){
	PWMdriver_reset();
	PWMdriver_writeRegister(PWM_MODE1, 0b00010001);	// sleep
	PWMdriver_writeRegister(PWM_PRE_SCALE, 121);	// set prescaler, 122 => ~50Hz (49.62Hz) PWM freq (John) | 121 -> 50.03Hz
	PWMdriver_writeRegister(PWM_MODE1, 0b00100001);	// wake up, auto increment enabled
}

void PWMdriver_reset(void){
	i2c_start_wait(0x00);
	i2c_write(0x06);
	i2c_stop();
}

void PWMdriver_setPWM(uint8_t num, uint16_t on, uint16_t off){
	i2c_start_wait(PWM_I2C_ADDR+I2C_WRITE);
	i2c_write(PWM_LED0_ON_L + 4*num);	// write to first out of four on/off registers for output num
	i2c_write(on);		// write low byte of on
	i2c_write(on>>8);	// write high byte of on
	i2c_write(off);		// write low byte of off
	i2c_write(off>>8);	// write high byte of off
	i2c_stop();
}

void PWMdriver_writeRegister(unsigned char regAddr, unsigned char data){
	i2c_start_wait(PWM_I2C_ADDR+I2C_WRITE);
	i2c_write(regAddr);
	i2c_write(data);
	i2c_stop();
}
