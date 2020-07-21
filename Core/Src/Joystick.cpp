/*
 * Joystick.cpp
 *
 *  Created on: 14.07.2020
 *      Author: Igor
 */

#include "Joystick.h"

Joystick joystick;

void Joystick::init(void){
    HAL_ADC_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1,adc_raw_values,NUMBER_OF_CHANNELS);

	for(uint8_t i = 0; i < axis.size; i++){
	 joy.measurements[axis.params[i].index].v_max_normalized =
			 axis.params[i].v_max -axis.params[i].v_neutral;
	 joy.measurements[axis.params[i].index].v_min_normalized =
			 axis.params[i].v_min -axis.params[i].v_neutral;
	}
}

//joystick* Joystick::get_joy_pointer(void){
//	return &joy;
//}

void Joystick::get_adc_data(joystick_data* joy,uint32_t* data){
	for(uint8_t i = 0 ; i < NUMBER_OF_CHANNELS; i++){
		joy->measurements[i].raw_data = data[i];
	}
}

uint32_t* Joystick::get_hal_adc_raw_data(void){
	return adc_raw_values;
}

void Joystick::convert_adc_values(void){
	get_adc_data(&joy,get_hal_adc_raw_data());
	uint32_t divider = pow(2, joy.bit_resolution);
	//todo Lukas: add offset
	for(uint8_t i = 0 ; i < NUMBER_OF_CHANNELS;i++){
		if(joy.measurements[i].raw_data == NULL){
			joy.measurements[i].converted_data = -1.0;
			continue;
		}
		joy.measurements[i].converted_data =
				((float)joy.measurements[i].raw_data
				* joy.reference_voltage)/(float)divider;
	}
}
uint8_t Joystick::is_joy_near_neautral(ax_params params){
	return(joy.measurements[params.index].converted_data > (params.v_neutral - MEASUREMENT_ERROR)
			&& joy.measurements[params.index].converted_data < (params.v_neutral + MEASUREMENT_ERROR ));
}


void Joystick::calcualate_axis_voltage(ax_params params){
	if(is_joy_near_neautral(params)){
		joy.measurements[params.index].percentage_value = 0.0;
		joy.measurements[params.index].sign = positive;
		return;
	}
	if (joy.measurements[params.index].converted_data >= params.v_max){
		joy.measurements[params.index].percentage_value = 100.0;
		joy.measurements[params.index].sign = positive;
		return;
	}

	if( joy.measurements[params.index].converted_data > params.v_neutral ){
		joy.measurements[params.index].percentage_value =
		fabs((joy.measurements[params.index].converted_data -params.v_neutral)
				/ joy.measurements[params.index].v_max_normalized) * 100.0;
		joy.measurements[params.index].sign = positive;
	}
	else{
		joy.measurements[params.index].percentage_value =
		fabs((joy.measurements[params.index].converted_data -params.v_neutral)
				/ joy.measurements[params.index].v_min_normalized) * 100.0;
		joy.measurements[params.index].sign = negative;
	}
}

///todo Lukas: update tests
void Joystick::calculate_joy_data(void){
	for(uint8_t i = 0 ; i < axis.size; i++){
		calcualate_axis_voltage(axis.params[i]);
	}
}
void Joystick::sendToCan(void){
	float vel = joy.measurements[X_AXIS_INDEX].percentage_value;
	float turn = joy.measurements[Y_AXIS_INDEX].percentage_value;
	canManager.sendTurn(turn);
	canManager.sendVelocity(vel);

}

void Joystick::process(void){
	convert_adc_values();
	calculate_joy_data();
	sendToCan();
}


Joystick::Joystick() {

}

Joystick::~Joystick() {
	// TODO Auto-generated destructor stub
}

