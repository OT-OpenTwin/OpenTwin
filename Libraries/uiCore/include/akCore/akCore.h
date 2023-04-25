/*
 *	File:		akCore.h
 *	Package:	akCore
 *
 *  Created on: February 06, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

 // AK header
#include <akCore/globalDataTypes.h>

 // Qt header
#include <qstring.h>		// QString
#include <qflags.h>			// QFlags
#include <qcryptographichash.h>

// C++ header
#include <vector>					// vector<T>
#include <list>						// list<T>
#include <string>
#include <sstream>

// Forward declaration
class QKeyEvent;

namespace ak {

	enum colorAreaFlag {
		cafBackgroundColorWindow = 1 << 0,
		cafForegroundColorWindow = 1 << 1,
		cafBorderColorWindow = 1 << 2,
		cafBackgroundColorHeader = 1 << 3,
		cafForegroundColorHeader = 1 << 4,
		cafBorderColorHeader = 1 << 5,
		cafBackgroundColorControls = 1 << 6,
		cafForegroundColorControls = 1 << 7,
		cafBorderColorControls = 1 << 8,
		cafDefaultBorderControls = 1 << 9,
		cafDefaultBorderWindow = 1 << 10,
		cafBackgroundColorAlternate = 1 << 11,
		cafForegroundColorError = 1 << 12,
		cafBackgroundColorFocus = 1 << 13,
		cafBackgroundColorSelected = 1 << 14,
		cafForegroundColorFocus = 1 << 15,
		cafForegroundColorSelected = 1 << 16,
		cafBackgroundColorTransparent = 1 << 17,
		cafImagesTree = 1 << 18,
		cafImagesDock = 1 << 19,
		cafDefaultBorderHeader = 1 << 20,
		cafBackgroundColorDialogWindow = 1 << 21,
		cafForegroundColorDialogWindow = 1 << 22,
		cafBackgroundColorButton = 1 << 23,
		cafForegroundColorButton = 1 << 24,
		cafTabToolBarGroupSeperatorLine = 1 << 25,
		cafBackgroundColorView = 1 << 26
	};

	//! Describes the type of an command
	enum commandType {
		ctClearCreatedObjects
	};

	enum contextMenuRole {
		cmrNone,
		cmrClear
	};

	//! Describes the date format
	enum dateFormat {
		dfDDMMYYYY,
		dfMMDDYYYY,
		dfYYYYMMDD,
		dfYYYYDDMM
	};

	//! Describes the dialog result
	enum dialogResult {
		resultYes,
		resultNo,
		resultOk,
		resultCancel,
		resultRetry,
		resultIgnore,
		resultNone
	};

	//! Describes the dock location of a dock widget
	enum dockLocation {
		dockLeft,
		dockRight,
		dockBottom
	};

	//! Describes the type of an event
	enum eventType : unsigned long long {
		etUnknownEvent = 1 << 0,
		etActivated = 1 << 1,
		etChanged = 1 << 2,
		etCleared = 1 << 3,
		etClicked = 1 << 4,
		etClosing = 1 << 5,
		etCollpased = 1 << 6,
		etCursorPosotionChanged = 1 << 7,
		etDestroyed = 1 << 8,
		etDoubleClicked = 1 << 9,
		etExpanded = 1 << 10,
		etFocused = 1 << 11,
		etFocusLeft = 1 << 12,
		etInvalidEntry = 1 << 13,
		etIndexChanged = 1 << 14,
		etItemChanged = 1 << 15,
		etItemTextChanged = 1 << 16,
		etKeyPressed = 1 << 17,
		etKeyReleased = 1 << 18,
		etLocationChanged = 1 << 19,
		etSelectionChanged = 1 << 20,
		etStateChanged = 1 << 21,
		etTabToolbarChanged = 1 << 22,
		etTabToolbarClicked = 1 << 23,
		etTimeout = 1 << 24,
		etToggeledChecked = 1 << 25,
		etToggeledUnchecked = 1 << 26,
		etContextMenuItemClicked = 1 << 27,
		etContextMenuItemCheckedChanged = 1 << 28,
		etVisibilityChanged = 1 << 29,
		etReturnPressed = 1 << 30,
		etEditingFinished = 1 << 31,
		etDeleted = (unsigned long long ) (1) << 32
	};

	enum HashAlgorithm
	{
		hashKeccak_224,
		hashKeccak_256,
		hashKeccak_384,
		hashKeccak_512,
		hashMd4,
		hashMd5,
		hashRealSha3_224,
		hashRealSha3_256,
		hashRealSha3_384,
		hashRealSha3_512,
		hashSha1,
		hashSha_224,
		hashSha_256,
		hashSha_384,
		hashSha3_224,
		hashSha3_256,
		hashSha3_384,
		hashSha3_512,
		hashSha512
	};

	enum keyType {
		keyUnknown,
		keyAlt,
		keyAltGr,
		keyControl,
		keyDelete,
		keyEscape,
		keyF1,
		keyF10,
		keyF11,
		keyF12,
		keyF2,
		keyF3,
		keyF4,
		keyF5,
		keyF6,
		keyF7,
		keyF8,
		keyF9,
		keyReturn,
		keySpace
	};

	//! Describes the type of an object
	enum objectType {
		otNone,
		otAction,
		otColorEditButton,
		otColorStyleSwitchButton,
		otComboBox,
		otComboButton,
		otComboButtonItem,
		otCheckBox,
		otDatePicker,
		otDatePickerDialog,
		otDefaultWelcomeScreen,
		otDock,
		otDockWatcher,
		otGlobalKeyListener,
		otGraphicsView,
		otLabel,
		otLineEdit,
		otList,
		otLogInDialog,
		otMainWindow,
		otMenu,
		otNiceLineEdit,
		otOptionsDialog,
		otPrompt,
		otPropertyGrid,
		otPropertyGridItem,
		otPushButton,
		otRadioButton,
		otSpecialTabBar,
		otSpinBox,
		otTable,
		otTabToolbar,
		otTabToolbarGroup,
		otTabToolbarPage,
		otTabToolbarSubgroup,
		otTabView,
		otTextEdit,
		otTimePicker,
		otTimePickerDialog,
		otTimer,
		otToolButton,
		otToolButtonCustomContextMenu,
		otTree,
		otTreeItem
	};

	//! Describes the promt type
	enum promptType {
		promptOk,
		promptYesNo,
		promptYesNoCancel,
		promptOkCancel,
		promptRetryCancel,
		promptIgnoreRetryCancel,
		promptIconLeft,
		promptOkCancelIconLeft,
		promptOkIconLeft,
		promptYesNoIconLeft,
		promptYesNoCancelIconLeft
	};
	
	//! Descirbes the tab location
	enum tabLocation {
		tabLocationUp,
		tabLocationDown,
		tabLocationLeft,
		tabLocationRight
	};

	//! Describes the text alignment
	enum textAlignment {
		alignLeft,
		alignRight,
		alignCenter
	};

	//! Describes the time format
	enum timeFormat {
		tfHHMM,
		tfHHMMSS,
		tfHHMMSSMMMM
	};

	enum valueType {
		vtArray,
		vtBool,
		vtColor,
		vtDate,
		vtDouble,
		vtFloat,
		vtInt,
		vtInt64,
		vtLosslessDouble,
		vtLosslessFloat,
		vtNull,
		vtNumber,
		vtObject,
		vtSelection,
		vtString,
		vtTime,
		vtUint,
		vtUint64
	};

	inline eventType operator | (eventType a, eventType b)
	{
		return static_cast<eventType>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline colorAreaFlag operator | (colorAreaFlag a, colorAreaFlag b)
	{
		return static_cast<colorAreaFlag>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline bool flagIsSet(colorAreaFlag _flags, colorAreaFlag _testFlag) { return ((_flags & _testFlag) == _testFlag); }

	// ############################################################################################################################################

	// Point2D and Point3D

	template <class T, class U> class aPoint2D {
	public:
		//! @brief Constructor
		//! @param _x The initial X value
		//! @param _y The initial Y value
		aPoint2D(T _x, U _y);

		//! @brief Copy constructor
		//! @param _other The other 2D point
		aPoint2D(const aPoint2D & _other);

		aPoint2D<T, U> & operator = (const aPoint2D<T, U> & _other);
		aPoint2D<T, U> operator + (const aPoint2D<T, U> & _other);
		aPoint2D<T, U> & operator += (const aPoint2D<T, U> & _other);
		bool operator == (const aPoint2D<T, U> & _other);
		bool operator != (const aPoint2D<T, U> & _other);

		//! @brief Returns the current X value
		T x(void) const;

		//! @brief Returns the current Y value
		U y(void) const;

		//! @brief Will set the X value
		//! @param _x The X value to set
		void setX(T _x);

		//! @brief Will set the Y value
		//! @param _y The Y value to set
		void setY(U _y);

		//! @brief Will add the provided value to the X value and return the new X value
		//! @param _v The value to add
		T addX(T _v);

		//! @brief Will add the provided value to the Y value and return the new Y value
		//! @param _v The value to add
		U addY(U _v);

		//! @brief Will subtract the provided value from the X value and return the new X value
		//! @param _v The value to subtract
		T subX(T _v);

		//! @brief Will subtract the provided value from the Y value and return the new Y value
		//! @param _v The value to subtract
		U subY(U _v);

	private:
		// Block default constructor
		aPoint2D() = delete;

		T		m_x;			//! The current X value
		U		m_y;			//! The current Y value

	};

	template <class T, class U, class V> class aPoint3D {
	public:
		//! @brief Constructor
		//! @param _x The initial X value
		//! @param _y The initial Y value
		//! @param _z The initial Z value
		aPoint3D(T _x, U _y, V _z);

		//! @brief Copy constructor
		//! @param _other The other 3D point
		aPoint3D(const aPoint3D & _other);

		aPoint3D<T, U, V> & operator = (const aPoint3D<T, U, V> & _other);
		aPoint3D<T, U, V> operator + (const aPoint3D<T, U, V> & _other);
		aPoint3D<T, U, V> & operator += (const aPoint3D<T, U, V> & _other);
		bool operator == (const aPoint3D<T, U, V> & _other);
		bool operator != (const aPoint3D<T, U, V> & _other);

		//! @brief Returns the current X value
		T x(void) const;

		//! @brief Returns the current Y value
		U y(void) const;

		//! @brief Returns the current Z value
		V z(void) const;

		//! @brief Will set the X value
		//! @param _x The X value to set
		void setX(T _x);

		//! @brief Will set the Y value
		//! @param _y The Y value to set
		void setY(U _y);

		//! @brief Will set the Z value
		//! @param _z The Z value to set
		void setZ(V _z);

		//! @brief Will add the provided value to the X value and return the new X value
		//! @param _v The value to add
		T addX(T _v);

		//! @brief Will add the provided value to the Y value and return the new Y value
		//! @param _v The value to add
		U addY(U _v);

		//! @brief Will add the provided value to the Z value and return the new Z value
		//! @param _v The value to add
		V addZ(V _z);

		//! @brief Will subtract the provided value from the X value and return the new X value
		//! @param _v The value to subtract
		T subX(T _v);

		//! @brief Will subtract the provided value from the Y value and return the new Y value
		//! @param _v The value to subtract
		U subY(U _v);

		//! @brief Will subtract the provided value from the Z value and return the new Z value
		//! @param _v The value to subtract
		V subZ(V _v);

	private:
		// Block default constructor
		aPoint3D() = delete;

		T		m_x;			//! The current X value
		U		m_y;			//! The current Y value
		V		m_z;			//! The current Z value

	}; // class aPoint3D

	// Point 2D and Point3D

	// ############################################################################################################################################

	// Converting

	//! @brief Create a string representation of the provided ak::core::eventType
	//! @param _eventType The event type to be converted
	UICORE_API_EXPORT QString toQString(
		eventType								_eventType
	);

	//! @brief Create a string representation of the provided ak::core::valueType
	//! @param _valueType The value type to be converted
	UICORE_API_EXPORT QString toQString(
		valueType								_valueType
	);
	
	UICORE_API_EXPORT std::vector<int> toCharIndex(
		const char *					str
	);

	//! @brief Will return the QCryptographicHash::Algorithm corresponding to the provided HashAlgorithm
	UICORE_API_EXPORT QCryptographicHash::Algorithm toQCryptographicHashAlgorithm(HashAlgorithm _algorhitm);

	//! @brief Will return a hashed string that was hashed by the specified algorhitm
	UICORE_API_EXPORT QString hashString(const QString & _str, HashAlgorithm _algorhitm);

	//! @brief Will return a vector containing all the information from the list
	template <class T> std::vector<T> toVector(const std::list<T> & _list) {
		std::vector<T> ret;
		ret.reserve(_list.size());
		for (auto itm : _list) { ret.push_back(itm); }
		return ret;
	}

	//! @brief Create a string representation of the provided elementType
	//! @param _elementType The elementtype to be converted
	UICORE_API_EXPORT QString toQString(
		objectType								_objectType
	);

	//! @brief Create a string representation of the provided textAlignment
	//! @param _textAlignment The text alignment to be converted
	UICORE_API_EXPORT QString toQString(
		textAlignment							_textAlignment
	);

	//! @brief Create a string representation of the provided tabLocation
	//! @param _tabLocation The tab location to be converted
	UICORE_API_EXPORT QString toQString(
		tabLocation								_tabLocation
	);

	//! @brief Create a string representation of the provided dockLocation
	//! @param _dockLocation The dock location to be converted
	UICORE_API_EXPORT QString toQString(
		dockLocation							_dockLocation
	);

	//! @brief Create a string representation of the provided dialog result
	//! @param _dialogResult The dialog result to be converted
	UICORE_API_EXPORT QString toQString(
		dialogResult							_dialogResult
	);

	//! @brief Create a string representation of the provided prompt type
	//! @param _promptType The prompt type to be converted
	UICORE_API_EXPORT QString toQString(
		promptType								_promptType
	);

	//! @brief Create a string representation of the provided keyType
	UICORE_API_EXPORT QString toQString(
		keyType									_keyType
	);

	//! @brief Create a string representation of the provided contextMenuRole
	UICORE_API_EXPORT QString toQString(
		contextMenuRole							_role
	);

	//! @brief Create a string representation of the provided date format
	UICORE_API_EXPORT QString toQString(
		dateFormat								_dateFormat
	);

	//! @brief Create a string representation of the provided time format
	UICORE_API_EXPORT QString toQString(
		timeFormat								_timeFormat
	);

	// Converting

	// ############################################################################################################################################

	// Parsing

	//! @brief Will return the key type of the provided QKeyEvent
	//! If the key is not included in the keyType type a keyType::key_Unknown will be returned
	//! @param _event The event to extract the keyType from
	UICORE_API_EXPORT keyType toKeyType(
		QKeyEvent *								_event
	);

	//! @brief Will return the corresponding Qt text alignment
	//! @param _textAlignment The text alignment to convert
	UICORE_API_EXPORT Qt::AlignmentFlag toQtAlignmentFlag(
		textAlignment							_textAlignment
	);

	//! @brief Will return the date format that the provided string is representing
	UICORE_API_EXPORT dateFormat toDateFormat(const QString& _str);

	//! @brief Will return the time format that the provided string is representing
	UICORE_API_EXPORT timeFormat toTimeFormat(const QString& _str);

	// Parsing

	// ############################################################################################################################################

	// Type checking

	//! @brief Will convert and return the number specified in the string
	//! @param _string The string containing the number
	//! @param _failed Reference to a flag where the failed state will be written to
	template <class T> T toNumber(const std::string& _string, bool & _failed) {
		std::stringstream ss(_string);
		T v;
		ss >> v;
		_failed = false;
		if (ss.fail()) { _failed = true; }
		std::string rest;
		ss >> rest;
		if (!rest.empty()) { _failed = true; }
		return v;
	}

	//! @brief Will convert and return the number specified in the string
	//! @param _string The string containing the number
	//! @param _failed Reference to a flag where the failed state will be written to
	template <class T> T toNumber(const QString& _string, bool & _failed) {
		return toNumber<T>(_string.toStdString(), _failed);
	}

	//! @brief Will convert and return the number specified in the string
	//! @param _string The string containing the number
	//! @param _failed Reference to a flag where the failed state will be written to
	template <class T> T toNumber(const char* _string, bool & _failed) {
		return toNumber<T>(std::string(_string), _failed);
	}

	//! @brief Returns true if the provided C-String is in a format ready to be converted to a double or float
	//! @param str The C-String to be checked
	UICORE_API_EXPORT bool isDecimal(const char *str);

	//! @brief Returns true if the provided C++ String is in a format ready to be converted to a double or float
	//! @param str The C++ String to be checked
	UICORE_API_EXPORT bool isDecimal(const std::string &str);

	//! @brief Returns true if the provided Qt String is in a format ready to be converted to a double or float
	//! @param str The Qt String to be checked
	UICORE_API_EXPORT bool isDecimal(const QString &str);

	//! @brief Returns true if the provided C-String is in a format ready to be converted to an integer
	//! @param str The C-String to be checked
	UICORE_API_EXPORT bool isInteger(const char *str);

	//! @brief Returns true if the provided C++ String is in a format ready to be converted to an integer
	//! @param str The C++ String to be checked
	UICORE_API_EXPORT bool isInteger(const std::string &str);

	//! @brief Returns true if the provided Qt String is in a format ready to be converted to an integer
	//! @param str The Qt String to be checked
	UICORE_API_EXPORT bool isInteger(const QString &str);

	//! @brief Returns true if the provided C-String consists only of numerical characters
	//! @param str The C-String to be checked
	UICORE_API_EXPORT bool isNumericOnly(const char *str);

	//! @brief Returns true if the provided C++ String consists only of numerical characters
	//! @param str The C++ String to be checked
	UICORE_API_EXPORT bool isNumericOnly(const std::string &str);

	//! @brief Returns true if the provided Qt String consists only of numerical characters
	//! @param str The Qt String to be checked
	UICORE_API_EXPORT bool isNumericOnly(const QString &str);

} // namespace ak

// point 2d

template <class T, class U> ak::aPoint2D<T, U>::aPoint2D(T _x, U _y) { m_x = _x; m_y = _y; }

template <class T, class U> ak::aPoint2D<T, U>::aPoint2D(const aPoint2D & _other) { m_x = _other.x(); m_y = _other.y(); }

template <class T, class U> ak::aPoint2D<T, U> & ak::aPoint2D<T, U>::operator = (const aPoint2D<T, U> & _other) {
	m_x = _other.x(); m_y = _other.y();
	return *this;
}

template <class T, class U> ak::aPoint2D<T, U> ak::aPoint2D<T, U>::operator + (const aPoint2D<T, U> & _other) {
	aPoint2D<T, U> ret(m_x, m_y);
	ret.addX(_other.x());
	ret.addY(_other.y());
	return ret;

}

template <class T, class U> ak::aPoint2D<T, U> & ak::aPoint2D<T, U>::operator += (const aPoint2D<T, U> & _other) {
	m_x = m_x + _other.x();
	m_y = m_y + _other.y();
	m_z = m_z + _other.z();
	return *this;
}

template <class T, class U> bool ak::aPoint2D<T, U>::operator == (const aPoint2D & _other) {
	if (m_x != _other.x()) { return false; }
	if (m_y != _other.y()) { return false; }
	return true;
}

template <class T, class U> bool ak::aPoint2D<T, U>::operator != (const aPoint2D & _other) { return !(*this == _other); }

template <class T, class U> T ak::aPoint2D<T, U>::x(void) const { return m_x; }

template <class T, class U> U ak::aPoint2D<T, U>::y(void) const { return m_y; }

template <class T, class U> void ak::aPoint2D<T, U>::setX(T _x) { m_x = _x; }

template <class T, class U> void ak::aPoint2D<T, U>::setY(U _y) { m_y = _y; }

template <class T, class U> T ak::aPoint2D<T, U>::addX(T _v) { m_x = m_x + v; return m_x; }

template <class T, class U> U ak::aPoint2D<T, U>::addY(U _v) { m_y = m_y + v; return m_y; }

template <class T, class U> T ak::aPoint2D<T, U>::subX(T _v) { m_x = m_x - _v; return m_x; }

template <class T, class U> U ak::aPoint2D<T, U>::subY(U _v) { m_y = m_y - _v; return m_y; }

// ############################################################################################################################################

// point 3d

template <class T, class U, class V> ak::aPoint3D<T, U, V>::aPoint3D(T _x, U _y, V _z) { m_x = _x; m_y = _y; m_z = _z; }

template <class T, class U, class V> ak::aPoint3D<T, U, V>::aPoint3D(const aPoint3D & _other) { m_x = _other.x(); m_y = _other.y(); m_z = _other.z(); }

template <class T, class U, class V> ak::aPoint3D<T, U, V> & ak::aPoint3D<T, U, V>::operator = (const aPoint3D<T, U, V> & _other) {
	m_x = _other.x(); m_y = _other.y(); m_z = _other.z();
	return *this;
}

template <class T, class U, class V> ak::aPoint3D<T, U, V> ak::aPoint3D<T, U, V>::operator + (const aPoint3D<T, U, V> & _other) {
	aPoint3D<T, U, V> ret(m_x, m_y, m_z);
	ret.addX(_other.x());
	ret.addY(_other.y());
	ret.addZ(_other.z());
	return ret;

}

template <class T, class U, class V>  ak::aPoint3D<T, U, V> & ak::aPoint3D<T, U, V>::operator += (const aPoint3D<T, U, V> & _other) {
	m_x = m_x + _other.x();
	m_y = m_y + _other.y();
	m_z = m_z + _other.z();
	return *this;
}

template <class T, class U, class V> bool ak::aPoint3D<T, U, V>::operator == (const aPoint3D & _other) {
	if (m_x != _other.x()) { return false; }
	if (m_y != _other.y()) { return false; }
	if (m_z != _other.z()) { return false; }
	return true;
}

template <class T, class U, class V> bool ak::aPoint3D<T, U, V>::operator != (const aPoint3D & _other) { return !(*this == _other); }

template <class T, class U, class V> T ak::aPoint3D<T, U, V>::x(void) const { return m_x; }

template <class T, class U, class V> U ak::aPoint3D<T, U, V>::y(void) const { return m_y; }

template <class T, class U, class V> V ak::aPoint3D<T, U, V>::z(void) const { return m_z; }

template <class T, class U, class V> void ak::aPoint3D<T, U, V>::setX(T _x) { m_x = _x; }

template <class T, class U, class V> void ak::aPoint3D<T, U, V>::setY(U _y) { m_y = _y; }

template <class T, class U, class V> void ak::aPoint3D<T, U, V>::setZ(V _z) { m_z = _z; }

template <class T, class U, class V> T ak::aPoint3D<T, U, V>::addX(T _v) { m_x = m_x + v; return m_x; }

template <class T, class U, class V> U ak::aPoint3D<T, U, V>::addY(U _v) { m_y = m_y + v; return m_y; }

template <class T, class U, class V> V ak::aPoint3D<T, U, V>::addZ(V _v) { m_z = m_z + v; return m_z; }

template <class T, class U, class V> T ak::aPoint3D<T, U, V>::subX(T _v) { m_x = m_x - _v; return m_x; }

template <class T, class U, class V> U ak::aPoint3D<T, U, V>::subY(U _v) { m_y = m_y - _v; return m_y; }

template <class T, class U, class V> V ak::aPoint3D<T, U, V>::subZ(V _z) { m_z = m_z - _v; return m_z; }
