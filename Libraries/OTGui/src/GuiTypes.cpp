#include "OTGui/GuiTypes.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"

std::string ot::toString(ot::Alignment _alignment) {
	switch (_alignment)
	{
	case ot::Alignment::AlignCenter: return "AlignCenter";
	case ot::Alignment::AlignTop: return "AlignTop";
	case ot::Alignment::AlignTopRight: return "AlignTopRight";
	case ot::Alignment::AlignRight: return "AlignRight";
	case ot::Alignment::AlignBottomRight: return "AlignBottomRight";
	case ot::Alignment::AlignBottom: return "AlignBottom";
	case ot::Alignment::AlignBottomLeft: return "AlignBottomLeft";
	case ot::Alignment::AlignLeft: return "AlignLeft";
	case ot::Alignment::AlignTopLeft: return "AlignTopLeft";
	default:
		OT_LOG_EAS("Unknown Alignment provided: \"" + std::to_string((int)_alignment) + "\"");
		throw std::exception("Unknown Alignment provided");
	}
}

ot::Alignment ot::stringToAlignment(const std::string& _string) {
	if (_string == toString(ot::Alignment::AlignCenter)) return ot::Alignment::AlignCenter;
	else if (_string == toString(ot::Alignment::AlignTop)) return ot::Alignment::AlignTop;
	else if (_string == toString(ot::Alignment::AlignTopRight)) return ot::Alignment::AlignTopRight;
	else if (_string == toString(ot::Alignment::AlignRight)) return ot::Alignment::AlignRight;
	else if (_string == toString(ot::Alignment::AlignBottomRight)) return ot::Alignment::AlignBottomRight;
	else if (_string == toString(ot::Alignment::AlignBottom)) return ot::Alignment::AlignBottom;
	else if (_string == toString(ot::Alignment::AlignBottomLeft)) return ot::Alignment::AlignBottomLeft;
	else if (_string == toString(ot::Alignment::AlignLeft)) return ot::Alignment::AlignLeft;
	else if (_string == toString(ot::Alignment::AlignTopLeft)) return ot::Alignment::AlignTopLeft;
	else {
		OT_LOG_EAS("Unknown Alignment provided: \"" + _string + "\"");
		throw std::exception("Unknown Alignment provided");
	}
}

std::string ot::toString(Orientation _orientation) {
	switch (_orientation)
	{
	case ot::Horizontal: return "Horizontal";
	case ot::Vertical: return "Vertical";
	default:
		OT_LOG_EAS("Unknown Orientation provided: \"" + std::to_string((int)_orientation) + "\"");
		throw std::exception("Unknown Orientation provided");
	}
}

ot::Orientation ot::stringToOrientation(const std::string& _string) {
	if (_string == toString(ot::Orientation::Horizontal)) return ot::Orientation::Horizontal;
	else if (_string == toString(ot::Orientation::Vertical)) return ot::Orientation::Vertical;
	else {
		OT_LOG_EAS("Unknown Orientation provided: \"" + _string + "\"");
		throw std::exception("Unknown Orientation provided");
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
		OT_LOG_EAS("Unknown font family provided: \"" + _string + "\"");
		throw std::exception("Unknown font family provided");
	}
}

std::string ot::toString(SizePolicy _policy) {
	switch (_policy)
	{
	case ot::Preferred: return "Preferred";
	case ot::Dynamic: return "Dynamic";
	default:
		OT_LOG_EA("Unknown size policy");
		throw std::exception("Unknown size policy");
	}
}

ot::SizePolicy ot::stringToSizePolicy(const std::string& _string) {
	if (_string == toString(ot::Preferred)) { return ot::Preferred; }
	else if (_string == toString(ot::Dynamic)) { return ot::Dynamic; }
	else {
		OT_LOG_EAS("Unknown size policy \"" + _string + "\"");
		throw std::exception("Unknown size policy");
	}
}

std::string ot::toString(ot::ConnectionDirection _direction) {
	switch (_direction)
	{
	case ot::ConnectAny: return "Any";
	case ot::ConnectLeft: return "Left";
	case ot::ConnectUp: return "Up";
	case ot::ConnectRight: return "Right";
	case ot::ConnectDown: return "Down";
	default:
		OT_LOG_EA("Unknown connection direction");
		throw std::exception("Unknown connection direction");
	}
}

ot::ConnectionDirection ot::stringToConnectionDirection(const std::string& _direction) {
	if (_direction == toString(ot::ConnectAny)) return ot::ConnectAny;
	else if (_direction == toString(ot::ConnectLeft)) return ot::ConnectLeft;
	else if (_direction == toString(ot::ConnectUp)) return ot::ConnectUp;
	else if (_direction == toString(ot::ConnectRight)) return ot::ConnectRight;
	else if (_direction == toString(ot::ConnectDown)) return ot::ConnectDown;
	else {
		OT_LOG_EAS("Unknown connection direction \"" + _direction + "\"");
		throw std::exception("Unknown connection direction");
	}
}