//! @file GuiTypes.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "OTCore/LogDispatcher.h"
#include "OTCore/ContainerHelper.h"
#include "OTGui/GuiTypes.h"

// ###########################################################################################################################################################################################################################################################################################################################

// Alignment

std::string ot::toString(ot::Alignment _alignment) {
	switch (_alignment)
	{
	case Alignment::Center: return "AlignCenter";
	case Alignment::Top: return "AlignTop";
	case Alignment::TopRight: return "AlignTopRight";
	case Alignment::Right: return "AlignRight";
	case Alignment::BottomRight: return "AlignBottomRight";
	case Alignment::Bottom: return "AlignBottom";
	case Alignment::BottomLeft: return "AlignBottomLeft";
	case Alignment::Left: return "AlignLeft";
	case Alignment::TopLeft: return "AlignTopLeft";
	default:
		OT_LOG_EAS("Unknown Alignment provided: \"" + std::to_string((int)_alignment) + "\"");
		throw std::exception("Unknown Alignment provided");
	}
}

ot::Alignment ot::stringToAlignment(const std::string& _string) {
	if (_string == toString(Alignment::Center)) return Alignment::Center;
	else if (_string == toString(Alignment::Top)) return Alignment::Top;
	else if (_string == toString(Alignment::TopRight)) return Alignment::TopRight;
	else if (_string == toString(Alignment::Right)) return Alignment::Right;
	else if (_string == toString(Alignment::BottomRight)) return Alignment::BottomRight;
	else if (_string == toString(Alignment::Bottom)) return Alignment::Bottom;
	else if (_string == toString(Alignment::BottomLeft)) return Alignment::BottomLeft;
	else if (_string == toString(Alignment::Left)) return Alignment::Left;
	else if (_string == toString(Alignment::TopLeft)) return Alignment::TopLeft;
	else {
		OT_LOG_EAS("Unknown Alignment provided: \"" + _string + "\"");
		throw std::exception("Unknown Alignment provided");
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Orientation

std::string ot::toString(Orientation _orientation) {
	switch (_orientation)
	{
	case Orientation::Horizontal: return "Horizontal";
	case Orientation::Vertical: return "Vertical";
	default:
		OT_LOG_EAS("Unknown Orientation provided: \"" + std::to_string((int)_orientation) + "\"");
		throw std::exception("Unknown Orientation provided");
	}
}

ot::Orientation ot::stringToOrientation(const std::string& _string) {
	if (_string == toString(Orientation::Horizontal)) return Orientation::Horizontal;
	else if (_string == toString(Orientation::Vertical)) return Orientation::Vertical;
	else {
		OT_LOG_EAS("Unknown Orientation provided: \"" + _string + "\"");
		throw std::exception("Unknown Orientation provided");
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Font Family

std::string ot::toString(FontFamily _fontFamily) {
	switch (_fontFamily)
	{
	case FontFamily::Arial: return "Arial";
	case FontFamily::ComicSansMS: return "ComicSansMS";
	case FontFamily::Consolas: return "Consolas";
	case FontFamily::CourierNew: return "CourierNew";
	case FontFamily::Georgia: return "Georgia";
	case FontFamily::Helvetica: return "Helvetica";
	case FontFamily::Impact: return "Impact";
	case FontFamily::Palatino: return "Palatino";
	case FontFamily::TimesNewRoman: return "TimesNewRoman";
	case FontFamily::TrebuchetMS: return "TrebuchetMS";
	case FontFamily::Verdana: return "Verdana";
	default:
		OT_LOG_EA("Unknown font family provided");
		throw std::exception("Unknown font family provided");
	}
}

ot::FontFamily ot::stringToFontFamily(const std::string& _string) {
	if (_string == toString(FontFamily::Arial)) { return FontFamily::Arial; }
	else if (_string == toString(FontFamily::ComicSansMS)) { return FontFamily::ComicSansMS; }
	else if (_string == toString(FontFamily::Consolas)) { return FontFamily::Consolas; }
	else if (_string == toString(FontFamily::CourierNew)) { return FontFamily::CourierNew; }
	else if (_string == toString(FontFamily::Georgia)) { return FontFamily::Georgia; }
	else if (_string == toString(FontFamily::Helvetica)) { return FontFamily::Helvetica; }
	else if (_string == toString(FontFamily::Impact)) { return FontFamily::Impact; }
	else if (_string == toString(FontFamily::Palatino)) { return FontFamily::Palatino; }
	else if (_string == toString(FontFamily::TimesNewRoman)) { return FontFamily::TimesNewRoman; }
	else if (_string == toString(FontFamily::TrebuchetMS)) { return FontFamily::TrebuchetMS; }
	else if (_string == toString(FontFamily::Verdana)) { return FontFamily::Verdana; }
	else {
		OT_LOG_EAS("Unknown font family provided: \"" + _string + "\"");
		throw std::exception("Unknown font family provided");
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Size Policy

std::string ot::toString(SizePolicy _policy) {
	switch (_policy)
	{
	case SizePolicy::Preferred: return "Preferred";
	case SizePolicy::Dynamic: return "Dynamic";
	default:
		OT_LOG_EA("Unknown size policy");
		throw std::exception("Unknown size policy");
	}
}

ot::SizePolicy ot::stringToSizePolicy(const std::string& _string) {
	if (_string == toString(SizePolicy::Preferred)) { return SizePolicy::Preferred; }
	else if (_string == toString(SizePolicy::Dynamic)) { return SizePolicy::Dynamic; }
	else {
		OT_LOG_EAS("Unknown size policy \"" + _string + "\"");
		throw std::exception("Unknown size policy");
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Connections

std::string ot::toString(ConnectionDirection _direction) {
	switch (_direction)
	{
	case ConnectionDirection::Any: return "Any";
	case ConnectionDirection::Left: return "Left";
	case ConnectionDirection::Up: return "Up";
	case ConnectionDirection::Right: return "Right";
	case ConnectionDirection::Down: return "Down";
	case ConnectionDirection::Out: return "Out";
	case ConnectionDirection::In: return "In";
	default:
		OT_LOG_EA("Unknown connection direction");
		throw std::exception("Unknown connection direction");
	}
}

ot::ConnectionDirection ot::stringToConnectionDirection(const std::string& _direction) {
	if (_direction == toString(ConnectionDirection::Any)) return ConnectionDirection::Any;
	else if (_direction == toString(ConnectionDirection::Left)) return ConnectionDirection::Left;
	else if (_direction == toString(ConnectionDirection::Up)) return ConnectionDirection::Up;
	else if (_direction == toString(ConnectionDirection::Right)) return ConnectionDirection::Right;
	else if (_direction == toString(ConnectionDirection::Down)) return ConnectionDirection::Down;
	else if (_direction == toString(ConnectionDirection::Out)) return ConnectionDirection::Out;
	else if (_direction == toString(ConnectionDirection::In)) return ConnectionDirection::In;
	else {
		OT_LOG_EAS("Unknown connection direction \"" + _direction + "\"");
		throw std::exception("Unknown connection direction");
	}
}

ot::ConnectionDirection ot::inversedConnectionDirection(ConnectionDirection _direction) {
	switch (_direction)
	{
	case ConnectionDirection::Any: return ConnectionDirection::Any;
	case ConnectionDirection::Left: return ConnectionDirection::Right;
	case ConnectionDirection::Up: return ConnectionDirection::Down;
	case ConnectionDirection::Right: return ConnectionDirection::Left;
	case ConnectionDirection::Down: return ConnectionDirection::Up;
	case ConnectionDirection::Out: return ConnectionDirection::In;
	case ConnectionDirection::In: return ConnectionDirection::Out;
	default:
		OT_LOG_EA("Unknown connection direction");
		return ConnectionDirection::Any;
	}
}

std::list<ot::ConnectionDirection> ot::getAllConnectionDirections()
{
	return std::list<ConnectionDirection>({
		ConnectionDirection::Any,
		ConnectionDirection::Left,
		ConnectionDirection::Up,
		ConnectionDirection::Right,
		ConnectionDirection::Down,
		ConnectionDirection::Out,
		ConnectionDirection::In
		});
}

// ###########################################################################################################################################################################################################################################################################################################################

// Gradient

std::string ot::toString(GradientSpread _spread) {
	switch (_spread)
	{
	case GradientSpread::Pad: return "Pad";
	case GradientSpread::Repeat: return "Repeat";
	case GradientSpread::Reflect: return "Reflect";
	default:
		OT_LOG_EA("Unknown gradient spread");
		throw std::exception("Unknown gradient spread");
	}
}

ot::GradientSpread ot::stringToGradientSpread(const std::string& _spread) {
	if (_spread == toString(GradientSpread::Pad)) return GradientSpread::Pad;
	else if (_spread == toString(GradientSpread::Repeat)) return GradientSpread::Repeat;
	else if (_spread == toString(GradientSpread::Reflect)) return GradientSpread::Reflect;
	else {
		OT_LOG_EAS("Unknown gradient spread \"" + _spread + "\"");
		throw std::exception("Unknown gradient spread");
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Basic Key

std::string ot::toString(BasicKey _key) {
	switch (_key) {
	case BasicKey::A: return "A";
	case BasicKey::B: return "B";
	case BasicKey::C: return "C";
	case BasicKey::D: return "D";
	case BasicKey::E: return "E";
	case BasicKey::F: return "F";
	case BasicKey::G: return "G";
	case BasicKey::H: return "H";
	case BasicKey::I: return "I";
	case BasicKey::J: return "J";
	case BasicKey::K: return "K";
	case BasicKey::L: return "L";
	case BasicKey::M: return "M";
	case BasicKey::N: return "N";
	case BasicKey::O: return "O";
	case BasicKey::P: return "P";
	case BasicKey::Q: return "Q";
	case BasicKey::R: return "R";
	case BasicKey::S: return "S";
	case BasicKey::T: return "T";
	case BasicKey::U: return "U";
	case BasicKey::V: return "V";
	case BasicKey::W: return "W";
	case BasicKey::X: return "X";
	case BasicKey::Y: return "Y";
	case BasicKey::Z: return "Z";
	case BasicKey::Key_1: return "1";
	case BasicKey::Key_2: return "2";
	case BasicKey::Key_3: return "3";
	case BasicKey::Key_4: return "4";
	case BasicKey::Key_5: return "5";
	case BasicKey::Key_6: return "6";
	case BasicKey::Key_7: return "7";
	case BasicKey::Key_8: return "8";
	case BasicKey::Key_9: return "9";
	case BasicKey::Key_0: return "0";
	case BasicKey::Alt: return "Alt";
	case BasicKey::Shift: return "Shift";
	case BasicKey::Control: return "Ctrl";
	case BasicKey::CapsLock: return "CapsLock";
	case BasicKey::Tab: return "Tab";
	case BasicKey::Return: return "Return";
	case BasicKey::Space: return "Space";
	case BasicKey::Backspace: return "Backspace";
	case BasicKey::Delete: return "Del";
	case BasicKey::Up: return "Up";
	case BasicKey::Down: return "Down";
	case BasicKey::Left: return "Left";
	case BasicKey::Right: return "Right";
	case BasicKey::F1: return "F1";
	case BasicKey::F2: return "F2";
	case BasicKey::F3: return "F3";
	case BasicKey::F4: return "F4";
	case BasicKey::F5: return "F5";
	case BasicKey::F6: return "F6";
	case BasicKey::F7: return "F7";
	case BasicKey::F8: return "F8";
	case BasicKey::F9: return "F9";
	case BasicKey::F10: return "F10";
	case BasicKey::F11: return "F11";
	case BasicKey::F12: return "F12";
	default:
		OT_LOG_E("Unknown basic key (" + std::to_string((int)_key) + ")");
		return "A";
	}
}

ot::BasicKey ot::stringToBasicKey(const std::string& _key) {
	if (_key == toString(BasicKey::A)) return BasicKey::A;
	else if (_key == toString(BasicKey::B)) return BasicKey::B;
	else if (_key == toString(BasicKey::C)) return BasicKey::C;
	else if (_key == toString(BasicKey::D)) return BasicKey::D;
	else if (_key == toString(BasicKey::E)) return BasicKey::E;
	else if (_key == toString(BasicKey::F)) return BasicKey::F;
	else if (_key == toString(BasicKey::G)) return BasicKey::G;
	else if (_key == toString(BasicKey::H)) return BasicKey::H;
	else if (_key == toString(BasicKey::I)) return BasicKey::I;
	else if (_key == toString(BasicKey::J)) return BasicKey::J;
	else if (_key == toString(BasicKey::K)) return BasicKey::K;
	else if (_key == toString(BasicKey::L)) return BasicKey::L;
	else if (_key == toString(BasicKey::M)) return BasicKey::M;
	else if (_key == toString(BasicKey::N)) return BasicKey::N;
	else if (_key == toString(BasicKey::O)) return BasicKey::O;
	else if (_key == toString(BasicKey::P)) return BasicKey::P;
	else if (_key == toString(BasicKey::Q)) return BasicKey::Q;
	else if (_key == toString(BasicKey::R)) return BasicKey::R;
	else if (_key == toString(BasicKey::S)) return BasicKey::S;
	else if (_key == toString(BasicKey::T)) return BasicKey::T;
	else if (_key == toString(BasicKey::U)) return BasicKey::U;
	else if (_key == toString(BasicKey::V)) return BasicKey::V;
	else if (_key == toString(BasicKey::W)) return BasicKey::W;
	else if (_key == toString(BasicKey::X)) return BasicKey::X;
	else if (_key == toString(BasicKey::Y)) return BasicKey::Y;
	else if (_key == toString(BasicKey::Z)) return BasicKey::Z;
	else if (_key == toString(BasicKey::Key_1)) return BasicKey::Key_1;
	else if (_key == toString(BasicKey::Key_2)) return BasicKey::Key_2;
	else if (_key == toString(BasicKey::Key_3)) return BasicKey::Key_3;
	else if (_key == toString(BasicKey::Key_4)) return BasicKey::Key_4;
	else if (_key == toString(BasicKey::Key_5)) return BasicKey::Key_5;
	else if (_key == toString(BasicKey::Key_6)) return BasicKey::Key_6;
	else if (_key == toString(BasicKey::Key_7)) return BasicKey::Key_7;
	else if (_key == toString(BasicKey::Key_8)) return BasicKey::Key_8;
	else if (_key == toString(BasicKey::Key_9)) return BasicKey::Key_9;
	else if (_key == toString(BasicKey::Key_0)) return BasicKey::Key_0;
	else if (_key == toString(BasicKey::Alt)) return BasicKey::Alt;
	else if (_key == toString(BasicKey::Shift)) return BasicKey::Shift;
	else if (_key == toString(BasicKey::Control)) return BasicKey::Control;
	else if (_key == toString(BasicKey::CapsLock)) return BasicKey::CapsLock;
	else if (_key == toString(BasicKey::Tab)) return BasicKey::Tab;
	else if (_key == toString(BasicKey::Return)) return BasicKey::Return;
	else if (_key == toString(BasicKey::Space)) return BasicKey::A;
	else if (_key == toString(BasicKey::Backspace)) return BasicKey::A;
	else if (_key == toString(BasicKey::Delete)) return BasicKey::A;
	else if (_key == toString(BasicKey::Up)) return BasicKey::Up;
	else if (_key == toString(BasicKey::Down)) return BasicKey::Down;
	else if (_key == toString(BasicKey::Left)) return BasicKey::Left;
	else if (_key == toString(BasicKey::Right)) return BasicKey::Right;
	else if (_key == toString(BasicKey::F1)) return BasicKey::F1;
	else if (_key == toString(BasicKey::F2)) return BasicKey::F2;
	else if (_key == toString(BasicKey::F3)) return BasicKey::F3;
	else if (_key == toString(BasicKey::F4)) return BasicKey::F4;
	else if (_key == toString(BasicKey::F5)) return BasicKey::F5;
	else if (_key == toString(BasicKey::F6)) return BasicKey::F6;
	else if (_key == toString(BasicKey::F7)) return BasicKey::F7;
	else if (_key == toString(BasicKey::F8)) return BasicKey::F8;
	else if (_key == toString(BasicKey::F9)) return BasicKey::F9;
	else if (_key == toString(BasicKey::F10)) return BasicKey::F10;
	else if (_key == toString(BasicKey::F11)) return BasicKey::F11;
	else if (_key == toString(BasicKey::F12)) return BasicKey::F12;
	else {
		OT_LOG_E("Unknown basic key \"" + _key + "\"");
		return BasicKey::A;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Orientation

std::string ot::toString(DocumentSyntax _syntax) {
	switch (_syntax) {
	case DocumentSyntax::PlainText: return "Plain";
	case DocumentSyntax::PythonScript: return "Python";
	default:
		OT_LOG_EAS("Unknown document syntax (" + std::to_string((int)_syntax) + ")");
		return "Plain";
	}
}

ot::DocumentSyntax ot::stringToDocumentSyntax(const std::string& _syntax) {
	if (_syntax == toString(DocumentSyntax::PlainText)) return DocumentSyntax::PlainText;
	else if (_syntax == toString(DocumentSyntax::PythonScript)) return DocumentSyntax::PythonScript;
	else {
		OT_LOG_EAS("Unknown document syntax \"" + _syntax + "\"");
		return DocumentSyntax::PlainText;
	}
}

std::list<std::string> ot::getSupportedDocumentSyntaxStringList() {
	return std::list<std::string>({
		toString(DocumentSyntax::PlainText),
		toString(DocumentSyntax::PythonScript)
	});
}

// ###########################################################################################################################################################################################################################################################################################################################

// Image Format

std::string ot::toString(ImageFileFormat _format) {
	switch (_format) {
	case ImageFileFormat::PNG: return "png";
	case ImageFileFormat::JPEG: return "jpeg";
	case ImageFileFormat::SVG: return "svg";
	default:
		OT_LOG_E("Unknown image format (" + std::to_string((int)_format) + ")");
		return "PNG";
	}
}

ot::ImageFileFormat ot::stringToImageFileFormat(const std::string& _format) {
	if (_format == toString(ImageFileFormat::PNG)) return ImageFileFormat::PNG;
	else if (ContainerHelper::contains(std::vector<std::string>({"jpg", "jpeg", "jiff", "jpe"}), _format)) return ImageFileFormat::JPEG;
	else if (_format == toString(ImageFileFormat::SVG)) return ImageFileFormat::SVG;
	else {
		OT_LOG_E("Unknown image format \"" + _format + "\"");
		return ImageFileFormat::PNG;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Lock Type

std::string ot::toString(LockTypeFlag _type) {
	switch (_type) {
	case LockTypeFlag::None: return "None";
	case LockTypeFlag::ViewWrite: return "ViewWrite";
	case LockTypeFlag::ViewRead: return "ViewRead";
	case LockTypeFlag::ModelWrite: return "ModelWrite";
	case LockTypeFlag::ModelRead: return "ModelRead";
	case LockTypeFlag::Properties: return "Properties";
	case LockTypeFlag::NavigationWrite: return "NavigationWrite";
	case LockTypeFlag::NavigationAll: return "NavigationAll";
	case LockTypeFlag::All: return "All";
	default:
		OT_LOG_E("Unknown lock type (" + std::to_string((int)_type) + ")");
		return "None";
	}
}

ot::LockTypeFlag ot::stringToLockTypeFlag(const std::string& _flag) {
	if (_flag == toString(LockTypeFlag::ViewWrite)) return LockTypeFlag::ViewWrite;
	else if (_flag == toString(LockTypeFlag::ViewRead)) return LockTypeFlag::ViewRead;
	else if (_flag == toString(LockTypeFlag::ModelWrite)) return LockTypeFlag::ModelWrite;
	else if (_flag == toString(LockTypeFlag::ModelRead)) return LockTypeFlag::ModelRead;
	else if (_flag == toString(LockTypeFlag::All)) return LockTypeFlag::All;
	else if (_flag == toString(LockTypeFlag::Properties)) return LockTypeFlag::Properties;
	else if (_flag == toString(LockTypeFlag::NavigationWrite)) return LockTypeFlag::NavigationWrite;
	else if (_flag == toString(LockTypeFlag::NavigationAll)) return LockTypeFlag::NavigationAll;
	else if (_flag != toString(LockTypeFlag::None)) {
		OT_LOG_E("Unknown lock type flag \"" + _flag + "\"");
	}
	return LockTypeFlag::None;
}

std::list<std::string> ot::toStringList(const LockTypeFlags& _flags) {
	std::list<std::string> ret;

	if (_flags & LockTypeFlag::ViewWrite) ret.push_back(toString(LockTypeFlag::ViewWrite));
	if (_flags & LockTypeFlag::ViewRead) ret.push_back(toString(LockTypeFlag::ViewRead));
	if (_flags & LockTypeFlag::ModelWrite) ret.push_back(toString(LockTypeFlag::ModelWrite));
	if (_flags & LockTypeFlag::ModelRead) ret.push_back(toString(LockTypeFlag::ModelRead));
	if (_flags & LockTypeFlag::Properties) ret.push_back(toString(LockTypeFlag::Properties));
	if (_flags & LockTypeFlag::NavigationWrite) ret.push_back(toString(LockTypeFlag::NavigationWrite));
	if (_flags & LockTypeFlag::NavigationAll) ret.push_back(toString(LockTypeFlag::NavigationAll));
	if (_flags & LockTypeFlag::All) ret.push_back(toString(LockTypeFlag::All));

	return ret;
}

ot::LockTypeFlags ot::stringListToLockTypeFlags(const std::vector<std::string>& _flags) {
	LockTypeFlags flags(LockTypeFlag::None);
	for (const std::string& flag : _flags) {
		flags |= stringToLockTypeFlag(flag);
	}
	return flags;
}

ot::LockTypeFlags ot::stringListToLockTypeFlags(const std::list<std::string>& _flags) {
	LockTypeFlags flags(LockTypeFlag::None);
	for (const std::string& flag : _flags) {
		flags |= stringToLockTypeFlag(flag);
	}
	return flags;
}

std::list<ot::LockTypeFlag> ot::getAllSetFlags(const LockTypeFlags& _lockFlags) {
	std::list<LockTypeFlag> result;

	if (_lockFlags & LockTypeFlag::ViewWrite) result.push_back(LockTypeFlag::ViewWrite);
	if (_lockFlags & LockTypeFlag::ViewRead) result.push_back(LockTypeFlag::ViewRead);
	if (_lockFlags & LockTypeFlag::ModelWrite) result.push_back(LockTypeFlag::ModelWrite);
	if (_lockFlags & LockTypeFlag::ModelRead) result.push_back(LockTypeFlag::ModelRead);
	if (_lockFlags & LockTypeFlag::Properties) result.push_back(LockTypeFlag::Properties);
	if (_lockFlags & LockTypeFlag::NavigationWrite) result.push_back(LockTypeFlag::NavigationWrite);
	if (_lockFlags & LockTypeFlag::NavigationAll) result.push_back(LockTypeFlag::NavigationAll);
	if (_lockFlags & LockTypeFlag::All) result.push_back(LockTypeFlag::All);

	return result;
}

std::list<ot::LockTypeFlag> ot::getAllLockTypeFlags() {
	return std::list<LockTypeFlag>({
		LockTypeFlag::ViewWrite,
		LockTypeFlag::ViewRead,
		LockTypeFlag::ModelWrite,
		LockTypeFlag::ModelRead,
		LockTypeFlag::Properties,
		LockTypeFlag::NavigationWrite,
		LockTypeFlag::NavigationAll,
		LockTypeFlag::All,
		});
}

// ###########################################################################################################################################################################################################################################################################################################################

// Selection Result

std::string ot::toString(SelectionOrigin _flag) {
	switch (_flag) {
	case SelectionOrigin::User: return "User";
	case SelectionOrigin::View: return "View";
	case SelectionOrigin::Custom: return "Custom";
	default:
		OT_LOG_E("Unknown selection origin (" + std::to_string(static_cast<size_t>(_flag)) + ")");
		return "Custom";
	}
}

ot::SelectionOrigin ot::stringToSelectionOrigin(const std::string& _flag) {
	if (_flag == toString(SelectionOrigin::User)) return SelectionOrigin::User;
	else if (_flag == toString(SelectionOrigin::View)) return SelectionOrigin::View;
	else if (_flag == toString(SelectionOrigin::Custom)) return SelectionOrigin::Custom;
	else {
		OT_LOG_E("Unknown selection origin \"" + _flag + "\"");
		return SelectionOrigin::Custom;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Selection Result

std::string ot::toString(SelectionHandlingEvent _flag) {
	switch (_flag) {
	case SelectionHandlingEvent::Default: return "Default";
	case SelectionHandlingEvent::NewViewRequested: return "NewViewRequested";
	case SelectionHandlingEvent::ActiveViewChanged: return "ActiveViewChanged";
	case SelectionHandlingEvent::ActiveViewChangeRequested: return "ActiveViewChangeRequested";
	case SelectionHandlingEvent::ModelWasNotified: return "ModelWasNotified";
	default:
		OT_LOG_E("Unknown selection result (" + std::to_string((int)_flag) + ")");
		return "Default";
	}
}

ot::SelectionHandlingEvent ot::stringToSelectionHandlingEvent(const std::string& _flag) {
	if (_flag == toString(SelectionHandlingEvent::NewViewRequested)) return SelectionHandlingEvent::NewViewRequested;
	if (_flag == toString(SelectionHandlingEvent::ActiveViewChanged)) return SelectionHandlingEvent::ActiveViewChanged;
	if (_flag == toString(SelectionHandlingEvent::ActiveViewChangeRequested)) return SelectionHandlingEvent::ActiveViewChangeRequested;
	if (_flag == toString(SelectionHandlingEvent::ModelWasNotified)) return SelectionHandlingEvent::ModelWasNotified;
	else if (_flag != toString(SelectionHandlingEvent::Default)) {
		OT_LOG_E("Unknown selection result \"" + _flag + "\"");
	}
	return SelectionHandlingEvent::Default;
}

std::list<std::string> ot::toStringList(const SelectionHandlingResult& _flags) {
	std::list<std::string> result;
	for (SelectionHandlingEvent _event : getAllSetEvents(_flags)) {
		result.push_back(toString(_event));
	}
	return result;
}

ot::SelectionHandlingResult ot::stringListToSelectionHandlingResult(const std::list<std::string>& _flags) {
	SelectionHandlingResult result;
	for (const std::string& flag : _flags) {
		result |= stringToSelectionHandlingEvent(flag);
	}
	return result;
}

ot::SelectionHandlingResult ot::stringListToSelectionHandlingResult(const std::vector<std::string>& _flags) {
	SelectionHandlingResult result;
	for (const std::string& flag : _flags) {
		result |= stringToSelectionHandlingEvent(flag);
	}
	return result;
}

std::list<ot::SelectionHandlingEvent> ot::getAllSetEvents(const SelectionHandlingResult& _flags) {
	std::list<SelectionHandlingEvent> result;
	if (_flags & SelectionHandlingEvent::NewViewRequested) result.push_back(SelectionHandlingEvent::NewViewRequested);
	if (_flags & SelectionHandlingEvent::ActiveViewChanged) result.push_back(SelectionHandlingEvent::ActiveViewChanged);
	if (_flags & SelectionHandlingEvent::ActiveViewChangeRequested) result.push_back(SelectionHandlingEvent::ActiveViewChangeRequested);
	if (_flags & SelectionHandlingEvent::ModelWasNotified) result.push_back(SelectionHandlingEvent::ModelWasNotified);
	return result;
}

ot::SortOrder ot::oppositeOrder(SortOrder _order) {
	return (_order == SortOrder::Ascending ? SortOrder::Descending : SortOrder::Ascending);
}

std::string ot::toString(SortOrder _order) {
	switch (_order) {
	case SortOrder::Ascending: return "Ascending";
	case SortOrder::Descending: return "Descending";
	default:
		OT_LOG_E("Unknown sort order (" + std::to_string(static_cast<int>(_order)) + ")");
		return "Ascending";
	}
}

ot::SortOrder ot::stringToSortOrder(const std::string& _order) {
	if (_order == toString(SortOrder::Ascending)) return SortOrder::Ascending;
	else if (_order == toString(SortOrder::Descending)) return SortOrder::Descending;
	else {
		OT_LOG_E("Unknown sort order \"" + _order + "\"");
		return SortOrder::Ascending;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Path Browse Mode

std::string ot::toString(PathBrowseMode _mode) {
	switch (_mode) {
	case PathBrowseMode::ReadFile: return "Read File";
	case PathBrowseMode::WriteFile: return "Write File";
	case PathBrowseMode::Directory: return "Directory";
	default:
		OT_LOG_EA("Unknown browse mode");
		return "Read File";
	}
}

ot::PathBrowseMode ot::stringToPathBrowseMode(const std::string& _mode) {
	if (_mode == toString(PathBrowseMode::ReadFile)) return PathBrowseMode::ReadFile;
	else if (_mode == toString(PathBrowseMode::WriteFile)) return PathBrowseMode::WriteFile;
	else if (_mode == toString(PathBrowseMode::Directory)) return PathBrowseMode::Directory;
	else {
		OT_LOG_EA("Unknown browse mode");
		return PathBrowseMode::ReadFile;
	}
}
