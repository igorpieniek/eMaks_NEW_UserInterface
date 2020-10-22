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
  if (htim->Instance == TIM4) {
	  canManager.stopAllMotors();
	  modeManager.stopIdleTimer();
  }
  if (htim->Instance == TIM3) {
	  if (modeManager.isJoystickMode()){
		  HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_SET);
		  joystick.update_measurments();
		  canManager.joystickSendProcess();
	  }
  }
}

void HAL_CAN_RxFifo0MsgPendingCallback (CAN_HandleTypeDef* hcan ){

	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,
			&canManager.canMsgRx.header,
			canManager.canMsgRx.data );

	canManager.rewriteFrameProcess(canManager.canMsgRx.header.StdId, canManager.canMsgRx.data);

}
