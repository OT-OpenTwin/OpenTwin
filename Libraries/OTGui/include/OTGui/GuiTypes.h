//! @file GuiTypes.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>
#include <string>
#include <vector>

namespace ot {

	//! \enum Alignment
	//! \brief Alignment.
	enum Alignment {
		AlignCenter,
		AlignTop,
		AlignTopRight,
		AlignRight,
		AlignBottomRight,
		AlignBottom,
		AlignBottomLeft,
		AlignLeft,
		AlignTopLeft
	};

	//! \enum Orientation
	//! \brief Orientation.
	enum Orientation {
		Horizontal,
		Vertical
	};

	//! \enum FontFamily
	//! \brief Generally available font families.
	enum FontFamily {
		Arial,
		ComicSansMS,
		Consolas,
		CourierNew,
		Georgia,
		Helvetica,
		Impact,
		Palatino,
		TimesNewRoman,
		TrebuchetMS,
		Verdana
	};

	//! \enum SizePolicy
	//! \brief Size policy for UI elements.
	enum SizePolicy {
		Preferred, //! \brief The item size is the ideal size, the item wont shrink but may expand.
		Dynamic //! \brief The item size will be ignored and the requested size will be used instead (only use for nested items).
	};

	//! \enum ConnectionDirection
	//! \brief Describes in which direction a connetion from an item is starting.
	enum ConnectionDirection {
		ConnectAny, //! \brief Connections may start in any direction.
		ConnectLeft, //! \brief Connections start towards left.
		ConnectUp, //! \brief Connections start upwards.
		ConnectRight, //! \brief Connections start towards right.
		ConnectDown //! \brief Connections start downwards.
	};

	//! \brief Describes how the painter will behave when painting outsite the gradient area.
	enum GradientSpread {
		PadSpread, //! Pad the outside area with the color at the closest stop point.
		RepeatSpread, //! Repeat the gradient.
		ReflectSpread //! Reflect the gradient.
	};

	//! \enum BasicKey
	enum BasicKey {
		Key_A,
		Key_B,
		Key_C,
		Key_D,
		Key_E,
		Key_F,
		Key_G,
		Key_H,
		Key_I,
		Key_J,
		Key_K,
		Key_L,
		Key_M,
		Key_N,
		Key_O,
		Key_P,
		Key_Q,
		Key_R,
		Key_S,
		Key_T,
		Key_U,
		Key_V,
		Key_W,
		Key_X,
		Key_Y,
		Key_Z,
		Key_1,
		Key_2,
		Key_3,
		Key_4,
		Key_5,
		Key_6,
		Key_7,
		Key_8,
		Key_9,
		Key_0,
		Key_Alt,
		Key_Shift,
		Key_Control,
		Key_CapsLock,
		Key_Tab,
		Key_Return,
		Key_Space,
		Key_Backspace,
		Key_Delete,
		Key_Up,
		Key_Down,
		Key_Left,
		Key_Right,
		Key_F1,
		Key_F2,
		Key_F3,
		Key_F4,
		Key_F5,
		Key_F6,
		Key_F7,
		Key_F8,
		Key_F9,
		Key_F10,
		Key_F11,
		Key_F12
	};

	//! \brief Supported document syntax.
	//! Mainly used for syntax highlighting.
	enum class DocumentSyntax {
		PlainText,
		PythonScript
	};

	//! \brief Describes the object type to lock in the UI.
	//! \warning When adding new flags, add the corresponding flag to ot::getAllLockTypeFlags().
	enum LockTypeFlag {
		NoLockFlags         = 0x0000, //! \brief No lock flags are set.
		LockViewWrite       = 0x0001, //! \brief Lock view write operations.
		LockViewRead        = 0x0002, //! \brief Lock view read operations.
		//! \brief Lock model write operations.
		//! Any control should be disabled and any event supressed which would lead to modifying the model.
		LockModelWrite      = 0x0004,
		//! \brief Lock model write operations.
		//! Any control should be disabled and any event supressed which would lead to any model reading access.
		LockModelRead       = 0x0008,
		LockProperties      = 0x0010, //! \brief Lock properties.
		LockNavigationWrite = 0x0020, //! \brief Lock navigation write.
		LockNavigationAll   = 0x0040, //! \brief Lock navigation.
		LockAll             = 0x1000 //! \brief Lock all.
	};

	typedef ot::Flags<LockTypeFlag> LockTypeFlags;

	OT_GUI_API_EXPORT std::list<LockTypeFlag> getAllLockTypeFlags(void);

	OT_GUI_API_EXPORT std::string toString(Alignment _alignment);
	OT_GUI_API_EXPORT Alignment stringToAlignment(const std::string& _string);

	OT_GUI_API_EXPORT std::string toString(Orientation _orientation);
	OT_GUI_API_EXPORT Orientation stringToOrientation(const std::string& _string);

	OT_GUI_API_EXPORT std::string toString(FontFamily _fontFamily);
	OT_GUI_API_EXPORT FontFamily stringToFontFamily(const std::string& _string);

	OT_GUI_API_EXPORT std::string toString(SizePolicy _policy);
	OT_GUI_API_EXPORT SizePolicy stringToSizePolicy(const std::string& _string);

	OT_GUI_API_EXPORT std::string toString(ConnectionDirection _direction);
	OT_GUI_API_EXPORT ConnectionDirection stringToConnectionDirection(const std::string& _direction);

	//! @brief Returnse the opposite connection of the one provided
	OT_GUI_API_EXPORT ConnectionDirection inversedConnectionDirection(ConnectionDirection _direction);

	OT_GUI_API_EXPORT std::list<ConnectionDirection> getAllConnectionDirections(void);

	OT_GUI_API_EXPORT std::string toString(GradientSpread _spread);
	OT_GUI_API_EXPORT GradientSpread stringToGradientSpread(const std::string& _spread);

	OT_GUI_API_EXPORT BasicKey toBasicKey(const std::string& _key);

	OT_GUI_API_EXPORT std::string toString(BasicKey _key);

	OT_GUI_API_EXPORT DocumentSyntax toDocumentSyntax(const std::string& _syntax);

	OT_GUI_API_EXPORT std::string toString(DocumentSyntax _syntax);

	OT_GUI_API_EXPORT std::list<std::string> getAllSupportedDocumentSyntax(void);

	OT_GUI_API_EXPORT LockTypeFlags toLockTypeFlags(const std::vector<std::string>& _flags);

	OT_GUI_API_EXPORT LockTypeFlags toLockTypeFlags(const std::list<std::string>& _flags);

	OT_GUI_API_EXPORT LockTypeFlag toLockTypeFlag(const std::string& _flag);

	OT_GUI_API_EXPORT std::list<std::string> toStringList(const LockTypeFlags& _flags);

	OT_GUI_API_EXPORT std::string toString(LockTypeFlag _type);

}

OT_ADD_FLAG_FUNCTIONS(ot::LockTypeFlag);