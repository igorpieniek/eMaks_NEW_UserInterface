/*
 * CanManager.cpp
 *
 *  Created on: 27.03.2020
 *      Author: Igor
 */

#include "CanManager.h"

CanManager canManager;


/////////////////////////RX PART///////////////////////////////////////////

float CanManager::getSign_Rx(uint8_t * data){
	uint16_t sign = uint8_To_uint16(data, 0);
	if(sign == 	NEGATIVE_SIGN) return -1.f;
	else					   return 1.f;
}

float CanManager::convertVelocityTurnData_Rx(uint8_t * data){
	uint16_t rawData = uint8_To_uint16(data, 2); // because value start from 2 byte
	return  getSign_Rx(data) * (float)(rawData / MAX_CANVALUE)*100.f; // PERCENTAGE CALCULATE
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


uint16_t CanManager::uint8_To_uint16(uint8_t* data, uint8_t start_byte){
	return ((uint16_t)data[start_byte + 1] << 8) | (uint16_t)data[start_byte]; //copy from can_frrames.c
}

void CanManager::setVelocity(float vel, ModeManager::MSG_ORIGIN origin){
	float velocity = modeManager.setVelocity( vel, origin );
	sendVelocity(velocity);
}
void CanManager::setTurn(float turn, ModeManager::MSG_ORIGIN origin){
	float tur = modeManager.setVelocity( turn, origin );
	sendTurn(tur);
}

void CanManager::getData_Rx(uint32_t frame_id, uint8_t* data, uint8_t dlc){
	if (frame_id == STATUS_FRAME_ID) convertStatusData_Rx( data); // function also update status in modemanager
	else if ( frame_id == VELOCITY_FRAME_ID ) 		setVelocity( convertVelocityTurnData_Rx( data ), ModeManager::RC );
	else if ( frame_id == I3_VELOCITY_FRAME_ID )	setVelocity( convertVelocityTurnData_Rx( data ),ModeManager::I3  );
	else if ( frame_id == TURN_FRAME_ID )			setTurn( convertVelocityTurnData_Rx( data ), ModeManager::RC  );
	else if ( frame_id == I3_TURN_FRAME_ID )	 	setTurn( convertVelocityTurnData_Rx( data ), ModeManager::I3  );
}

/////////////////////////TX PART///////////////////////////////////////////

void CanManager::sendMsg(SEND_MODE mode, uint8_t * msgData){
	if (mode == TURN && sizeof(msgData)==STEERING_FRAME_LENGTH ){
//		hal_can_send( STEERING_VELOCITY_FRAME_ID, STEERING_FRAME_LENGTH , msgData);
	}
	else if (mode == VELOCITY && sizeof(msgData)==STEERING_FRAME_LENGTH){
//		hal_can_send( STEERING_TURN_FRAME_ID,  STEERING_FRAME_LENGTH , msgData);
	}
}

uint8_t CanManager::getSign_Tx(float value){
	if (value >=0) return POSITIVE_SIGN;
	else return NEGATIVE_SIGN;
}

uint16_t CanManager::convertFloatToUint16t(float maxValue, float value){
	float percentage = value/maxValue;
	if (percentage > 1) percentage = 1;
	return (uint16_t)(MAX_CANVALUE * percentage);
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

uint8_t* CanManager::encode_frame_big_endian(uint8_t* data , uint8_t data_length){
	 uint8_t* encoded_data = (uint8_t*)calloc(data_length, sizeof(uint8_t));
	for( uint8_t i = 1 ; i <= data_length  ;i++){
		encoded_data[i] = data[data_length-i];
	}
	return encoded_data;
}
void CanManager::convertVelocityTurnData_Tx(float value, SEND_MODE mode){
	uint8_t sign = getSign_Tx(value);
	if (sign == NEGATIVE_SIGN){ value *= -1; } //Change signt to positive after check
	uint16_t convertedData = convertFloatToUint16t(MAX_PERCERTAGE_VALUE,value);
	sendMsg(mode, convertToFrame_Tx(sign, convertedData , mode) );
}
void CanManager::sendVelocity(float vel){
	convertVelocityTurnData_Tx(vel, VELOCITY);
}
void CanManager::sendTurn(float turn){
	convertVelocityTurnData_Tx(turn, TURN);
}

void CanManager::stopAllMotors(){
	convertVelocityTurnData_Tx(0.f, VELOCITY);
	convertVelocityTurnData_Tx(0.f, TURN);
}


void CanManager::hal_can_send(uint16_t frame_id, uint8_t dlc, uint8_t* data){
	can_messages.tx_data = data;
	can_messages.tx_header.DLC = dlc;
	can_messages.tx_header.RTR = CAN_RTR_DATA;
	can_messages.tx_header.IDE  = CAN_ID_STD;
	can_messages.tx_header.StdId = frame_id;

	HAL_CAN_AddTxMessage(&hcan, &(can_messages.tx_header),can_messages.tx_data,&(can_messages.mailbox));

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
	hal_can_filter_init();

}

CanManager::~CanManager() {
	// TODO Auto-generated destructor stub
}

