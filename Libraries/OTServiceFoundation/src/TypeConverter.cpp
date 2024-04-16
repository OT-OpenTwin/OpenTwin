#include "OTServiceFoundation/TypeConverter.h"
#include "OTCore/OTAssert.h"

std::string ot::convert::toString(ItemCheckedState _flag) {
	switch (_flag)
	{
	case ot::NoCheckedState: return "NoCheckedState";
	case ot::Checked: return "Checked";
	case ot::Unchecked: return "Unchecked";
	case ot::PartiallyChecked: return "PartiallyChecked";
	case ot::itemCheckedStateMask: return "Mask";
	default:
		OTAssert(0, "Unknown flag");
		return "NoCheckedState";
	}
}

std::string ot::convert::toString(ItemFlag _flag) {
	switch (_flag)
	{
	case ot::NoFlags: return "NoFlag";
	case ot::ItemIsCheckable: return "IsCheckable";
	case ot::ItemIsVisible: return "IsVisible";
	case ot::itemFlagMask: return "Mask";
	default: 
		OTAssert(0, "Unknown flag");
		return "NoFlag";
	}
}

std::list<std::string> ot::convert::toString(ItemCheckedStateFlags _flags) {
	unsigned long long check = 1;
	std::list<std::string> ret;
	while (check < itemCheckedStateMask) {
		if (_flags.flagIsSet((ItemCheckedState)check)) {
			ret.push_back(toString((ItemCheckedState)check));
		}
		check *= 2;
	}
	return ret;
}

std::list<std::string> ot::convert::toString(ItemFlags _flags) {
	unsigned long long check = 1;
	std::list<std::string> ret;
	while (check < itemFlagMask) {
		if (_flags.flagIsSet((ItemFlag)check)) {
			ret.push_back(toString((ItemFlag)check));
		}
		check *= 2;
	}
	return ret;
}

ot::ItemCheckedState ot::convert::toItemCheckedState(const std::string& _flags) {
	if (_flags == toString(NoCheckedState)) { return NoCheckedState; }
	for (ItemCheckedState s{ (ItemCheckedState)1 }; s < itemCheckedStateMask; s = (ItemCheckedState)(s * 2)) {
		if (_flags == toString(s)) { return s; }
	}
	OTAssert(0, "Unknown flag");
	return NoCheckedState;
}

ot::ItemFlag ot::convert::toItemFlag(const std::string& _flags) {
	if (_flags == toString(NoFlags)) { return NoFlags; }
	for (ItemFlag s{ (ItemFlag)1 }; s < itemFlagMask; s = (ItemFlag)(s * 2)) {
		if (_flags == toString(s)) { return s; }
	}
	OTAssert(0, "Unknown flag");
	return NoFlags;
}

ot::ItemCheckedStateFlags ot::convert::toItemCheckedStateFlags(const std::list<std::string>& _flags) {
	ItemCheckedStateFlags ret(NoCheckedState);
	for (auto f : _flags) { ret.setFlag(toItemCheckedState(f)); }
	return ret;
}

ot::ItemFlags ot::convert::toItemFlags(const std::list<std::string>& _flags) {
	ItemFlags ret(NoFlags);
	for (const auto& f : _flags) { ret.setFlag(toItemFlag(f)); }
	return ret;
}