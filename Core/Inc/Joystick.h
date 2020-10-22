/*
 * Joystick.h
 *
 *  Created on: 14.07.2020
 *      Author: Igor
 */

#ifndef SRC_JOYSTICK_H_
#define SRC_JOYSTICK_H_

#include "main.h"
#include <stdbool.h>
#include "adc.h"
#include "math.h"
#include "canManager.h"
#include "Joy.h"

#define X_AXIS_ADC_CHANNEL  2 //index in adc channels
#define Y_AXIS_ADC_CHANNEL  1
#define AXIS_NUMBER    		2

//#define X_AXIS_INDEX 0
//#define Y_AXIS_INDEX 1


// configurable defines al in V:
#define V_NEUTRAL_X_AXIS 1.2f
#define V_NEUTRAL_Y_AXIS 1.2f
#define V_MAX_X_AXIS     3.0f
#define V_MIN_X_AXIS     0.1f
#define V_MAX_Y_AXIS     3.0f
#define V_MIN_Y_AXIS     0.1f
#define MEASUREMENT_ERROR 0.06f

#define NUMBER_OF_CHANNELS     4
#define ADC_BIT_RESOLUTION	   12
#define ADC_REFERENCE_VOLTAGE  3.3


class Joystick {
public:

	typedef enum {
		X_AXIS_INDEX,
		Y_AXIS_INDEX
	}Axis_num;

	void init(void);
	uint8_t getSign(Axis_num ax);
	float getPercentage(Axis_num ax);

	void update_measurments(void);

	Joystick();
	~Joystick();

private:

	void addJoy();
	Joy axis[NUMBER_OF_CHANNELS];
	uint32_t adc_raw_values[NUMBER_OF_CHANNELS];


};

extern Joystick joystick;
#endif /* SRC_JOYSTICK_H_ */
