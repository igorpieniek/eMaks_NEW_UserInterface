/*
 * CanManager.h
 *
 *  Created on: 27.03.2020
 *      Author: Igor
 */

#ifndef CAN_CANMANAGER_H_
#define CAN_CANMANAGER_H_

#include "main.h"
#include "can.h"
#include "ModeManager.h"
#include <malloc.h>



#define VELOCITY_FRAME_ID   0x23E //TODO
#define TURN_FRAME_ID  		0x24D
#define STATUS_FRAME_ID  	0x25D

#define I3_VELOCITY_FRAME_ID    0x26D
#define I3_TURN_FRAME_ID  		0x27D

#define JOYSTICK_MODE_MSG 	0x00
#define ACRO_MODE_MSG		0x01
#define SEMI_MODE_MSG		0x02
#define AUTONOMOUS_MODE_MSG 0x03

#define ENABLE_DRIVE_MSG  	0x01
#define DISABLE_DRIVE_MSG 	0x00

#define MAX_CANVALUE 0xC800

#define STATUS_MODE_BYTE      0
#define STATUS_PERMITION_BYTE 1

#define POSITIVE_SIGN 1
#define NEGATIVE_SIGN 0

#define MAX_PERCERTAGE_VALUE 100.f

// Tx
#define STEERING_FRAME_LENGTH 4
#define STEERING_VELOCITY_FRAME_ID  0x21D //old x axis
#define STEERING_TURN_FRAME_ID   	0x22D // old y axis


class CanManager {
private:
	void convertStatusData_Rx(uint8_t * data);
	ModeManager::RC_MODE getRCmodeStatus_Rx(uint8_t data);
	ModeManager::DRIVE_MODE getDriveModestatus_Rx(uint8_t data);

	// Tx part
	enum SEND_MODE{
		TURN = 0,
		VELOCITY,
		STATUS
	};
	void sendMsg(SEND_MODE mode, uint8_t * msgData);
	uint16_t convertFloatToUint16t(float value);
	uint8_t * convertToFrame_Tx(uint8_t sign, uint16_t value, SEND_MODE mode);
	uint8_t* encode_frame_big_endian(uint8_t* data , uint8_t data_length);
	void convertVelocityTurnData_Tx(float value, uint8_t sign, SEND_MODE mode);

	//Tx part
	void setVelocity(uint8_t*vel, ModeManager::MSG_ORIGIN origin);
	void setTurn(uint8_t* turn, ModeManager::MSG_ORIGIN origin);


	void hal_can_send(uint16_t frame_id, uint8_t dlc, uint8_t* data);
	CAN_FilterTypeDef hcan_filter;
	void hal_can_filter_init(void);

public:
	// RX part
	void init();
	void process();

	void getData_Rx(uint32_t frame_id, uint8_t* data, uint8_t dlc);
	void sendVelocity(float vel, uint8_t sign);
	void sendTurn(float turn, uint8_t sign);
	void stopAllMotors();


	typedef struct{
		CAN_TxHeaderTypeDef     header;
		uint32_t 				mailbox;
		uint8_t*				data;
	}hal_can_messageTx;

	typedef struct{
		CAN_RxHeaderTypeDef	    header;
		uint32_t 				mailbox;
		uint8_t*				data;
	}hal_can_messageRx;

	hal_can_messageRx canMsgRx;

	CanManager();
	virtual ~CanManager();
};

extern CanManager canManager;

#endif /* CAN_CANMANAGER_H_ */