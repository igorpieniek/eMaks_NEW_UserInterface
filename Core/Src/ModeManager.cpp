/*
 * ModeManager.cpp
 *
 *  Created on: 27.03.2020
 *      Author: Igor
 */

#include "ModeManager.h"

ModeManager modeManager;

void ModeManager::init(){
	driveMode = ENABLE;
	RCmode = MODE_JOYSICK;
	HAL_TIM_Base_Start_IT(&PROCESS_TIMER);
	isIdleTimerON = 1;
}
uint8_t ModeManager::velocityPermission(MSG_ORIGIN origin){
	resetIdleTimer();
	if (getDriveMode() == ENABLE){
		if(origin == RC && (getRCmode() == MODE_ACRO ||
  	   	   	     	 	 	getRCmode() == MODE_SEMI) ){
			return 1;
		}
		else if(origin == I3 && (getRCmode() == MODE_AUTONOMOUS) ){
			return 1;
		}
		else return 0;
	}
	else return 0;
}

uint8_t ModeManager::turnPermission(MSG_ORIGIN origin){
	resetIdleTimer();
	if (getDriveMode() == ENABLE){
		if(origin == RC && getRCmode() == MODE_ACRO){
			return 1;
		}
		else if(origin == I3 && (getRCmode() == MODE_AUTONOMOUS ||
				   	   	   	     getRCmode() == MODE_SEMI) ){
			return 1;
		}
		else return 0;
	}
	else return 0;
}


void ModeManager::statusUpdate(RC_MODE RCstatus, DRIVE_MODE drivestatus){
	RCmode = RCstatus;
	driveMode = drivestatus;
	if (RCstatus !=MODE_JOYSICK){
		startIdleTimer();
	}
	else stopIdleTimer();
}

void ModeManager::startIdleTimer(){
	if(!isIdleTimerON){
		HAL_TIM_Base_Start_IT(&IDLE_TIMER);
	}
}

void ModeManager::stopIdleTimer(){
	if(isIdleTimerON){
		HAL_TIM_Base_Stop_IT(&IDLE_TIMER);
		resetIdleTimer();
	}
}
void ModeManager::resetIdleTimer(){
	__HAL_TIM_SET_COUNTER(&IDLE_TIMER, 0);
}

ModeManager::DRIVE_MODE ModeManager::getDriveMode(){
	return driveMode;
}
ModeManager::RC_MODE ModeManager::getRCmode(){
	return RCmode;
}


uint8_t ModeManager::isJoystickMode(){
	if (getDriveMode() == ENABLE &&
		getRCmode() == MODE_JOYSICK) return 1;
	else return 0;
}

ModeManager::ModeManager() {
	// TODO Auto-generated constructor stub

}

ModeManager::~ModeManager() {
	// TODO Auto-generated destructor stub
}

