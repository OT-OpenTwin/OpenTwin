//! \file ColorStyleTypes.h
//! \author Alexander Kuester (alexk95)
//! \date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>
#include <string>

namespace ot {

	//! \brief Contains default OpenTwin color style names.
	enum class ColorStyleName {
		BrightStyle,
		DarkStyle,
		BlueStyle
	};

	//! \brief Contains color style file keys.
	//! The keys are used to determine the beginning of a section in the color style file.
	enum class ColorStyleFileKey {
		NameKey,
		IntegerKey,
		DoubleKey,
		FileKey,
		PainterKey,
		SheetKey
	};

	//! \brief Contains color style file macros.
	enum class ColorStyleFileMacro {
		RootMacro
	};

	enum class ColorStyleFileValue {
		NameValue,
		PathValue,
		ValueValue
	};

	//! \brief Contains macros used in the color style base file.
	enum class ColorStyleBaseFileMacro {
		PainterMacro,
		FileMacro,
		IntMacro,
		DoubleMacro
	};

	//! \brief Contains default file paths set in a color style.
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

		//! \brief Used to determine the last entry.
		//! \warning This must always be the last entry in this enum.
		ColorStyleFileEntry_End
	};

	//! \brief Contains default painters set in a color style.
	enum class ColorStyleValueEntry {
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
		InputForeground,

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

		ToolBarFirstTabBackground,
		ToolBarFirstTabForeground,

		GraphicsItemBorder,
		GraphicsItemConnection,
		GraphicsItemBackground,
		GraphicsItemForeground,
		GraphicsItemSelectionBorder,
		GraphicsItemHoverBorder,
		GraphicsItemConnectableBackground,
		GraphicsItemLineColor,

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

		ErrorForeground,

		//! \brief Used to determine the last entry.
		//! \warning This must always be the last entry in this enum.
		ColorStyleValueEntry_End
	};

	//! \brief Contains default integer values set in a color style.
	enum class ColorStyleIntegerEntry {
		SplitterHandleWidth,
		SplitterBorderRadius
	};

	//! \brief Contains default double values set in a color style.
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
	OT_GUI_API_EXPORT std::list<std::string> getAllColorStyleValueEntries(void);

	OT_GUI_API_EXPORT std::string toString(ColorStyleIntegerEntry _colorStyleIntegerEntry);
	OT_GUI_API_EXPORT ColorStyleIntegerEntry stringToColorStyleIntegerEntry(const std::string& _colorStyleIntegerEntry);

	OT_GUI_API_EXPORT std::string toString(ColorStyleDoubleEntry _colorStyleDoubleEntry);
	OT_GUI_API_EXPORT ColorStyleDoubleEntry stringToColorStyleDoubleEntry(const std::string& _colorStyleDoubleEntry);

}