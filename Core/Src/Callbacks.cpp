/*
 * Callbacks.cpp
 *
 *  Created on: 14.07.2020
 *      Author: Igor
 */


#include "main.h"
#include "tim.h"
#include "can.h"

#include "CanManager.h"
#include "ModeManager.h"
#include "Joystick.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if (htim->Instance == TIM6) {
	  canManager.stopAllMotors();
  }
  if (htim->Instance == TIM3) {
	  if (modeManager.isJoystickMode()){
		joystick.convert_adc_values(); //copy from /user_interface.c
		joystick.calculate_joy_data();
//		joystick.can_transmit_data();
	  }
  }
}

void HAL_CAN_RxFifo1MsgPendingCallback (CAN_HandleTypeDef* hcan ){

	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,
			&canManager.can_messages.rx_header,
			canManager.can_messages.rx_data );

	canManager.getData_Rx(canManager.can_messages.rx_header.StdId,
			canManager.can_messages.rx_data,
			canManager.can_messages.rx_header.DLC);

}
