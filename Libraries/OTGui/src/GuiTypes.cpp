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

// ###########################################################################################################################################################################################################################################################################################################################

// Orientation

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

// ###########################################################################################################################################################################################################################################################################################################################

// Font Family

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

// ###########################################################################################################################################################################################################################################################################################################################

// Size Policy

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

// ###########################################################################################################################################################################################################################################################################################################################

// Connections

std::string ot::toString(ot::ConnectionDirection _direction) {
	switch (_direction)
	{
	case ot::ConnectAny: return "Any";
	case ot::ConnectLeft: return "Left";
	case ot::ConnectUp: return "Up";
	case ot::ConnectRight: return "Right";
	case ot::ConnectDown: return "Down";
	case ot::ConnectOut: return "Out";
	case ot::ConnectIn: return "In";
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
	else if (_direction == toString(ot::ConnectOut)) return ot::ConnectOut;
	else if (_direction == toString(ot::ConnectIn)) return ot::ConnectIn;
	else {
		OT_LOG_EAS("Unknown connection direction \"" + _direction + "\"");
		throw std::exception("Unknown connection direction");
	}
}

ot::ConnectionDirection ot::inversedConnectionDirection(ConnectionDirection _direction) {
	switch (_direction)
	{
	case ot::ConnectAny: return ot::ConnectAny;
	case ot::ConnectLeft: return ot::ConnectRight;
	case ot::ConnectUp: return ot::ConnectDown;
	case ot::ConnectRight: return ot::ConnectLeft;
	case ot::ConnectDown: return ot::ConnectUp;
	case ot::ConnectOut: return ot::ConnectIn;
	case ot::ConnectIn: return ot::ConnectOut;
	default:
		OT_LOG_EA("Unknown connection direction");
		return ot::ConnectAny;
	}
}

std::list<ot::ConnectionDirection> ot::getAllConnectionDirections(void)
{
	return std::list<ConnectionDirection>({
		ConnectAny,
		ConnectLeft,
		ConnectUp,
		ConnectRight,
		ConnectDown,
		ConnectOut,
		ConnectIn
		});
}

// ###########################################################################################################################################################################################################################################################################################################################

// Gradient

std::string ot::toString(GradientSpread _spread) {
	switch (_spread)
	{
	case ot::PadSpread: return "Pad";
	case ot::RepeatSpread: return "Repeat";
	case ot::ReflectSpread: return "Reflect";
	default:
		OT_LOG_EA("Unknown gradient spread");
		throw std::exception("Unknown gradient spread");
	}
}

ot::GradientSpread ot::stringToGradientSpread(const std::string& _spread) {
	if (_spread == toString(PadSpread)) return PadSpread;
	else if (_spread == toString(RepeatSpread)) return RepeatSpread;
	else if (_spread == toString(ReflectSpread)) return ReflectSpread;
	else {
		OT_LOG_EAS("Unknown gradient spread \"" + _spread + "\"");
		throw std::exception("Unknown gradient spread");
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Basic Key

std::string ot::toString(BasicKey _key) {
	switch (_key) {
	case ot::Key_A: return "A";
	case ot::Key_B: return "B";
	case ot::Key_C: return "C";
	case ot::Key_D: return "D";
	case ot::Key_E: return "E";
	case ot::Key_F: return "F";
	case ot::Key_G: return "G";
	case ot::Key_H: return "H";
	case ot::Key_I: return "I";
	case ot::Key_J: return "J";
	case ot::Key_K: return "K";
	case ot::Key_L: return "L";
	case ot::Key_M: return "M";
	case ot::Key_N: return "N";
	case ot::Key_O: return "O";
	case ot::Key_P: return "P";
	case ot::Key_Q: return "Q";
	case ot::Key_R: return "R";
	case ot::Key_S: return "S";
	case ot::Key_T: return "T";
	case ot::Key_U: return "U";
	case ot::Key_V: return "V";
	case ot::Key_W: return "W";
	case ot::Key_X: return "X";
	case ot::Key_Y: return "Y";
	case ot::Key_Z: return "Z";
	case ot::Key_1: return "1";
	case ot::Key_2: return "2";
	case ot::Key_3: return "3";
	case ot::Key_4: return "4";
	case ot::Key_5: return "5";
	case ot::Key_6: return "6";
	case ot::Key_7: return "7";
	case ot::Key_8: return "8";
	case ot::Key_9: return "9";
	case ot::Key_0: return "0";
	case ot::Key_Alt: return "Alt";
	case ot::Key_Shift: return "Shift";
	case ot::Key_Control: return "Ctrl";
	case ot::Key_CapsLock: return "CapsLock";
	case ot::Key_Tab: return "Tab";
	case ot::Key_Return: return "Return";
	case ot::Key_Space: return "Space";
	case ot::Key_Backspace: return "Backspace";
	case ot::Key_Delete: return "Del";
	case ot::Key_Up: return "Up";
	case ot::Key_Down: return "Down";
	case ot::Key_Left: return "Left";
	case ot::Key_Right: return "Right";
	case ot::Key_F1: return "F1";
	case ot::Key_F2: return "F2";
	case ot::Key_F3: return "F3";
	case ot::Key_F4: return "F4";
	case ot::Key_F5: return "F5";
	case ot::Key_F6: return "F6";
	case ot::Key_F7: return "F7";
	case ot::Key_F8: return "F8";
	case ot::Key_F9: return "F9";
	case ot::Key_F10: return "F10";
	case ot::Key_F11: return "F11";
	case ot::Key_F12: return "F12";
	default:
		OT_LOG_E("Unknown basic key (" + std::to_string((int)_key) + ")");
		return "A";
	}
}

ot::BasicKey ot::stringToBasicKey(const std::string& _key) {
	if (_key == toString(Key_A)) return Key_A;
	else if (_key == toString(Key_B)) return Key_B;
	else if (_key == toString(Key_C)) return Key_C;
	else if (_key == toString(Key_D)) return Key_D;
	else if (_key == toString(Key_E)) return Key_E;
	else if (_key == toString(Key_F)) return Key_F;
	else if (_key == toString(Key_G)) return Key_G;
	else if (_key == toString(Key_H)) return Key_H;
	else if (_key == toString(Key_I)) return Key_I;
	else if (_key == toString(Key_J)) return Key_J;
	else if (_key == toString(Key_K)) return Key_K;
	else if (_key == toString(Key_L)) return Key_L;
	else if (_key == toString(Key_M)) return Key_M;
	else if (_key == toString(Key_N)) return Key_N;
	else if (_key == toString(Key_O)) return Key_O;
	else if (_key == toString(Key_P)) return Key_P;
	else if (_key == toString(Key_Q)) return Key_Q;
	else if (_key == toString(Key_R)) return Key_R;
	else if (_key == toString(Key_S)) return Key_S;
	else if (_key == toString(Key_T)) return Key_T;
	else if (_key == toString(Key_U)) return Key_U;
	else if (_key == toString(Key_V)) return Key_V;
	else if (_key == toString(Key_W)) return Key_W;
	else if (_key == toString(Key_X)) return Key_X;
	else if (_key == toString(Key_Y)) return Key_Y;
	else if (_key == toString(Key_Z)) return Key_Z;
	else if (_key == toString(Key_1)) return Key_1;
	else if (_key == toString(Key_2)) return Key_2;
	else if (_key == toString(Key_3)) return Key_3;
	else if (_key == toString(Key_4)) return Key_4;
	else if (_key == toString(Key_5)) return Key_5;
	else if (_key == toString(Key_6)) return Key_6;
	else if (_key == toString(Key_7)) return Key_7;
	else if (_key == toString(Key_8)) return Key_8;
	else if (_key == toString(Key_9)) return Key_9;
	else if (_key == toString(Key_0)) return Key_0;
	else if (_key == toString(Key_Alt)) return Key_Alt;
	else if (_key == toString(Key_Shift)) return Key_Shift;
	else if (_key == toString(Key_Control)) return Key_Control;
	else if (_key == toString(Key_CapsLock)) return Key_CapsLock;
	else if (_key == toString(Key_Tab)) return Key_Tab;
	else if (_key == toString(Key_Return)) return Key_Return;
	else if (_key == toString(Key_Space)) return Key_A;
	else if (_key == toString(Key_Backspace)) return Key_A;
	else if (_key == toString(Key_Delete)) return Key_A;
	else if (_key == toString(Key_Up)) return Key_Up;
	else if (_key == toString(Key_Down)) return Key_Down;
	else if (_key == toString(Key_Left)) return Key_Left;
	else if (_key == toString(Key_Right)) return Key_Right;
	else if (_key == toString(Key_F1)) return Key_F1;
	else if (_key == toString(Key_F2)) return Key_F2;
	else if (_key == toString(Key_F3)) return Key_F3;
	else if (_key == toString(Key_F4)) return Key_F4;
	else if (_key == toString(Key_F5)) return Key_F5;
	else if (_key == toString(Key_F6)) return Key_F6;
	else if (_key == toString(Key_F7)) return Key_F7;
	else if (_key == toString(Key_F8)) return Key_F8;
	else if (_key == toString(Key_F9)) return Key_F9;
	else if (_key == toString(Key_F10)) return Key_F10;
	else if (_key == toString(Key_F11)) return Key_F11;
	else if (_key == toString(Key_F12)) return Key_F12;
	else {
		OT_LOG_E("Unknown basic key \"" + _key + "\"");
		return Key_A;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Orientation

std::string ot::toString(DocumentSyntax _syntax) {
	switch (_syntax) {
	case ot::DocumentSyntax::PlainText: return "Plain";
	case ot::DocumentSyntax::PythonScript: return "Python";
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

std::list<std::string> ot::getSupportedDocumentSyntaxStringList(void) {
	return std::list<std::string>({
		ot::toString(DocumentSyntax::PlainText),
		ot::toString(DocumentSyntax::PythonScript)
	});
}

// ###########################################################################################################################################################################################################################################################################################################################

// Image Format

std::string ot::toString(ImageFileFormat _format) {
	switch (_format) {
	case ot::ImageFileFormat::PNG: return "png";
	case ot::ImageFileFormat::JPEG: return "jpeg";
	case ot::ImageFileFormat::SVG: return "svg";
	default:
		OT_LOG_E("Unknown image format (" + std::to_string((int)_format) + ")");
		return "PNG";
	}
}

ot::ImageFileFormat ot::stringToImageFileFormat(const std::string& _format) {
	if (_format == toString(ImageFileFormat::PNG)) return ImageFileFormat::PNG;
	else if (ot::ContainerHelper::contains(std::vector<std::string>({"jpg", "jpeg", "jiff", "jpe"}), _format)) return ImageFileFormat::JPEG;
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

ot::LockTypeFlag ot::stringToLockTypeFlag(const std::string& _flag) {
	if (_flag == toString(ot::LockViewWrite)) return ot::LockViewWrite;
	else if (_flag == toString(ot::LockViewRead)) return ot::LockViewRead;
	else if (_flag == toString(ot::LockModelWrite)) return ot::LockModelWrite;
	else if (_flag == toString(ot::LockModelRead)) return ot::LockModelRead;
	else if (_flag == toString(ot::LockAll)) return ot::LockAll;
	else if (_flag == toString(ot::LockProperties)) return ot::LockProperties;
	else if (_flag == toString(ot::LockNavigationWrite)) return ot::LockNavigationWrite;
	else if (_flag == toString(ot::LockNavigationAll)) return ot::LockNavigationAll;
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

ot::LockTypeFlags ot::stringListToLockTypeFlags(const std::vector<std::string>& _flags) {
	LockTypeFlags flags(NoLockFlags);
	for (const std::string& flag : _flags) {
		flags |= stringToLockTypeFlag(flag);
	}
	return flags;
}

ot::LockTypeFlags ot::stringListToLockTypeFlags(const std::list<std::string>& _flags) {
	LockTypeFlags flags(NoLockFlags);
	for (const std::string& flag : _flags) {
		flags |= stringToLockTypeFlag(flag);
	}
	return flags;
}

std::list<ot::LockTypeFlag> ot::getAllSetFlags(const LockTypeFlags& _lockFlags) {
	std::list<LockTypeFlag> result;

	if (_lockFlags & LockViewWrite) result.push_back(LockViewWrite);
	if (_lockFlags & LockViewRead) result.push_back(LockViewRead);
	if (_lockFlags & LockModelWrite) result.push_back(LockModelWrite);
	if (_lockFlags & LockModelRead) result.push_back(LockModelRead);
	if (_lockFlags & LockProperties) result.push_back(LockProperties);
	if (_lockFlags & LockNavigationWrite) result.push_back(LockNavigationWrite);
	if (_lockFlags & LockNavigationAll) result.push_back(LockNavigationAll);
	if (_lockFlags & LockAll) result.push_back(LockAll);

	return result;
}

std::list<ot::LockTypeFlag> ot::getAllLockTypeFlags(void) {
	return std::list<ot::LockTypeFlag>({
		LockTypeFlag::LockViewWrite,
		LockTypeFlag::LockViewRead,
		LockTypeFlag::LockModelWrite,
		LockTypeFlag::LockModelRead,
		LockTypeFlag::LockProperties,
		LockTypeFlag::LockNavigationWrite,
		LockTypeFlag::LockNavigationAll,
		LockTypeFlag::LockAll,
		});
}

// ###########################################################################################################################################################################################################################################################################################################################

// Selection Result

std::string ot::toString(SelectionOrigin _flag) {
	switch (_flag) {
	case ot::SelectionOrigin::User: return "User";
	case ot::SelectionOrigin::View: return "View";
	case ot::SelectionOrigin::Custom: return "Custom";
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
	case ot::SortOrder::Ascending: return "Ascending";
	case ot::SortOrder::Descending: return "Descending";
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
	case ot::PathBrowseMode::ReadFile: return "Read File";
	case ot::PathBrowseMode::WriteFile: return "Write File";
	case ot::PathBrowseMode::Directory: return "Directory";
	default:
		OT_LOG_EA("Unknown browse mode");
		return "Read File";
	}
}

ot::PathBrowseMode ot::stringToPathBrowseMode(const std::string& _mode) {
	if (_mode == ot::toString(PathBrowseMode::ReadFile)) return PathBrowseMode::ReadFile;
	else if (_mode == ot::toString(PathBrowseMode::WriteFile)) return PathBrowseMode::WriteFile;
	else if (_mode == ot::toString(PathBrowseMode::Directory)) return PathBrowseMode::Directory;
	else {
		OT_LOG_EA("Unknown browse mode");
		return PathBrowseMode::ReadFile;
	}
}
