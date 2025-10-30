// @otlicense
// File: GuiTypes.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>
#include <string>
#include <vector>

namespace ot {

	// ###########################################################################################################################################################################################################################################################################################################################

	// Alignment

	//! @brief Alignment.
	enum class Alignment {
		Center,
		Top,
		TopRight,
		Right,
		BottomRight,
		Bottom,
		BottomLeft,
		Left,
		TopLeft
	};

	OT_GUI_API_EXPORT std::string toString(Alignment _alignment);
	OT_GUI_API_EXPORT Alignment stringToAlignment(const std::string& _string);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Orientation

	//! @brief Orientation.
	enum class Orientation {
		Horizontal,
		Vertical
	};

	OT_GUI_API_EXPORT std::string toString(Orientation _orientation);
	OT_GUI_API_EXPORT Orientation stringToOrientation(const std::string& _string);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Font Family
	
	//! @brief Generally available font families.
	enum class FontFamily {
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

	OT_GUI_API_EXPORT std::string toString(FontFamily _fontFamily);
	OT_GUI_API_EXPORT FontFamily stringToFontFamily(const std::string& _string);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Size Policy

	//! @brief Size policy for UI elements.
	enum class SizePolicy {
		Preferred, //! @brief The item size is the ideal size, the item wont shrink but may expand.
		Dynamic //! @brief The item size will be ignored and the requested size will be used instead (only use for nested items).
	};

	OT_GUI_API_EXPORT std::string toString(SizePolicy _policy);
	OT_GUI_API_EXPORT SizePolicy stringToSizePolicy(const std::string& _string);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Connections

	//! @brief Describes in which direction a connetion from an item is starting.
	enum class ConnectionDirection {
		Any,   //! @brief Connection may start in any direction.
		Left,  //! @brief Connection start towards left.
		Up,    //! @brief Connection start upwards.
		Right, //! @brief Connection start towards right.
		Down,  //! @brief Connection start downwards.
		Out,   //! @brief Connection start outwards relative to the item center.
		In     //! @brief Connection start inwards relative to the item center.
	};

	OT_GUI_API_EXPORT std::string toString(ConnectionDirection _direction);
	OT_GUI_API_EXPORT ConnectionDirection stringToConnectionDirection(const std::string& _direction);

	//! @brief Returnse the opposite connection of the one provided
	OT_GUI_API_EXPORT ConnectionDirection inversedConnectionDirection(ConnectionDirection _direction);

	OT_GUI_API_EXPORT std::list<ConnectionDirection> getAllConnectionDirections();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Gradient

	//! @brief Describes how the painter will behave when painting outsite the gradient area.
	enum class GradientSpread {
		Pad,    //! Pad the outside area with the color at the closest stop point.
		Repeat, //! Repeat the gradient.
		Reflect //! Reflect the gradient.
	};

	OT_GUI_API_EXPORT std::string toString(GradientSpread _spread);
	OT_GUI_API_EXPORT GradientSpread stringToGradientSpread(const std::string& _spread);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Basic Key

	//! @brief Basic keys used for shortcuts and key events.
	enum class BasicKey {
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
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
		Alt,
		Shift,
		Control,
		CapsLock,
		Tab,
		Return,
		Space,
		Backspace,
		Delete,
		Up,
		Down,
		Left,
		Right,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12
	};

	OT_GUI_API_EXPORT std::string toString(BasicKey _key);

	OT_GUI_API_EXPORT BasicKey stringToBasicKey(const std::string& _key);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Document Syntax

	//! @brief Supported document syntax.
	//! Mainly used for syntax highlighting.
	enum class DocumentSyntax {
		PlainText,
		PythonScript,
		Markdown,
		HTML
	};

	OT_GUI_API_EXPORT std::string toString(DocumentSyntax _syntax);

	OT_GUI_API_EXPORT DocumentSyntax stringToDocumentSyntax(const std::string& _syntax);

	OT_GUI_API_EXPORT std::list<std::string> getSupportedDocumentSyntaxStringList();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Image Format

	enum class ImageFileFormat {
		PNG,
		JPEG,
		SVG
	};

	OT_GUI_API_EXPORT std::string toString(ImageFileFormat _format);
	OT_GUI_API_EXPORT ImageFileFormat stringToImageFileFormat(const std::string& _format);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Lock Type

