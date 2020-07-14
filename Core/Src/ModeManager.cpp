/*
 * ModeManager.cpp
 *
 *  Created on: 27.03.2020
 *      Author: Igor
 */

#include "ModeManager.h"

ModeManager modeManager;

float ModeManager::setVelocity(float vel, MSG_ORIGIN origin){
	resetIdleTimer();
	if (getDriveMode() == ENABLE){
		if(origin == RC && (getRCmode() == MODE_ACRO ||
  	   	   	     	 	 	getRCmode() == MODE_SEMI) ){
			return vel;
		}
		else if(origin == I3 && (getRCmode() == MODE_AUTONOMOUS) ){
			return vel;
		}
		else return 0;
	}
	else return 0;
}

float ModeManager::setTurn(float turn, MSG_ORIGIN origin){
	resetIdleTimer();
	if (getDriveMode() == ENABLE){
		if(origin == RC && getRCmode() == MODE_ACRO){
			return turn;
		}
		else if(origin == I3 && (getRCmode() == MODE_AUTONOMOUS ||
				   	   	   	getRCmode() == MODE_SEMI) ){
			return turn;
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
	HAL_TIM_Base_Start_IT(&IDLE_TIMER);
}

void ModeManager::stopIdleTimer(){
	HAL_TIM_Base_Stop_IT(&IDLE_TIMER);
	resetIdleTimer();
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

//void ModeManager::sendVelocity(float vel){
//	//TODO : SENDING DATA TO CAN MANAGER
//}
//void ModeManager::sendTurn(float turn){
//	//TODO : SENDING DATA TO CAN MANAGER
//}
//void ModeManager::stopAll(){
//	sendVelocity(0);
//	sendTurn(0);
//}

//float ModeManager::joystickProcess(){
//	//TODO: get joystick data
//	joystick * joy = get_joy_pointer();
//	float vel = joy->measurements[X_AXIS_INDEX].percentage_value;
//	float turn = joy->measurements[Y_AXIS_INDEX].percentage_value;
////	sendVelocity(vel);
////	sendTurn(turn);
//	return vel; //todo: change this method in this case it won't be working
//}
//void ModeManager::process(){
//	if (getDriveMode() == ENABLE && getRCmode() == MODE_JOYSICK) joystickProcess();
//
//	//TODO: add IT if there will be no can signal for more than e.g. 2 s
//	// check status and if it equal to JOYSICK MODE Start sending data from joysick
//}
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

