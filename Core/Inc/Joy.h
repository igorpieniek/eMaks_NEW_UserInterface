/*
 * Joy.h
 *
 *  Created on: 22.10.2020
 *      Author: Igor
 */

#ifndef INC_JOY_H_
#define INC_JOY_H_

#include "main.h"
#include "math.h"


class Joy {


public:
	Joy(float max, float min, float neutral, uint8_t res, float ref);
	virtual ~Joy();


	void calculate_data(uint32_t raw);
private:
	uint32_t raw_data;
	float v_max_normalized;
	float v_min_normalized;
	float converted_data;
	float percentage_value;
	uint8_t  sign;

	uint32_t max_number_levels;

	float v_max;
	float v_min;
	float v_neutral;
	uint8_t bit_resolution;
	float v_ref;

	void convert_adc_value(uint32_t raw);
	void calculate_joy_data(void);
	uint8_t is_joy_near_neautral();

};



#endif /* INC_JOY_H_ */
