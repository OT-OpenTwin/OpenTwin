#include "OpenTwinFoundation/TypeConverter.h"
#include "OpenTwinCore/otAssert.h"

std::string ot::convert::toString(itemCheckedState _flag) {
	switch (_flag)
	{
	case ot::NoCheckedState: return "NoCheckedState";
	case ot::Checked: return "Checked";
	case ot::Unchecked: return "Unchecked";
	case ot::PartiallyChecked: return "PartiallyChecked";
	case ot::itemCheckedStateMask: return "Mask";
	default:
		otAssert(0, "Unknown flag");
		return "NoCheckedState";
	}
}

std::string ot::convert::toString(itemFlag _flag) {
	switch (_flag)
	{
	case ot::NoFlags: return "NoFlag";
	case ot::ItemIsCheckable: return "IsCheckable";
	case ot::ItemIsVisible: return "IsVisible";
	case ot::itemFlagMask: return "Mask";
	default: 
		otAssert(0, "Unknown flag");
		return "NoFlag";
	}
}

std::list<std::string> ot::convert::toString(const ot::Flags<itemCheckedState>& _flags) {
	unsigned long long check = 1;
	std::list<std::string> ret;
	while (check < itemCheckedStateMask) {
		if (_flags.flagIsSet((itemCheckedState)check)) {
			ret.push_back(toString((itemCheckedState)check));
		}
		check *= 2;
	}
	return ret;
}

std::list<std::string> ot::convert::toString(const ot::Flags<itemFlag>& _flags) {
	unsigned long long check = 1;
	std::list<std::string> ret;
	while (check < itemFlagMask) {
		if (_flags.flagIsSet((itemFlag)check)) {
			ret.push_back(toString((itemFlag)check));
		}
		check *= 2;
	}
	return ret;
}

ot::itemCheckedState ot::convert::toItemCheckedState(const std::string& _flags) {
	if (_flags == toString(NoCheckedState)) { return NoCheckedState; }
	for (itemCheckedState s{ (itemCheckedState)1 }; s < itemCheckedStateMask; s = (itemCheckedState)(s * 2)) {
		if (_flags == toString(s)) { return s; }
	}
	otAssert(0, "Unknown flag");
	return NoCheckedState;
}

ot::itemFlag ot::convert::toItemFlags(const std::string& _flags) {
	if (_flags == toString(NoFlags)) { return NoFlags; }
	for (itemFlag s{ (itemFlag)1 }; s < itemFlagMask; s = (itemFlag)(s * 2)) {
		if (_flags == toString(s)) { return s; }
	}
	otAssert(0, "Unknown flag");
	return NoFlags;
}

ot::Flags<ot::itemCheckedState> ot::convert::toItemCheckedState(const std::list<std::string>& _flags) {
	ot::Flags<itemCheckedState> ret(NoCheckedState);
	for (auto f : _flags) { ret.setFlag(toItemCheckedState(f)); }
	return ret;
}

ot::Flags<ot::itemFlag> ot::convert::toItemFlags(const std::list<std::string>& _flags) {
	ot::Flags<itemFlag> ret(NoFlags);
	for (auto f : _flags) { ret.setFlag(toItemFlags(f)); }
	return ret;
}