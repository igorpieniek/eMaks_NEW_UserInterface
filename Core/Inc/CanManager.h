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
#include "Joystick.h"
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

#define STATUS_MODE_BYTE      2
#define STATUS_PERMITION_BYTE 0

#define POSITIVE_SIGN 1
#define NEGATIVE_SIGN 0

#define MAX_PERCERTAGE_VALUE 100.f

// Tx
#define STEERING_FRAME_LENGTH 4
#define STEERING_VELOCITY_FRAME_ID  0x21D //old x axis
#define STEERING_TURN_FRAME_ID   	0x22D // old y axis


class CanManager {
private:
	float getSign_Rx(uint8_t * data);
	void convertStatusData_Rx(uint8_t * data);
	ModeManager::RC_MODE getRCmodeStatus_Rx(uint8_t data);
	ModeManager::DRIVE_MODE getDriveModestatus_Rx(uint8_t data);
	void setVelocity(uint8_t*  vel, ModeManager::MSG_ORIGIN origin);
	void setTurn(uint8_t*  turn, ModeManager::MSG_ORIGIN origin);
	void fill_frame(uint8_t* data);
	void clearTxBuff();

	// Tx part
	enum SEND_MODE{
		TURN = 0,
		VELOCITY,
		STATUS,
		JOYSTICK_X,
		JOYSTICK_Y
	};
	void sendMsg(SEND_MODE mode);
	uint8_t getSign_Tx(float value);
	uint16_t convertFloatToUint16t( float value);
	void convertToFrame_Tx(uint8_t sign, uint16_t value);
	void encode_frame_big_endian(uint8_t data_length);


	void hal_can_send(uint16_t frame_id, uint8_t dlc);
	CAN_FilterTypeDef hcan_filter;
	void hal_can_filter_init(void);

public:
	// RX part
	void init();
	void joystickSendProcess();


	void rewriteFrameProcess(uint32_t frame_id, uint8_t* data);
	void stopAllMotors();


	typedef struct{
		CAN_TxHeaderTypeDef     header;
		uint32_t 				mailbox;
		uint8_t				    data[8];
	}hal_can_messageTx;

	typedef struct{
		CAN_RxHeaderTypeDef	    header;
		uint32_t 				mailbox;
		uint8_t				    data[8];
	}hal_can_messageRx;

	hal_can_messageRx  canMsgRx;
	hal_can_messageTx  canMsgTx;

	CanManager();
	virtual ~CanManager();
};

extern CanManager canManager;

#endif /* CAN_CANMANAGER_H_ */
