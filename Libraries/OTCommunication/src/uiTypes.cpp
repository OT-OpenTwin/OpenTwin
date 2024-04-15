/*
 *	uiTypes.cpp
 *
 *  Created on: February 06, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 openTwin
 */

// OpenTwin header
#include "OTCommunication/UiTypes.h"

// std header
#include <cassert>

ot::LockTypeFlags ot::toLockTypeFlags(const std::vector<std::string> & _flags) {
	LockTypeFlags flags(NoLockFlags);
	for (const std::string& flag : _flags) {
		flags |= toLockTypeFlag(flag);
	}
	return flags;
}

ot::LockTypeFlags ot::toLockTypeFlags(const std::list<std::string>& _flags) {
	LockTypeFlags flags(NoLockFlags);
	for (const std::string& flag : _flags) {
		flags |= toLockTypeFlag(flag);
	}
	return flags;
}

ot::LockTypeFlag ot::toLockTypeFlag(const std::string& _flag) {
	LockTypeFlags f;
	if (_flag == toString(ot::LockViewWrite)) f |= ot::LockViewWrite;
	else if (_flag == toString(ot::LockViewRead)) f |= ot::LockViewRead;
	else if (_flag == toString(ot::LockModelWrite)) f |= ot::LockModelWrite;
	else if (_flag == toString(ot::LockModelRead)) f |= ot::LockModelRead;
	else if (_flag == toString(ot::LockAll)) f |= ot::LockAll;
	else if (_flag == toString(ot::LockProperties)) f |= ot::LockProperties;
	else if (_flag == toString(ot::LockNavigationWrite)) f |= ot::LockNavigationWrite;
	else if (_flag == toString(ot::LockNavigationAll)) f |= ot::LockNavigationAll;
	else if (_flag != toString(ot::NoLockFlags)) {
		OT_LOG_E("Unknown lock type flag \"" + _flag + "\"");
	}
	return ot::NoLockFlags;
}

std::list<std::string> ot::toStringList(const LockTypeFlags& _flags) {
	std::list<std::string> ret;

	if (_flags & LockViewWrite) ret.push_back(toString(LockViewWrite));
	if (_flags & LockViewRead) ret.push_back(toString(LockViewRead));
	if (_flags & LockModelWrite) ret.push_back(toString(LockModelWrite));
	if (_flags & LockModelRead) ret.push_back(toString(LockModelRead));
	if (_flags & LockProperties) ret.push_back(toString(LockProperties));
	if (_flags & LockNavigationWrite) ret.push_back(toString(LockNavigationWrite));
	if (_flags & LockNavigationAll) ret.push_back(toString(LockNavigationAll));
	if (_flags & LockAll) ret.push_back(toString(LockAll));

	return ret;
}

std::string ot::toString(LockTypeFlag _type) {
	switch (_type)
	{
	case ot::NoLockFlags: return "None";
	case ot::LockViewWrite: return "LockViewWrite";
	case ot::LockViewRead: return "LockViewRead";
	case ot::LockModelWrite: return "LockModelWrite";
	case ot::LockModelRead: return "LockModelRead";
	case ot::LockProperties: return "LockProperties";
	case ot::LockNavigationWrite: return "LockNavigationWrite";
	case ot::LockNavigationAll: return "LockNavigationAll";
	case ot::LockAll: return "LockAll";
	default:
		OT_LOG_E("Unknown lock type (" + std::to_string((int)_type) + ")");
		return "None";
	}
}