	//! @brief Describes the object type to lock in the UI.
	//! @warning When adding new flags, add the corresponding flag to ot::getAllLockTypes().
	enum class LockType {
		None            = 0 << 0, //! @brief No lock flags are set.
		ViewWrite       = 1 << 0, //! @brief Lock view write operations.
		ViewRead        = 1 << 1, //! @brief Lock view read operations.
		
		//! @brief Lock model write operations.
		//! Any control should be disabled and any event supressed which would lead to modifying the model.
		ModelWrite      = 1 << 2,

		//! @brief Lock model write operations.
		//! Any control should be disabled and any event supressed which would lead to any model reading access.
		ModelRead       = 1 << 3,
		Properties      = 1 << 4, //! @brief Lock properties.
		NavigationWrite = 1 << 5, //! @brief Lock navigation write.
		NavigationAll   = 1 << 6, //! @brief Lock navigation.
		All             = 1 << 7  //! @brief Lock all.
	};

	typedef ot::Flags<LockType> LockTypes;

	OT_GUI_API_EXPORT std::string toString(LockType _type);

	OT_GUI_API_EXPORT LockType stringToLockType(const std::string& _flag);

	OT_GUI_API_EXPORT std::list<std::string> toStringList(const LockTypes& _flags);

	OT_GUI_API_EXPORT LockTypes stringListToLockTypes(const std::list<std::string>& _flags);

	OT_GUI_API_EXPORT LockTypes stringListToLockTypes(const std::vector<std::string>& _flags);

	OT_GUI_API_EXPORT std::list<LockType> getAllSetFlags(const LockTypes& _lockFlags);

	OT_GUI_API_EXPORT std::list<LockType> getAllLockTypes();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Selection Result

	//! @brief Selection origin.
	enum class SelectionOrigin {
		User,
		View,
		Custom
	};

	OT_GUI_API_EXPORT std::string toString(SelectionOrigin _flag);

	OT_GUI_API_EXPORT SelectionOrigin stringToSelectionOrigin(const std::string& _flag);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Selection Result

	//! @brief Result flag of a selection changed notification.
	enum class SelectionHandlingEvent {
		Default                   = 0 << 0, //! @brief Default selection handling result.
		NewViewRequested          = 1 << 0, //! @brief A new view was requested.
		ActiveViewChanged         = 1 << 1, //! @brief The active view has changed after handling the selection.
		ActiveViewChangeRequested = 1 << 2, //! @brief A change of the active view was requested/queued.
		ModelWasNotified          = 1 << 3, //! @brief Model service received a selection changed notification.

		//! @brief The active view has not changed yet but may have been queued.
		NoCurrentViewChangeMask   = ~ActiveViewChanged,

		//! @brief No view change was requested, but may have already changed.
		//! @ref SelectionResult::ActiveViewChanged
		NoViewChangeRequestedMask = ~(NewViewRequested | ActiveViewChangeRequested)
	};

	//! @brief Result of a selection changed notification.
	typedef ot::Flags<SelectionHandlingEvent> SelectionHandlingResult;

	OT_GUI_API_EXPORT std::string toString(SelectionHandlingEvent _flag);

	OT_GUI_API_EXPORT SelectionHandlingEvent stringToSelectionHandlingEvent(const std::string& _flag);

	OT_GUI_API_EXPORT std::list<std::string> toStringList(const SelectionHandlingResult& _flags);

	OT_GUI_API_EXPORT SelectionHandlingResult stringListToSelectionHandlingResult(const std::list<std::string>& _flags);

	OT_GUI_API_EXPORT SelectionHandlingResult stringListToSelectionHandlingResult(const std::vector<std::string>& _flags);

	OT_GUI_API_EXPORT std::list<SelectionHandlingEvent> getAllSetEvents(const SelectionHandlingResult& _lockFlags);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Sort Order

	enum class SortOrder {
		Ascending,
		Descending
	};

	OT_GUI_API_EXPORT SortOrder oppositeOrder(SortOrder _order);

	OT_GUI_API_EXPORT std::string toString(SortOrder _order);

	OT_GUI_API_EXPORT SortOrder stringToSortOrder(const std::string& _order);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Path Browse Mode

	enum class PathBrowseMode {
		ReadFile = 0,
		WriteFile = 1,
		Directory = 2
	};

	OT_GUI_API_EXPORT std::string toString(PathBrowseMode _mode);

	OT_GUI_API_EXPORT PathBrowseMode stringToPathBrowseMode(const std::string& _mode);

}

OT_ADD_FLAG_FUNCTIONS(ot::LockType)
OT_ADD_FLAG_FUNCTIONS(ot::SelectionHandlingEvent)