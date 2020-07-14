/*
 * ModeManager.h
 *
 *  Created on: 27.03.2020
 *      Author: Igor
 */

#ifndef SRC_MODEMANAGER_H_
#define SRC_MODEMANAGER_H_

#include "main.h"
#include "tim.h"
#include "stm32f1xx_hal_tim.h"


#define IDLE_TIMER htim6
#define IGNORE_VALUE 101.f

class ModeManager {

	public:

		enum RC_MODE{
			MODE_JOYSICK = 0,
			MODE_AUTONOMOUS,
			MODE_SEMI,
			MODE_ACRO
		};

		enum DRIVE_MODE{
			DISABLE = 0,
			ENABLE
		};

		enum MSG_ORIGIN{
			RC = 0,
			I3
		};

		float setVelocity(float vel, MSG_ORIGIN origin);
		float setTurn(float turn,MSG_ORIGIN origin);
		void statusUpdate(RC_MODE RCstatus, DRIVE_MODE drivestatus);
		DRIVE_MODE getDriveMode();
		RC_MODE getRCmode();

		uint8_t isJoystickMode();

		ModeManager();
		virtual ~ModeManager();

	private:
//		void sendVelocity(float);
//		void sendTurn(float);
//		void stopAll();
		float joystickProcess();
		void startIdleTimer();
		void stopIdleTimer();
		void resetIdleTimer();
//		void process();


		DRIVE_MODE driveMode = DISABLE;
		RC_MODE RCmode = MODE_JOYSICK;

};

extern ModeManager modeManager;



#endif /* SRC_MODEMANAGER_H_ */
