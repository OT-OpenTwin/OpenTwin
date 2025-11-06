// @otlicense
// File: ColorStyleTypes.h
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
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>
#include <string>

namespace ot {

	//! @brief Contains default OpenTwin color style names.
	enum class ColorStyleName {
		BrightStyle,
		DarkStyle,
		BlueStyle
	};

	//! @brief Contains color style file keys.
	//! The keys are used to determine the beginning of a section in the color style file.
	enum class ColorStyleFileKey {
		NameKey,
		IntegerKey,
		DoubleKey,
		FileKey,
		PainterKey,
		SheetKey
	};

	//! @brief Contains color style file macros.
	enum class ColorStyleFileMacro {
		RootMacro
	};

	enum class ColorStyleFileValue {
		NameValue,
		PathValue,
		ValueValue
	};

	//! @brief Contains macros used in the color style base file.
	enum class ColorStyleBaseFileMacro {
		PainterMacro,
		FileMacro,
		IntMacro,
		DoubleMacro
	};

	//! @brief Contains default file paths set in a color style.
	enum class ColorStyleFileEntry {
		TransparentIcon = 0,

		ArrowUpIcon,
		ArrowUpDisabledIcon,
		ArrowUpFocusIcon,
		ArrowUpPressed,
		ArrowDownIcon,
		ArrowDownDisabledIcon,
		ArrowDownFocusIcon,
		ArrowDownPressed,
		ArrowLeftIcon,
		ArrowLeftDisabledIcon,
		ArrowLeftFocusIcon,
		ArrowLeftPressed,
		ArrowRightIcon,
		ArrowRightDisabledIcon,
		ArrowRightFocusIcon,
		ArrowRightPressed,

		TreeBranchClosedIcon,
		TreeBranchClosedFocusIcon,
		TreeBranchEndIcon,
		TreeBranchLineIcon,
		TreeBranchMoreIcon,
		TreeBranchOpenIcon,
		TreeBranchOpenFocusIcon,

		CheckBoxCheckedIcon,
		CheckBoxCheckedDisabledIcon,
		CheckBoxCheckedFocusIcon,
		CheckBoxCheckedPressedIcon,
		CheckBoxIndeterminateIcon,
		CheckBoxIndeterminateDisabledIcon,
		CheckBoxIndeterminateFocusIcon,
		CheckBoxIndeterminatePressedIcon,
		CheckBoxUncheckedIcon,
		CheckBoxUncheckedDisabledIcon,
		CheckBoxUncheckedFocusIcon,
		CheckBoxUncheckedPressedIcon,

		LogInCheckBoxCheckedIcon,
		LogInCheckBoxCheckedDisabledIcon,
		LogInCheckBoxCheckedFocusIcon,
		LogInCheckBoxCheckedPressedIcon,
		LogInCheckBoxIndeterminateIcon,
		LogInCheckBoxIndeterminateDisabledIcon,
		LogInCheckBoxIndeterminateFocusIcon,
		LogInCheckBoxIndeterminatePressedIcon,
		LogInCheckBoxUncheckedIcon,
		LogInCheckBoxUncheckedDisabledIcon,
		LogInCheckBoxUncheckedFocusIcon,
		LogInCheckBoxUncheckedPressedIcon,

		RadioButtonCheckedIcon,
		RadioButtonCheckedDisabledIcon,
		RadioButtonCheckedFocusIcon,
		RadioButtonUncheckedIcon,
		RadioButtonUncheckedDisabledIcon,
		RadioButtonUncheckedFocusIcon,

		ToolBarMoveHorizontalIcon,
		ToolBarMoveVerticalIcon,
		ToolBarSeparatorHorizontalIcon,
		ToolBarSeparatorVerticalIcon,

		HeaderFilterIcon,
		HeaderFilterHoverIcon,
		HeaderFilterPressedIcon,
		HeaderFilterActiveIcon,
		HeaderFilterDisabledIcon,

		WindowCloseIcon,
		WindowCloseDisabledIcon,
		WindowCloseFocusIcon,
		WindowClosePressedIcon,
		WindowPinIcon,
		WindowUnpinIcon,
		WindowPinDisabledIcon,
		WindowUnpinDisabledIcon,
		WindowPinFocusIcon,
		WindowUnpinFocusIcon,
		WindowPinPressedIcon,
		WindowUnpinPressedIcon,
		WindowLockIcon,
		WindowUnlockIcon,
		WindowLockDisabledIcon,
		WindowUnlockDisabledIcon,
		WindowLockFocusIcon,
		WindowUnlockFocusIcon,
		WindowLockPressedIcon,
		WindowUnlockPressedIcon,
		WindowGripIcon,
		WindowUndockIcon,
		WindowUndockDisabledIcon,
		WindowUndockFocusIcon,
		WindowUndockPressedIcon,

		PropertyItemDeleteIcon,
		PropertyGroupExpandedIcon,
		PropertyGroupCollapsedIcon,
		LogInBackgroundImage,

		//! @brief Used to determine the last entry.
		//! @warning This must always be the last entry in this enum.
		ColorStyleFileEntry_End
	};

	//! @brief Contains default painters set in a color style.
	enum class ColorStyleValueEntry : uint32_t {
		Transparent = 0,

