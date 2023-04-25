/*
 *	uiTypes.cpp
 *
 *  Created on: February 06, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2021 openTwin
 */

// OpenTwin header
#include "OpenTwinCommunication/UiTypes.h"

// std header
#include <cassert>

ot::Flags<ot::ui::lockType> ot::ui::toLockType(const std::vector<std::string> & _items) {
	ot::Flags<ot::ui::lockType> f;
	for (auto itm : _items) {
		if (itm == toString(lockType::tlAll)) {
			f.setFlag(lockType::tlAll);
		} else if (itm == toString(lockType::tlModelWrite)) {
			f.setFlag(lockType::tlModelWrite);
		}
		else if (itm == toString(lockType::tlModelRead)) {
			f.setFlag(lockType::tlModelRead);
		}
		else if (itm == toString(lockType::tlNavigationAll)) {
			f.setFlag(lockType::tlNavigationAll);
		}
		else if (itm == toString(lockType::tlNavigationWrite)) {
			f.setFlag(lockType::tlNavigationWrite);
		}
		else if (itm == toString(lockType::tlProperties)) {
			f.setFlag(lockType::tlProperties);
		}
		else if (itm == toString(lockType::tlViewRead)) {
			f.setFlag(lockType::tlViewRead);
		}
		else if (itm == toString(lockType::tlViewWrite)) {
			f.setFlag(lockType::tlViewWrite);
		}
		else {
			assert(0);	// Invalid type
		}
	}
	return f;
}

ot::Flags<ot::ui::lockType> ot::ui::toLockType(const std::list<std::string>& _items) {
	ot::Flags<ot::ui::lockType> f;
	for (auto itm : _items) {
		if (itm == toString(lockType::tlAll)) {
			f.setFlag(lockType::tlAll);
		}
		else if (itm == toString(lockType::tlModelWrite)) {
			f.setFlag(lockType::tlModelWrite);
		}
		else if (itm == toString(lockType::tlModelRead)) {
			f.setFlag(lockType::tlModelRead);
		}
		else if (itm == toString(lockType::tlNavigationAll)) {
			f.setFlag(lockType::tlNavigationAll);
		}
		else if (itm == toString(lockType::tlNavigationWrite)) {
			f.setFlag(lockType::tlNavigationWrite);
		}
		else if (itm == toString(lockType::tlProperties)) {
			f.setFlag(lockType::tlProperties);
		}
		else if (itm == toString(lockType::tlViewRead)) {
			f.setFlag(lockType::tlViewRead);
		}
		else if (itm == toString(lockType::tlViewWrite)) {
			f.setFlag(lockType::tlViewWrite);
		}
		else {
			assert(0);	// Invalid type
			throw std::exception("Invalid lockType");
		}
	}
	return f;
}

std::list<std::string> ot::ui::toList(const ot::Flags<lockType> & _flags) {
	std::list<std::string> ret;

	if (_flags.flagIsSet(ot::ui::lockType::tlAll)) {
		ret.push_back(toString(lockType::tlAll));
	}
	if (_flags.flagIsSet(ot::ui::lockType::tlModelWrite)) {
		ret.push_back(toString(lockType::tlModelWrite));
	}
	if (_flags.flagIsSet(ot::ui::lockType::tlModelRead)) {
		ret.push_back(toString(lockType::tlModelRead));
	}
	if (_flags.flagIsSet(ot::ui::lockType::tlNavigationAll)) {
		ret.push_back(toString(lockType::tlNavigationAll));
	}
	if (_flags.flagIsSet(ot::ui::lockType::tlNavigationWrite)) {
		ret.push_back(toString(lockType::tlNavigationWrite));
	}
	if (_flags.flagIsSet(ot::ui::lockType::tlProperties)) {
		ret.push_back(toString(lockType::tlProperties));
	}
	if (_flags.flagIsSet(ot::ui::lockType::tlViewRead)) {
		ret.push_back(toString(lockType::tlViewRead));
	}
	if (_flags.flagIsSet(ot::ui::lockType::tlViewWrite)) {
		ret.push_back(toString(lockType::tlViewWrite));
	}
	return ret;
}

