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


#define IDLE_TIMER htim4
#define PROCESS_TIMER htim3

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

		void init();
		uint8_t turnPermission(MSG_ORIGIN origin);
		uint8_t velocityPermission(MSG_ORIGIN origin);
		void statusUpdate(RC_MODE RCstatus, DRIVE_MODE drivestatus);
		DRIVE_MODE getDriveMode();
		RC_MODE getRCmode();

		void stopIdleTimer();

		uint8_t isJoystickMode();

		ModeManager();
		virtual ~ModeManager();

	private:

		float joystickProcess();
		void startIdleTimer();
		void resetIdleTimer();


		uint8_t isIdleTimerON;
		DRIVE_MODE driveMode;
		RC_MODE RCmode;

};

extern ModeManager modeManager;



#endif /* SRC_MODEMANAGER_H_ */
