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

	clearTxBuff();

	hal_can_filter_init();
	HAL_CAN_Start(&hcan);
	HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING);

}
void CanManager::joystickSendProcess(){
	if(modeManager.isJoystickMode()){
		uint8_t signX = joystick.getSign(Joystick::X_AXIS_INDEX);
		uint8_t signY = joystick.getSign(Joystick::Y_AXIS_INDEX);
		float   perX = joystick.getPercentage(Joystick::X_AXIS_INDEX);
		float   perY = joystick.getPercentage(Joystick::Y_AXIS_INDEX);

		uint16_t uperX = convertFloatToUint16t(perX);
		uint16_t uperY = convertFloatToUint16t(perY);


		// X conversion and sending (VELOCITY)
		convertToFrame_Tx(signX, uperX);
		sendMsg(JOYSTICK_X);

		// X conversion and sending (TURN)
		convertToFrame_Tx(signY, uperY);
		sendMsg(JOYSTICK_Y);

	}

}
/////////////////////////RX PART///////////////////////////////////////////

void CanManager::rewriteFrameProcess(uint32_t frame_id, uint8_t* data){
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
	else if (mode == JOYSTICK_X){
		hal_can_send( STEERING_VELOCITY_FRAME_ID,  STEERING_FRAME_LENGTH );
	}
	else if (mode == JOYSTICK_Y){
		hal_can_send( STEERING_TURN_FRAME_ID ,  STEERING_FRAME_LENGTH );
	}

	clearTxBuff();
}

void CanManager::clearTxBuff(){
	for(uint8_t i = 0; i<8; i++) canMsgTx.data[i]=0;
}


/////////////////////////////////////////////////////////////////
uint8_t CanManager::getSign_Tx(float value){
	if (value >=0) return POSITIVE_SIGN;
	else return NEGATIVE_SIGN;
}

uint16_t CanManager::convertFloatToUint16t( float value){
	float range = 128;
	if( value > range){
		return range;
	}
	return(uint16_t)(value * pow(2, 16) /range);
}

void CanManager::convertToFrame_Tx(uint8_t sign, uint16_t value){
	canMsgTx.data[0] = (uint8_t)(sign >> 8);
	canMsgTx.data[1] = (uint8_t) sign;
	canMsgTx.data[2] = (uint8_t)(value >> 8 );
	canMsgTx.data[3] = (uint8_t) value;

	encode_frame_big_endian(STEERING_FRAME_LENGTH);
}
void CanManager::encode_frame_big_endian(uint8_t data_length){
	 uint8_t* encoded_data = new uint8_t[data_length];
	 if (encoded_data != NULL){
		for( uint8_t i = 1 ; i <= data_length  ;i++){
			encoded_data[i-1] = canMsgTx.data[data_length-i];
		}
		for( uint8_t i = 0 ; i < data_length  ;i++) canMsgTx.data[i] = encoded_data[i];
	 }
	 delete[] encoded_data;
}

/////////////////////////////////////////////////////////////////////

void CanManager::stopAllMotors(){
	uint8_t* data = (uint8_t*)calloc(8, sizeof(uint8_t)); // create array[8] and fill with 0
	fill_frame(data);
	sendMsg(VELOCITY);
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

