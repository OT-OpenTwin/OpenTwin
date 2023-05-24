#include "OTGui/GuiTypes.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"

std::string ot::toString(ot::Orientation _orientation) {
	switch (_orientation)
	{
	case ot::Orientation::OrientCenter: return "Center";
	case ot::Orientation::OrientTop: return "Top";
	case ot::Orientation::OrientTopRight: return "TopRight";
	case ot::Orientation::OrientRight: return "Right";
	case ot::Orientation::OrientBottomRight: return "BottomRight";
	case ot::Orientation::OrientBottom: return "Bottom";
	case ot::Orientation::OrientBottomLeft: return "BottomLeft";
	case ot::Orientation::OrientLeft: return "Left";
	case ot::Orientation::OrientTopLeft: return "TopLeft";
	default:
		OT_LOG_EA("Unknown orientation provided");
		throw std::exception("Unknown orientation provided");
	}
}

ot::Orientation ot::stringToOrientation(const std::string& _string) {
	if (_string == toString(ot::Orientation::OrientCenter)) return ot::Orientation::OrientCenter;
	else if (_string == toString(ot::Orientation::OrientTop)) return ot::Orientation::OrientTop;
	else if (_string == toString(ot::Orientation::OrientTopRight)) return ot::Orientation::OrientTopRight;
	else if (_string == toString(ot::Orientation::OrientRight)) return ot::Orientation::OrientRight;
	else if (_string == toString(ot::Orientation::OrientBottomRight)) return ot::Orientation::OrientBottomRight;
	else if (_string == toString(ot::Orientation::OrientBottom)) return ot::Orientation::OrientBottom;
	else if (_string == toString(ot::Orientation::OrientBottomLeft)) return ot::Orientation::OrientBottomLeft;
	else if (_string == toString(ot::Orientation::OrientLeft)) return ot::Orientation::OrientLeft;
	else if (_string == toString(ot::Orientation::OrientTopLeft)) return ot::Orientation::OrientTopLeft;
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