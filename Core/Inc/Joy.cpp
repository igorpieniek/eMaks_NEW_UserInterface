/*
 * Joy.cpp
 *
 *  Created on: 22.10.2020
 *      Author: Igor
 */

#include <Joy.h>



Joy::Joy(float max, float min, float neutral, uint8_t res, float ref, float n_err):
		v_max(max), v_min(min), v_neutral(neutral),
		bit_resolution(res), v_ref(ref),
		neutral_error(n_err)
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
	calculate_joy_data();
}

void Joy::convert_adc_value(uint32_t raw){
	raw_data = raw;
	converted_data = ( (float)raw_data * v_ref ) / (float)max_number_levels;

}
void Joy::calculate_joy_data(void){
	if(is_joy_near_neautral()){
		percentage_value = 0.0f;
		sign = POSITIVE;
		return;
	}
	if (converted_data > v_max){
		percentage_value = 100.0f;
		sign = POSITIVE;
		return;
	}

	if(converted_data >= v_neutral){
		percentage_value = 100 * fabs((converted_data - v_neutral) / v_max_normalized );
		sign = POSITIVE;
	}else{
		percentage_value = 100 * fabs((converted_data - v_neutral) / v_min_normalized );
		sign = NEGATIVE;
	}


}
uint8_t Joy::is_joy_near_neautral(){
	return (converted_data > (v_neutral - neutral_error) &&
		   converted_data < (v_neutral + neutral_error));
}



