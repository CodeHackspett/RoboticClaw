
// PWMdriver.h
// John Berge, 2013


// CONSTANT DEFINITIONS

#define PWM_I2C_ADDR 0x80

// PCA9685 register addresses
#define PWM_MODE1 0x00
#define PWM_MODE2 0x01
#define PWM_PRE_SCALE 0xFE
#define PWM_LED0_ON_L 0x06
#define PWM_LED0_ON_H 0x07
#define PWM_LED0_OFF_L 0x08
#define PWM_LED0_OFF_H 0x09

// parallax standard servo:
// #define SERVOMAX 480		// out of 4095
// #define SERVOMIN 100		// 100 => ~450us
// #define SERVOCENTER 290	// 290 => ~1.3ms

// tilt servo, do not change
#define SERVO_TILT_MIN 120		// 0.54ms
#define SERVO_TILT_NEUTRAL 330
#define SERVO_TILT_MAX 550		// 2.49ms

// grip servo, do not change
#define SERVO_GRIP_MIN 200		// 180 = 0.82ms; 340
#define SERVO_GRIP_MAX 400		// 1.81ms


// FUNCTION DECLARATIONS

uint16_t servo_setPos(uint8_t output, uint16_t servo_pos, uint16_t min, uint16_t max);
void PWMdriver_init(void);
void PWMdriver_reset(void);
void PWMdriver_setPWM(uint8_t num, uint16_t on, uint16_t off);
void PWMdriver_writeRegister(unsigned char regAddr, unsigned char data);
