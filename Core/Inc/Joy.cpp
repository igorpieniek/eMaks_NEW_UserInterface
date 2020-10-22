/*
 * Joy.cpp
 *
 *  Created on: 22.10.2020
 *      Author: Igor
 */

#include <Joy.h>



Joy::Joy(float max, float min, float neutral): v_max(max), v_min(min), v_neutral(neutral) {
	// TODO Auto-generated constructor stub
	v_max_normalized = v_max - v_neutral;
	v_min_normalized = v_min - v_neutral;
}

Joy::~Joy() {
	// TODO Auto-generated destructor stub
}


void Joy::convert_adc_values(void){

}
void Joy::calculate_joy_data(void){

}
uint8_t Joy::is_joy_near_neautral(){

}

