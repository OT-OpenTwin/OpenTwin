#include "OTGui/GuiTypes.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"

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
		OT_LOG_EA("Unknown orientation provided");
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
		OT_LOG_EA("Unknown orientation provided");
		throw std::exception("Unknown orientation provided");
	}
}

std::string ot::toString(ot::FontFamily _fontFamily) {
	switch (_fontFamily)
	{
	case ot::Arial: return "Arial";
	case ot::ComicSansMS: return "ComicSansMS";
	case ot::Consolas: return "Consolas";
	case ot::CourierNew: return "CourierNew";
	case ot::Georgia: return "Georgia";
	case ot::Helvetica: return "Helvetica";
	case ot::Impact: return "Impact";
	case ot::Palatino: return "Palatino";
	case ot::TimesNewRoman: return "TimesNewRoman";
	case ot::TrebuchetMS: return "TrebuchetMS";
	case ot::Verdana: return "Verdana";
	default:
		OT_LOG_EA("Unknown font family provided");
		throw std::exception("Unknown font family provided");
	}
}

ot::FontFamily ot::stringToFontFamily(const std::string& _string) {
	if (_string == toString(Arial)) { return Arial; }
	else if (_string == toString(ComicSansMS)) { return ComicSansMS; }
	else if (_string == toString(Consolas)) { return Consolas; }
	else if (_string == toString(CourierNew)) { return CourierNew; }
	else if (_string == toString(Georgia)) { return Georgia; }
	else if (_string == toString(Helvetica)) { return Helvetica; }
	else if (_string == toString(Impact)) { return Impact; }
	else if (_string == toString(Palatino)) { return Palatino; }
	else if (_string == toString(TimesNewRoman)) { return TimesNewRoman; }
	else if (_string == toString(TrebuchetMS)) { return TrebuchetMS; }
	else if (_string == toString(Verdana)) { return Verdana; }
	else {
		OT_LOG_EA("Unknown font family provided");
		throw std::exception("Unknown font family provided");
	}
}