		WidgetBackground,
		WidgetAlternateBackground,
		WidgetForeground,
		WidgetDisabledBackground,
		WidgetDisabledForeground,
		WidgetHoverBackground,
		WidgetHoverForeground,
		WidgetSelectionBackground,
		WidgetSelectionForeground,
		
		TitleBackground,
		TitleForeground,
		TitleBorder,

		HeaderBackground,
		HeaderForeground,
		HeaderHoverBackground,
		HeaderHoverForeground,
		HeaderSelectionBackground,
		HeaderSelectionForeground,

		InputBackground,
		InputDisabledBackground,
		InputForeground,
		InputDisabledForeground,

		TextEditorLineBorder,
		TextEditorHighlightBackground,

		DialogBackground,
		DialogForeground,

		WindowBackground,
		WindowForeground,

		Border,
		BorderLight,
		BorderDisabled,
		BorderHover,
		BorderSelection,

		Rainbow0,
		Rainbow1,
		Rainbow2,
		Rainbow3,
		Rainbow4,
		Rainbow5,
		Rainbow6,
		Rainbow7,
		Rainbow8,
		Rainbow9,

		RainbowFirst = Rainbow0, //! @brief Helper to iterate over the rainbow. First rainbow entry.
		RainbowLast = Rainbow9, //! @brief Helper to iterate over the rainbow. Last rainbow entry.

		ToolBarFirstTabBackground,
		ToolBarFirstTabForeground,

		GraphicsItemBorder,
		GraphicsItemConnection,
		GraphicsItemConnectionConnectable,
		GraphicsItemBackground,
		GraphicsItemForeground,
		GraphicsItemSelectionBorder,
		GraphicsItemHoverBorder,
		GraphicsItemConnectableBackground,
		GraphicsItemLineColor,
		GraphicsItemRubberband,

		StyledTextHighlight,
		StyledTextLightHighlight,
		StyledTextComment,
		StyledTextWarning,
		StyledTextError,

		PythonKeyword,
		PythonClass,
		PythonString,
		PythonFunction,
		PythonComment,

		PlotCurve,
		PlotCurveDimmed,
		PlotCurveHighlight,
		PlotCurveSymbol,

		SuccessForeground,
		WarningForeground,
		ErrorForeground,

		//! @brief Used to determine the last entry.
		//! @warning This must always be the last entry in this enum.
		ColorStyleValueEntry_End
	};

	//! @brief Contains default integer values set in a color style.
	enum class ColorStyleIntegerEntry {
		SplitterHandleWidth,
		SplitterBorderRadius
	};

	//! @brief Contains default double values set in a color style.
	enum class ColorStyleDoubleEntry {
		BorderWidth,
		BorderRadiusBig,
		BorderRadiusSmall,
		ToolTipOpacity
	};

	OT_GUI_API_EXPORT std::string toString(ColorStyleName _colorStyleName);
	OT_GUI_API_EXPORT ColorStyleName stringToColorStyleName(const std::string& _colorStyleName);
	
	OT_GUI_API_EXPORT std::string toString(ColorStyleFileKey _colorStyleFileKey);
	OT_GUI_API_EXPORT ColorStyleFileKey stringToColorStyleFileKey(const std::string& _colorStyleFileKey);

	OT_GUI_API_EXPORT std::string toString(ColorStyleFileMacro _colorStyleFileMacro);
	OT_GUI_API_EXPORT ColorStyleFileMacro stringToColorStyleFileMacro(const std::string& _colorStyleFileMacro);

	OT_GUI_API_EXPORT std::string toString(ColorStyleFileValue _colorStyleFileValue);
	OT_GUI_API_EXPORT ColorStyleFileValue stringToColorStyleFileValue(const std::string& _colorStyleFileValue);

	OT_GUI_API_EXPORT std::string toString(ColorStyleBaseFileMacro _colorStyleBaseFileMacro);
	OT_GUI_API_EXPORT ColorStyleBaseFileMacro stringToColorStyleBaseFileMacro(const std::string& _colorStyleBaseFileMacro);

	OT_GUI_API_EXPORT std::string toString(ColorStyleFileEntry _colorStyleFileEntry);
	OT_GUI_API_EXPORT ColorStyleFileEntry stringToColorStyleFileEntry(const std::string& _colorStyleFileEntry);

	OT_GUI_API_EXPORT std::string toString(ColorStyleValueEntry _colorStyleValueEntry);
	OT_GUI_API_EXPORT ColorStyleValueEntry stringToColorStyleValueEntry(const std::string& _colorStyleValueEntry);
	OT_GUI_API_EXPORT std::list<ColorStyleValueEntry> getAllColorStyleValueEntries();
	OT_GUI_API_EXPORT std::list<std::string> getAllColorStyleValueEntryStrings();

	OT_GUI_API_EXPORT std::string toString(ColorStyleIntegerEntry _colorStyleIntegerEntry);
	OT_GUI_API_EXPORT ColorStyleIntegerEntry stringToColorStyleIntegerEntry(const std::string& _colorStyleIntegerEntry);

	OT_GUI_API_EXPORT std::string toString(ColorStyleDoubleEntry _colorStyleDoubleEntry);
	OT_GUI_API_EXPORT ColorStyleDoubleEntry stringToColorStyleDoubleEntry(const std::string& _colorStyleDoubleEntry);

}