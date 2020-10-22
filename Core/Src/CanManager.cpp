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
	canMsgTx.header.RTR = CAN_RTR_DATA;
	canMsgTx.header.IDE  = CAN_ID_STD;
	canMsgTx.header.ExtId = 0x01;
	canMsgTx.header.TransmitGlobalTime = DISABLE;
	hal_can_filter_init();
	HAL_CAN_Start(&hcan);
	HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING);

}
void CanManager::process(){
	getData_Rx(canMsgRx.header.StdId,
			canMsgRx.data,
			canMsgRx.header.DLC);
}
/////////////////////////RX PART///////////////////////////////////////////

void CanManager::getData_Rx(uint32_t frame_id, uint8_t* data, uint8_t dlc){
	if (frame_id == STATUS_FRAME_ID) convertStatusData_Rx( data); // function also update status in modemanager
	else if ( frame_id == VELOCITY_FRAME_ID ) 		setVelocity( data, ModeManager::RC );
	else if ( frame_id == I3_VELOCITY_FRAME_ID )	setVelocity( data, ModeManager::I3  );
	else if ( frame_id == TURN_FRAME_ID )			setTurn( data , ModeManager::RC  );
	else if ( frame_id == I3_TURN_FRAME_ID )	 	setTurn( data , ModeManager::I3  );
}

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


void CanManager::setVelocity(uint8_t* data, ModeManager::MSG_ORIGIN origin){
	if (modeManager.velocityPermission(origin)){
		fill_frame(data);
		sendMsg(VELOCITY);
	}

}
void CanManager::setTurn(uint8_t* data, ModeManager::MSG_ORIGIN origin){
	if (modeManager.turnPermission(origin)){
		fill_frame(data);
		sendMsg(TURN);
	}
}


void CanManager::fill_frame(uint8_t* data){
	for(uint8_t i=0; i <4; i++){
		canMsgTx.data[i] = data[i];
	}
}

/////////////////////////TX PART///////////////////////////////////////////

void CanManager::sendMsg(SEND_MODE mode){
	if (mode == TURN  ){
		hal_can_send( STEERING_VELOCITY_FRAME_ID, STEERING_FRAME_LENGTH );
	}
	else if (mode == VELOCITY){
		hal_can_send( STEERING_TURN_FRAME_ID,  STEERING_FRAME_LENGTH );
	}
}

uint8_t CanManager::getSign_Tx(float value){
	if (value >=0) return POSITIVE_SIGN;
	else return NEGATIVE_SIGN;
}

uint16_t CanManager::convertFloatToUint16t(float maxValue, float value){
//	float percentage = value/maxValue;
//	if (percentage > 1) percentage = 1;
//	return (uint16_t)(MAX_CANVALUE * percentage); // to tez daje dobre wyniki
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
	}
}




void CanManager::stopAllMotors(){
	uint8_t* data = (uint8_t*)calloc(8, sizeof(uint8_t)); // create array[8] and fill with 0
	fill_frame(data);
	sendMsg(VELOCITY, data);
	free(data);

}


void CanManager::hal_can_send(uint16_t frame_id, uint8_t dlc){
	canMsgTx.header.DLC = (uint32_t)dlc;
	canMsgTx.header.StdId = (uint32_t)frame_id;
	HAL_CAN_AddTxMessage(&hcan, &(canMsgTx.header),canMsgTx.data,&(canMsgTx.mailbox));

}

void CanManager::hal_can_filter_init(void){
	hcan_filter.FilterBank = 0;
	hcan_filter.FilterMode = CAN_FILTERMODE_IDMASK;
	hcan_filter.FilterScale = CAN_FILTERSCALE_32BIT;
	hcan_filter.FilterIdHigh = 0x0000;
	hcan_filter.FilterIdLow = 0x0000;
	hcan_filter.FilterMaskIdHigh = 0x0000;
	hcan_filter.FilterMaskIdLow = 0x0000;
	hcan_filter.FilterFIFOAssignment = CAN_RX_FIFO0;
	hcan_filter.FilterActivation = ENABLE;
	hcan_filter.SlaveStartFilterBank = 14;

	HAL_CAN_ConfigFilter(&hcan,&hcan_filter);
}
CanManager::CanManager() {

}

CanManager::~CanManager() {
	// TODO Auto-generated destructor stub
}

