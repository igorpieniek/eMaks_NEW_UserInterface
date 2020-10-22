/*
 * Joy.cpp
 *
 *  Created on: 22.10.2020
 *      Author: Igor
 */

#include <Joy.h>



Joy::Joy(float max, float min, float neutral, uint8_t res, float ref):
		v_max(max), v_min(min), v_neutral(neutral),
		bit_resolution(res), v_ref(ref)
{
	v_max_normalized = v_max - v_neutral;
	v_min_normalized = v_min - v_neutral;
	max_number_levels = pow(2, bit_resolution);
}

Joy::~Joy() {
	// TODO Auto-generated destructor stub
}

void Joy::calculate_data(uint32_t raw){
	convert_adc_value(raw);

}



void Joy::convert_adc_value(uint32_t raw){
	raw_data = raw;
	converted_data = ( (float)raw_data * v_ref ) / (float)max_number_levels;

}
void Joy::calculate_joy_data(void){

}
uint8_t Joy::is_joy_near_neautral(){

}