std::string ot::ui::toString(lockType _type) {
	switch (_type)
	{
	case ot::ui::tlViewWrite: return "tlViewWrite";
	case ot::ui::tlViewRead: return "tlViewRead";
	case ot::ui::tlModelWrite: return "tlModelWrite";
	case ot::ui::tlModelRead: return "tlModeRead";
	case ot::ui::tlAll: return "tlAll";
	case ot::ui::tlProperties: return "tlProperties";
	case ot::ui::tlNavigationWrite: return "tlNavigationWrite";
	case ot::ui::tlNavigationAll: return "tlNavigationAll";
	default:
		assert(0); // Invalid type
		return "";
	}
}

ot::ui::keyType ot::ui::toKeyType(const std::string& _key) {
	for (int i = 0; i <= (int)Key_None; i++) {
		if (toString((keyType)i) == _key) { return (keyType)i; }
	}
	assert(0);
	return Key_None;
}


std::string ot::ui::toString(keyType _key) {
	switch (_key)
	{
	case ot::ui::Key_A: return "A";
	case ot::ui::Key_B: return "B";
	case ot::ui::Key_C: return "C";
	case ot::ui::Key_D: return "D";
	case ot::ui::Key_E: return "E";
	case ot::ui::Key_F: return "F";
	case ot::ui::Key_G: return "G";
	case ot::ui::Key_H: return "H";
	case ot::ui::Key_I: return "I";
	case ot::ui::Key_J: return "J";
	case ot::ui::Key_K: return "K";
	case ot::ui::Key_L: return "L";
	case ot::ui::Key_M: return "M";
	case ot::ui::Key_N: return "N";
	case ot::ui::Key_O: return "O";
	case ot::ui::Key_P: return "P";
	case ot::ui::Key_Q: return "Q";
	case ot::ui::Key_R: return "R";
	case ot::ui::Key_S: return "S";
	case ot::ui::Key_T: return "T";
	case ot::ui::Key_U: return "U";
	case ot::ui::Key_V: return "V";
	case ot::ui::Key_W: return "W";
	case ot::ui::Key_X: return "X";
	case ot::ui::Key_Y: return "Y";
	case ot::ui::Key_Z: return "Z";
	case ot::ui::Key_Alt: return "Alt";
	case ot::ui::Key_Shift: return "Shift";
	case ot::ui::Key_Control: return "Ctrl";
	case ot::ui::Key_CapsLock: return "CapsLock";
	case ot::ui::Key_1: return "1";
	case ot::ui::Key_2: return "2";
	case ot::ui::Key_3: return "3";
	case ot::ui::Key_4: return "4";
	case ot::ui::Key_5: return "5";
	case ot::ui::Key_6: return "6";
	case ot::ui::Key_7: return "7";
	case ot::ui::Key_8: return "8";
	case ot::ui::Key_9: return "9";
	case ot::ui::Key_0: return "0";
	case ot::ui::Key_Tab: return "Tab";
	case ot::ui::Key_Return: return "Return";
	case ot::ui::Key_Space: return "Space";
	case ot::ui::Key_Backspace: return "Backspace";
	case ot::ui::Key_Delete: return "Del";
	case ot::ui::Key_ArrowUp: return "Up";
	case ot::ui::Key_ArrowDown: return "Down";
	case ot::ui::Key_ArrowLeft: return "Left";
	case ot::ui::Key_ArrowRight: return "Right";
	case ot::ui::Key_F1: return "F1";
	case ot::ui::Key_F2: return "F2";
	case ot::ui::Key_F3: return "F3";
	case ot::ui::Key_F4: return "F4";
	case ot::ui::Key_F5: return "F5";
	case ot::ui::Key_F6: return "F6";
	case ot::ui::Key_F7: return "F7";
	case ot::ui::Key_F8: return "F8";
	case ot::ui::Key_F9: return "F9";
	case ot::ui::Key_F10: return "F10";
	case ot::ui::Key_F11: return "F11";
	case ot::ui::Key_F12: return "F12";
	case ot::ui::Key_None: return "";
	default:
		assert(0);
		return "Key_None";
	}
}

std::string ot::ui::keySequenceToString(keyType _key1, keyType _key2, keyType _key3, keyType _key4) {
	std::string ret{ toString(_key1) };
	if (_key2 != Key_None) { ret.append("+").append(toString(_key2)); }
	if (_key3 != Key_None) { ret.append("+").append(toString(_key3)); }
	if (_key4 != Key_None) { ret.append("+").append(toString(_key4)); }
	return ret;
}