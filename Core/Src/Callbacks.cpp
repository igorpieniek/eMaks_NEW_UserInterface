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

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
//  if (htim->Instance == TIM6) {
//	  canManager.stopAllMotors();
//  }
//  if (htim->Instance == TIM3) {
//	  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
//	  if (modeManager.isJoystickMode()){
//		joystick.process();
//	  }
//  }
//}
//
//void HAL_CAN_RxFifo1MsgPendingCallback (CAN_HandleTypeDef* hcan ){
//
//	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,
//			&canManager.can_messages.rx_header,
//			canManager.can_messages.rx_data );
//
//	canManager.process();
//
//}
