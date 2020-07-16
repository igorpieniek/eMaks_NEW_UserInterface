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
#define X_AXIS_INDEX   1
#define Y_AXIS_INDEX   2
#define AXIS_NUMBER    2


// configurable defines al in V:
#define V_NEUTRAL_X_AXIS 2.1f
#define V_NEUTRAL_Y_AXIS 2.1f
#define V_MAX_X_AXIS     3.0f
#define V_MIN_X_AXIS     1.2f
#define V_MAX_Y_AXIS     3.0f
#define V_MIN_Y_AXIS     1.2f
#define MEASUREMENT_ERROR 0.06f

#define NUMBER_OF_CHANNELS     4
#define ADC_BIT_RESOLUTION	   12
#define ADC_REFERENCE_VOLTAGE  3.3

class Joystick {
public:
	typedef enum{
		negative = 0,
		positive
	}adc_direction;

	typedef struct{
		uint32_t raw_data;
		float v_neutral;
		float v_max_normalized;
		float v_min_normalized;
		float converted_data;
		float percentage_value;
		uint8_t  sign;
	}adc_data;

	typedef struct{
		adc_data measurements[NUMBER_OF_CHANNELS];
		uint8_t bit_resolution;
		float reference_voltage;
	}joystick_data;

	typedef struct{
		uint8_t index;
		float v_max;
		float v_min;
		float v_neutral;
	}ax_params;

	typedef struct{
		ax_params* params;
		uint8_t size;
	}joy_axis;

	void init(void);
	joystick_data* get_joy_pointer(void);
	void convert_adc_values(void);
	void calculate_joy_data(void);
	void get_adc_data(joystick_data* joy,uint32_t* data);
	uint8_t is_joy_near_neautral(ax_params params);

	void process(void);

	Joystick();
	virtual ~Joystick();

private:
	joystick_data joy;

	ax_params joy_axis_params[2] = {
			{.index = X_AXIS_INDEX, .v_max = V_MAX_X_AXIS,.v_min = V_MIN_X_AXIS,.v_neutral = V_NEUTRAL_X_AXIS},
			{.index = Y_AXIS_INDEX, .v_max = V_MAX_Y_AXIS,.v_min = V_MIN_Y_AXIS,.v_neutral = V_NEUTRAL_Y_AXIS}
	};

	joy_axis axis ={
		 .params =joy_axis_params,
		 .size = sizeof(joy_axis_params)/sizeof(ax_params)
	};

	uint32_t adc_raw_values[NUMBER_OF_CHANNELS];

	void calcualate_axis_voltage(ax_params params);
	uint32_t* get_hal_adc_raw_data(void);
	void sendToCan(void);
};

extern Joystick joystick;
#endif /* SRC_JOYSTICK_H_ */
