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

	//! Describes the object type to lock in the UI
	enum LockTypeFlag {
		NoLockFlags         = 0x0000,
		LockViewWrite       = 0x0001,
		LockViewRead        = 0x0002,
		LockModelWrite      = 0x0004,
		LockModelRead       = 0x0008,
		LockProperties      = 0x0010,
		LockNavigationWrite = 0x0020,
		LockNavigationAll   = 0x0040,
		LockAll             = 0x1000,
	};

	typedef ot::Flags<LockTypeFlag> LockTypeFlags;

	OT_COMMUNICATION_API_EXPORT LockTypeFlags toLockTypeFlags(const std::vector<std::string>& _flags);

	OT_COMMUNICATION_API_EXPORT LockTypeFlags toLockTypeFlags(const std::list<std::string>& _flags);

	OT_COMMUNICATION_API_EXPORT LockTypeFlag toLockTypeFlag(const std::string& _flag);

	OT_COMMUNICATION_API_EXPORT std::list<std::string> toStringList(const LockTypeFlags& _flags);

	OT_COMMUNICATION_API_EXPORT std::string toString(LockTypeFlag _type);

}

OT_ADD_FLAG_FUNCTIONS(ot::LockTypeFlag);
