#include "OTGui/GuiTypes.h"
#include "OpenTwinCore/otAssert.h"

std::string ot::toString(ot::Orientation _orientation) {
	switch (_orientation)
	{
	case ot::Orientation::Center: return "Center";
	case ot::Orientation::Top: return "Top";
	case ot::Orientation::TopRight: return "TopRight";
	case ot::Orientation::Right: return "Right";
	case ot::Orientation::BottomRight: return "BottomRight";
	case ot::Orientation::Bottom: return "Bottom";
	case ot::Orientation::BottomLeft: return "BottomLeft";
	case ot::Orientation::Left: return "Left";
	case ot::Orientation::TopLeft: return "TopLeft";
	default:
		otAssert(0, "Unknown orientation provided");
		throw std::exception("Unknown orientation provided");
	}
}

ot::Orientation ot::stringToOrientation(const std::string& _string) {
	if (_string == toString(ot::Orientation::Center)) return ot::Orientation::Center;
	else if (_string == toString(ot::Orientation::Top)) return ot::Orientation::Top;
	else if (_string == toString(ot::Orientation::TopRight)) return ot::Orientation::TopRight;
	else if (_string == toString(ot::Orientation::Right)) return ot::Orientation::Right;
	else if (_string == toString(ot::Orientation::BottomRight)) return ot::Orientation::BottomRight;
	else if (_string == toString(ot::Orientation::Bottom)) return ot::Orientation::Bottom;
	else if (_string == toString(ot::Orientation::BottomLeft)) return ot::Orientation::BottomLeft;
	else if (_string == toString(ot::Orientation::Left)) return ot::Orientation::Left;
	else if (_string == toString(ot::Orientation::TopLeft)) return ot::Orientation::TopLeft;
	else {
		otAssert(0, "Unknown orientation provided");
		throw std::exception("Unknown orientation provided");
	}
}