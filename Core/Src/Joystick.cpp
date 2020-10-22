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

    addJoy();

}

void Joystick::addJoy(){
    axis[X_AXIS_INDEX].init( V_MAX_X_AXIS, V_MIN_X_AXIS, V_NEUTRAL_X_AXIS,
    				ADC_BIT_RESOLUTION, ADC_REFERENCE_VOLTAGE, MEASUREMENT_ERROR);

    axis[Y_AXIS_INDEX].init( V_MAX_Y_AXIS, V_MIN_Y_AXIS, V_NEUTRAL_Y_AXIS,
        				ADC_BIT_RESOLUTION, ADC_REFERENCE_VOLTAGE, MEASUREMENT_ERROR);
}

void Joystick::update_measurments(void){
	axis[X_AXIS_INDEX].calculate_data(adc_raw_values[X_AXIS_ADC_CHANNEL]);
	axis[Y_AXIS_INDEX].calculate_data(adc_raw_values[Y_AXIS_ADC_CHANNEL]);
}

uint8_t Joystick::getSign(Axis_num ax){
	return axis[ax].getSign();
}
float Joystick::getPercentage(Axis_num ax){
	return axis[ax].getPercentage();
}

Joystick::Joystick() {

}

Joystick::~Joystick() {
	// TODO Auto-generated destructor stub
}

