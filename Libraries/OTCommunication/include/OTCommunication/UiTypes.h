/*
 *	uiTypes.h
 *
 *  Created on: February 06, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 openTwin
 */

#pragma once

#include <string>
#include <vector>
#include <list>

#include "OTCore/Flags.h"
#include "OTCommunication/CommunicationAPIExport.h"
#include "OTCommunication/CommunicationTypes.h"

namespace ot {
	
	namespace ui {
		/*
		//! Describes the locking group an UI object belongs to
		enum lockableType {
			ltViewWrite		= 0x01,
			ltViewRead		= 0x02,
			ltModelWrite	= 0x04,
			ltModelRead		= 0x08
		};
		*/

		enum keyType {
			Key_A,
			Key_B,
			Key_C,
			Key_D,
			Key_E,
			Key_F,
			Key_G,
			Key_H,
			Key_I,
			Key_J,
			Key_K,
			Key_L,
			Key_M,
			Key_N,
			Key_O,
			Key_P,
			Key_Q,
			Key_R,
			Key_S,
			Key_T,
			Key_U,
			Key_V,
			Key_W,
			Key_X,
			Key_Y,
			Key_Z,
			Key_Alt,
			Key_Shift,
			Key_Control,
			Key_CapsLock,
			Key_1,
			Key_2,
			Key_3,
			Key_4,
			Key_5,
			Key_6,
			Key_7,
			Key_8,
			Key_9,
			Key_0,
			Key_Tab,
			Key_Return,
			Key_Space,
			Key_Backspace,
			Key_Delete,
			Key_ArrowUp,
			Key_ArrowDown,
			Key_ArrowLeft,
			Key_ArrowRight,
			Key_F1,
			Key_F2,
			Key_F3,
			Key_F4,
			Key_F5,
			Key_F6,
			Key_F7,
			Key_F8,
			Key_F9,
			Key_F10,
			Key_F11,
			Key_F12,
			Key_None	// Key none must be the last item in this enum
		};
		
		//! Describes the object type to lock in the UI
		enum lockType {
			tlViewWrite			= 0x01,
			tlViewRead			= 0x02,
			tlModelWrite		= 0x04,
			tlModelRead			= 0x08,

			tlAll				= 0x1000,
			tlProperties		= 0x2000,
			tlNavigationWrite	= 0x4000,
			tlNavigationAll		= 0x8000
		};

		OT_COMMUNICATION_API_EXPORT ot::Flags<lockType> toLockType(const std::vector<std::string> & _items);

		OT_COMMUNICATION_API_EXPORT ot::Flags<lockType> toLockType(const std::list<std::string> & _items);

		OT_COMMUNICATION_API_EXPORT std::list<std::string> toList(const ot::Flags<lockType> & _flags);

		OT_COMMUNICATION_API_EXPORT std::string toString(lockType _type);

		//

		OT_COMMUNICATION_API_EXPORT keyType toKeyType(const std::string& _key);

		OT_COMMUNICATION_API_EXPORT std::string toString(keyType _key);

		OT_COMMUNICATION_API_EXPORT std::string keySequenceToString(keyType _key1, keyType _key2 = Key_None, keyType _key3 = Key_None, keyType _key4 = Key_None);
	}

}

OT_ADD_FLAG_FUNCTIONS(ot::ui::lockType);
