/*
 * CanManager.cpp
 *
 *  Created on: 27.03.2020
 *      Author: Igor
 */

#include "CanManager.h"
#include <math.h>

CanManager canManager;

void CanManager::init(){
	hal_can_filter_init();

}
void CanManager::process(){
	getData_Rx(canMsgRx.header.StdId,
			canMsgRx.data,
			canMsgRx.header.DLC);
}
/////////////////////////RX PART///////////////////////////////////////////

void CanManager::convertStatusData_Rx(uint8_t * data){

	uint8_t status = data[STATUS_MODE_BYTE];
	uint8_t permition = data[STATUS_PERMITION_BYTE];
	modeManager.statusUpdate(getRCmodeStatus_Rx( status ), getDriveModestatus_Rx( permition ) );
}

ModeManager::RC_MODE CanManager::getRCmodeStatus_Rx(uint8_t status){
	if (status == JOYSTICK_MODE_MSG ) 		return ModeManager::MODE_JOYSICK;
	else if(status == ACRO_MODE_MSG ) 		return ModeManager::MODE_ACRO;
	else if(status == SEMI_MODE_MSG ) 		return ModeManager::MODE_SEMI;
	else if(status == AUTONOMOUS_MODE_MSG ) return ModeManager::MODE_AUTONOMOUS;
}

ModeManager::DRIVE_MODE CanManager::getDriveModestatus_Rx(uint8_t permition){
	if (permition == ENABLE_DRIVE_MSG  ) 		return ModeManager::ENABLE;
	else if(permition ==  DISABLE_DRIVE_MSG ) 	return ModeManager::DISABLE;
}

void CanManager::setVelocity(uint8_t* vel, ModeManager::MSG_ORIGIN origin){
	if (modeManager.velocityPermission(origin)) sendMsg(VELOCITY, vel);

}
void CanManager::setTurn(uint8_t* turn, ModeManager::MSG_ORIGIN origin){
	if (modeManager.turnPermission(origin)) sendMsg(TURN,turn);
}

void CanManager::getData_Rx(uint32_t frame_id, uint8_t* data, uint8_t dlc){
	if (frame_id == STATUS_FRAME_ID) convertStatusData_Rx( data); // function also update status in modemanager
	else if ( frame_id == VELOCITY_FRAME_ID ) 		setVelocity( data , ModeManager::RC );
	else if ( frame_id == I3_VELOCITY_FRAME_ID )	setVelocity( data ,ModeManager::I3  );
	else if ( frame_id == TURN_FRAME_ID )			setTurn( data, ModeManager::RC  );
	else if ( frame_id == I3_TURN_FRAME_ID )	 	setTurn( data, ModeManager::I3  );
}

/////////////////////////TX PART///////////////////////////////////////////

void CanManager::sendMsg(SEND_MODE mode, uint8_t * msgData){
	if (mode == TURN && sizeof(msgData)==STEERING_FRAME_LENGTH ){
		hal_can_send( STEERING_VELOCITY_FRAME_ID, STEERING_FRAME_LENGTH , msgData);
	}
	else if (mode == VELOCITY && sizeof(msgData)==STEERING_FRAME_LENGTH){
		hal_can_send( STEERING_TURN_FRAME_ID,  STEERING_FRAME_LENGTH , msgData);
	}
}

uint16_t CanManager::convertFloatToUint16t(float value){
	float range = 128;
	if( value > range){
		return range;
	}
	return(uint16_t)(value * pow(2, 16) /range);
}

uint8_t * CanManager::convertToFrame_Tx(uint8_t sign, uint16_t value, SEND_MODE mode){
	if (mode == VELOCITY || mode == TURN ){
		uint8_t data_to_encode[]={
				(uint8_t)(sign >> 8),
				(uint8_t) sign,
				(uint8_t)(value >> 8 ),
				(uint8_t) value,
		};
		return encode_frame_big_endian(data_to_encode,STEERING_FRAME_LENGTH);
	}else{return 0;}
}

uint8_t* CanManager::encode_frame_big_endian(uint8_t* data , uint8_t data_length){
	 uint8_t* encoded_data = (uint8_t*)calloc(data_length, sizeof(uint8_t));
	for( uint8_t i = 1 ; i <= data_length  ;i++){
		encoded_data[i] = data[data_length-i];
	}
	return encoded_data;
}
void CanManager::convertVelocityTurnData_Tx(float value, uint8_t sign, SEND_MODE mode){
	uint16_t convertedData = convertFloatToUint16t(value);
	uint8_t * frameData = convertToFrame_Tx(sign, convertedData , mode);
	sendMsg(mode, frameData );
}
void CanManager::sendVelocity(float vel, uint8_t sign){
	convertVelocityTurnData_Tx(vel, sign, VELOCITY);
}
void CanManager::sendTurn(float turn, uint8_t sign){
	convertVelocityTurnData_Tx(turn, sign, TURN);
}

void CanManager::stopAllMotors(){
	convertVelocityTurnData_Tx(0.f, 0,VELOCITY);
	convertVelocityTurnData_Tx(0.f, 0,TURN);
}

void CanManager::hal_can_send(uint16_t frame_id, uint8_t dlc, uint8_t* data){
	HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_RESET);
	hal_can_messageTx canMsgTx;
	canMsgTx.data = data;
	canMsgTx.header.DLC = dlc;
	canMsgTx.header.RTR = CAN_RTR_DATA;
	canMsgTx.header.IDE  = CAN_ID_STD;
	canMsgTx.header.StdId = frame_id;

	HAL_CAN_AddTxMessage(&hcan, &(canMsgTx.header),canMsgTx.data,&(canMsgTx.mailbox));

}

void CanManager::hal_can_filter_init(void){
	hcan_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	hcan_filter.FilterIdHigh = 0xFFFF;
	hcan_filter.FilterIdLow = 0x0;
	hcan_filter.FilterIdHigh = 0x0;
	hcan_filter.FilterIdLow = 0x0;
	hcan_filter.FilterScale = CAN_FILTERSCALE_32BIT;
	hcan_filter.FilterActivation = ENABLE;

	HAL_CAN_ConfigFilter(&hcan,&hcan_filter);
}
CanManager::CanManager() {

}

CanManager::~CanManager() {
	// TODO Auto-generated destructor stub
}

