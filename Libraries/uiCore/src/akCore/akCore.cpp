/*
 *	File:		akCore.cpp
 *	Package:	akCore
 *
 *  Created on: February 06, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akCore/akCore.h>		// Corresponding header

// Qt header
#include <qevent.h>				// QKeyEvent

QString ak::toQString(
	eventType				_eventType
) {
	switch (_eventType)
	{
	case etUnknownEvent: return QString("UnknownEvent");
	case etActivated: return QString("Activated");
	case etChanged: return QString("Changed");
	case etCleared: return QString("Cleared");
	case etClicked: return QString("Clicked");
	case etClosing: return QString("Closing");
	case etCollpased: return QString("Collapsed");
	case etCursorPosotionChanged: return QString("CursorPositionChanged");
	case etDestroyed: return QString("Destroyed");
	case etDoubleClicked: return QString("DoubleClicked");
	case etExpanded: return QString("Expanded");
	case etFocused: return QString("Focused");
	case etFocusLeft: return QString("FocusLeft");
	case etIndexChanged: return QString("IndexChanged");
	case etInvalidEntry: return QString("InvalidEntry");
	case etItemChanged: return QString("ItemChanged");
	case etKeyPressed: return QString("KeyPressed");
	case etKeyReleased: return QString("KeyReleased");
	case etLocationChanged: return QString("LocationChanged");
	case etItemTextChanged: return QString("ItemTextChanged");
	case etSelectionChanged: return QString("SelectionChanged");
	case etStateChanged: return QString("StateChanged");
	case etTabToolbarChanged: return QString("TabToolbarChanged");
	case etTabToolbarClicked: return QString("TabToolbarClicked");
	case etTimeout: return QString("Timeout");
	case etToggeledChecked: return QString("ToggeledToChecked");
	case etToggeledUnchecked: return QString("ToggeledToUnchecked");
	case etContextMenuItemClicked: return QString("ContextMenuItemClicked");
	case etContextMenuItemCheckedChanged: return QString("ContextMenuItemCheckedChanged");
	case etVisibilityChanged: return QString("VisibilityChanged");
	case etReturnPressed: return QString("ReturnPressed");
	case etEditingFinished: return QString("EditingFinished");
	default: assert(0); // not implemented yet
	}
	return QString("");
}

QString ak::toQString(
	valueType				_valueType
) {
	switch (_valueType)
	{
	case vtArray: return QString("Array"); break;
	case vtBool: return QString("Bool"); break;
	case vtColor: return QString("Color"); break;
	case vtDate: return QString("Date"); break;
	case vtDouble: return QString("Double"); break;
	case vtFloat: return QString("Float"); break;
	case vtInt: return QString("Integer"); break;
	case vtInt64: return QString("64bitInteger"); break;
	case vtLosslessDouble: return QString("LosslessDouble"); break;
	case vtLosslessFloat: return QString("LosslessFloat"); break;
	case vtNull: return QString("Null"); break;
	case vtNumber: return QString("Number"); break;
	case vtObject: return QString("Object"); break;
	case vtSelection: return QString("Selection"); break;
	case vtString: return QString("String"); break;
	case vtTime: return QString("Time"); break;
	case vtUint: return QString("UnsignedInteger"); break;
	case vtUint64: return QString("Unsigned64bitInteger"); break;
	default: assert(0); // not implemented yet
	}
	return QString("");
}

bool ak::isDecimal(const char *str) {
	bool failed;
	toNumber<double>(str, failed);
	return !failed;
}

bool ak::isDecimal(const std::string &str) {
	bool failed;
	toNumber<double>(str, failed);
	return !failed;
}

bool ak::isDecimal(const QString &str) {
	bool failed;
	toNumber<double>(str.toStdString(), failed);
	return !failed;
}

bool ak::isInteger(const char *str) {
	bool failed;
	toNumber<int>(str, failed);
	return !failed;
}

bool ak::isInteger(const std::string &str) {
	bool failed;
	toNumber<int>(str, failed);
	return !failed;
}

bool ak::isInteger(const QString &str) {
	bool failed;
	toNumber<int>(str.toStdString(), failed);
	return !failed;
}

bool ak::isNumericOnly(const char *str) {
	QString s{ str };
	for (auto c : s) {
		if (!c.isNumber()) { return false; }
	}
	return true;
}

bool ak::isNumericOnly(const std::string &str) {
	QString s{ str.c_str() };
	for (auto c : s) {
		if (!c.isNumber()) { return false; }
	}
	return true;
}

bool ak::isNumericOnly(const QString &str) {
	for (auto c : str) {
		if (!c.isNumber()) { return false; }
	}
	return true;
}

QCryptographicHash::Algorithm ak::toQCryptographicHashAlgorithm(HashAlgorithm _algorhitm) {
	switch (_algorhitm)
	{
	case hashKeccak_224: return QCryptographicHash::Algorithm::Keccak_224;
	case hashKeccak_256: return QCryptographicHash::Algorithm::Keccak_256;
	case hashKeccak_384: return QCryptographicHash::Algorithm::Keccak_384;
	case hashKeccak_512: return QCryptographicHash::Algorithm::Keccak_512;
	case hashMd4: return QCryptographicHash::Algorithm::Md4;
	case hashMd5: return QCryptographicHash::Algorithm::Md5;
	case hashRealSha3_224: return QCryptographicHash::Algorithm::RealSha3_224;
	case hashRealSha3_256: return QCryptographicHash::Algorithm::RealSha3_256;
	case hashRealSha3_384: return QCryptographicHash::Algorithm::RealSha3_384;
	case hashRealSha3_512: return QCryptographicHash::Algorithm::RealSha3_512;
	case hashSha1: return QCryptographicHash::Algorithm::Sha1;
	case hashSha_224: return QCryptographicHash::Algorithm::Sha224;
	case hashSha_256: return QCryptographicHash::Algorithm::Sha256;
	case hashSha_384: return QCryptographicHash::Algorithm::Sha384;
	case hashSha3_224: return QCryptographicHash::Algorithm::Sha3_224;
	case hashSha3_256: return QCryptographicHash::Algorithm::Sha3_256;
	case hashSha3_384: return QCryptographicHash::Algorithm::Sha3_384;
	case hashSha3_512: return QCryptographicHash::Algorithm::Sha3_512;
	case hashSha512: return QCryptographicHash::Algorithm::Sha512;
	default:
		assert(0);	// No such algorithm
		throw std::exception("No such algorithm");
	}
}

QString ak::hashString(const QString & _str, HashAlgorithm _algorhitm) {
	QCryptographicHash hash(toQCryptographicHashAlgorithm(_algorhitm));
	std::string str(_str.toStdString());
	hash.addData(str.c_str(), str.length());
	QByteArray arr(hash.result());
	QByteArray result(arr.toHex());
	std::string ret(result.toStdString());
	return QString(ret.c_str());
}

// ########################################################################################

// String

std::vector<int> ak::toCharIndex(
	const char *					str
) {
	std::vector<int> out;
	if (str != nullptr) {
		while (*str != 0) { out.push_back((int)*str); str++; }
	}
	out.push_back(0);
	return out;
}

QString ak::toQString(
	objectType				_objectType
) {
	switch (_objectType)
	{
	case otNone: return QString("None");
	case otAction: return QString("Action");
	case otComboBox: return QString("ComboBox");
	case otComboButton: return QString("ComboButton");
	case otCheckBox: return QString("CheckBox");
	case otDatePicker: return QString("DatePicker");
	case otDatePickerDialog: return QString("DatePickerDialog");
	case otDefaultWelcomeScreen: return ("DefaultWelcomeScreen");
	case otDock: return QString("Dock");
	case otDockWatcher: return QString("DockWatcher");
	case otGlobalKeyListener: return QString("GlobalKeyListener");
	case otGraphicsView: return QString("GraphicsView");
	case otLabel: return QString("Label");
	case otLineEdit: return QString("LineEdit");
	case otList: return QString("List");
	case otMainWindow: return QString("Window");
	case otNiceLineEdit: return QString("NiceLineEdit");
	case otPrompt: return QString("Prompt");
	case otPropertyGrid: return QString("PropertyGrid");
	case otPushButton: return QString("PushButton");
	case otRadioButton: return QString("RadioButton");
	case otSpecialTabBar: return QString("SpecialTabBar");
	case otSpinBox: return QString("SpinBox");
	case otTable: return QString("Table");
	case otTabToolbar: return QString("TabToolBar");
	case otTabToolbarGroup: return QString("TabToolBarGroup");
	case otTabToolbarPage: return QString("TabBoolBarPage");
	case otTabToolbarSubgroup: return QString("TabToolBarSubGroup");
	case otTabView: return QString("TabView");
	case otTextEdit: return QString("TextEdit");
	case otTimePicker: return QString("TimePicker");
	case otTimePickerDialog: return QString("TimePickerDialog");
	case otTimer: return QString("Timer");
	case otToolButton: return QString("ToolButton");
	case otToolButtonCustomContextMenu: return QString("ToolButtonCustomContextMenu");
	case otTree: return QString("Tree");
	case otTreeItem: return QString("TreeItem");
	default: assert(0); // Not implemented yet
	}
	return QString("");
}

QString ak::toQString(
	textAlignment				_textAlignment
) {
	switch (_textAlignment)
	{
	case alignCenter: return QString("AlignCenter");
	case alignLeft: return QString("AlignLeft");
	case alignRight: return QString("AlignRight");
	default: assert(0); // Not implemented yet
	}
	return QString("");
}

QString ak::toQString(
	tabLocation								_tabLocation
) {
	switch (_tabLocation)
	{
	case tabLocationDown: return QString("Down");
	case tabLocationLeft: return QString("Left");
	case tabLocationRight: return QString("Right");
	case tabLocationUp: return QString("Up");
	default: assert(0); // Not implemented yet
		break;
	}
	return QString("");
}

QString ak::toQString(
	dockLocation							_dockLocation
) {
	switch (_dockLocation)
	{
	case dockBottom: return QString("DockBottom");
	case dockLeft: return QString("DockLeft");
	case dockRight: return QString("DockRight");
	default: assert(0); // Not implemented yet
	}
	return QString("");
}

QString ak::toQString(
	dialogResult							_dialogResult
) {
	switch (_dialogResult)
	{
	case resultYes: return QString("Result.Yes");
	case resultNo: return QString("Result.No");
	case resultOk: return QString("Result.Ok");
	case resultCancel: return QString("Result.Cancel");
	case resultIgnore: return QString("Result.Ignore");
	case resultRetry: return QString("Result.Retry");
	case resultNone: return QString("Result.None");
	default:
		assert(0);
		return QString();
	}
}

QString ak::toQString(
	promptType								_promptType
) {
	switch (_promptType)
	{
	case promptOk: return QString("Prompt.Ok");
	case promptYesNo: return QString("Prompt.YesNo");
	case promptYesNoCancel: return QString("Prompt.YesNoCancel");
	case promptOkCancel: return QString("Prompt.OkCancel");
	case promptRetryCancel: return QString("Prompt.RetryCancel");
	case promptIgnoreRetryCancel: return QString("Prompt.IgnoreRetryCancel");
	default:
		assert(0); // Unknown type
		return QString();
	}
}

QString ak::toQString(
	keyType									_keyType
) {
	switch (_keyType)
	{
	case keyUnknown: return QString("KeyUnknown");
	case keyAlt: return QString("KeyAlt");
	case keyAltGr: return QString("KeyAltGr");
	case keyControl: return QString("KeyControl");
	case keyDelete: return QString("KeyDelete");
	case keyEscape: return QString("KeyEscape");
	case keyF1: return QString("KeyF1");
	case keyF10: return QString("KeyF10");
	case keyF11: return QString("KeyF11");
	case keyF12: return QString("KeyF12");
	case keyF2: return QString("KeyF2");
	case keyF3: return QString("KeyF3");
	case keyF4: return QString("KeyF4");
	case keyF5: return QString("KeyF5");
	case keyF6: return QString("KeyF6");
	case keyF7: return QString("KeyF7");
	case keyF8: return QString("KeyF8");
	case keyF9: return QString("KeyF9");
	case keyReturn: return QString("KeyReturn");
	case keySpace: return QString("KeySpace");
	default:
		assert(0); // Not implemented yet
		return QString("");
	}
}

QString ak::toQString(
	contextMenuRole							_role
) {
	switch (_role)
	{
	case cmrNone: return QString("None");
	case cmrClear: return QString("Clear");
	default:
		assert(0); // Not implemented yet
		return QString("");
	}
}

QString ak::toQString(
	dateFormat								_dateFormat
) {
	switch (_dateFormat)
	{
	case ak::dfDDMMYYYY: return "DDMMYYYY";
	case ak::dfMMDDYYYY: return "MMDDYYYY";
	case ak::dfYYYYMMDD: return "YYYYMMDD";
	case ak::dfYYYYDDMM: return "YYYYDDMM";
	default:
		assert(0); return "DDMMYYYY";
	}
}

QString ak::toQString(
	timeFormat								_timeFormat
) {
	switch (_timeFormat)
	{
	case ak::tfHHMM: return "HHMM";
	case ak::tfHHMMSS: return "HHMMSS";
	case ak::tfHHMMSSMMMM: return "HHMMSSMMMM";
	default:
		assert(0); return "HHMM";
	}
}

// ############################################################################################################################################

// Parsing

ak::keyType ak::toKeyType(
	QKeyEvent *								_event
) {
	assert(_event != nullptr); // Nullptr provided
	switch (_event->key())
	{
	case Qt::Key::Key_Alt: return keyType::keyAlt; break;
	case Qt::Key::Key_AltGr: return keyType::keyAltGr; break;
	case Qt::Key::Key_Control: return keyType::keyControl; break;
	case Qt::Key::Key_Delete: return keyType::keyDelete; break;
	case Qt::Key::Key_Backspace: return keyType::keyDelete; break;
	case Qt::Key::Key_Escape: return keyType::keyEscape; break;
	case Qt::Key::Key_F1: return keyType::keyF1; break;
	case Qt::Key::Key_F10: return keyType::keyF10; break;
	case Qt::Key::Key_F11: return keyType::keyF11; break;
	case Qt::Key::Key_F12: return keyType::keyF12; break;
	case Qt::Key::Key_F2: return keyType::keyF2; break;
	case Qt::Key::Key_F3: return keyType::keyF3; break;
	case Qt::Key::Key_F4: return keyType::keyF4; break;
	case Qt::Key::Key_F5: return keyType::keyF5; break;
	case Qt::Key::Key_F6: return keyType::keyF6; break;
	case Qt::Key::Key_F7: return keyType::keyF7; break;
	case Qt::Key::Key_F8: return keyType::keyF8; break;
	case Qt::Key::Key_F9: return keyType::keyF9; break;
	case Qt::Key::Key_Return: return keyType::keyReturn; break;
	case Qt::Key::Key_Space: return keyType::keySpace; break;
	default: return keyType::keyUnknown;
	}
}

Qt::AlignmentFlag ak::toQtAlignmentFlag(
	textAlignment							_textAlignment
) {
	switch (_textAlignment)
	{
	case alignLeft: return Qt::AlignmentFlag::AlignLeft;
	case alignRight: return Qt::AlignmentFlag::AlignRight;
	case alignCenter: return Qt::AlignmentFlag::AlignCenter;
	default:
		assert(0);	// Not implemented yet
		return Qt::AlignmentFlag::AlignCenter;
		break;
	}
}

ak::dateFormat ak::toDateFormat(const QString& _str) {
	if (_str == "DDMMYYYY") { return dfDDMMYYYY; }
	else if (_str == "MMDDYYYY") { return dfDDMMYYYY; }
	else if (_str == "YYYYMMDD") { return dfDDMMYYYY; }
	else if (_str == "YYYYDDMM") { return dfDDMMYYYY; }
	else { assert(0); return dfDDMMYYYY; }
}

ak::timeFormat ak::toTimeFormat(const QString& _str) {
	if (_str == "HHMM") { return tfHHMM; }
	else if (_str == "HHMMSS") { return tfHHMMSS; }
	else if (_str == "HHMMSSMMMM") { return tfHHMMSSMMMM; }
	else { assert(0); return tfHHMM; }
}

// Parsing

// ############################################################################################################################################
