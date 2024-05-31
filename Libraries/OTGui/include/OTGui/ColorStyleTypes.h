//! \file ColorStyleTypes.h
//! \author Alexander Kuester (alexk95)
//! \date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

#define OT_COLORSTYLE_NAME_Bright "Bright"
#define OT_COLORSTYLE_NAME_Dark "Dark"
#define OT_COLORSTYLE_NAME_Blue "Blue"

#define OT_COLORSTYLE_FILE_KEY_Name "name:"
#define OT_COLORSTYLE_FILE_KEY_Integers "integers:"
#define OT_COLORSTYLE_FILE_KEY_Doubles "doubles:"
#define OT_COLORSTYLE_FILE_KEY_Files "files:"
#define OT_COLORSTYLE_FILE_KEY_Values "values:"
#define OT_COLORSTYLE_FILE_KEY_StyleSheet "sheet:"

#define OT_COLORSTYLE_FILE_MACRO_Root "%root%"

#define OT_COLORSTYLE_FILE_VALUE_Name "Name"
#define OT_COLORSTYLE_FILE_VALUE_Path "Path"
#define OT_COLORSTYLE_FILE_VALUE_Value "Value"

#define OT_COLORSTYLE_BASEFILE_MACRO_Color "color"
#define OT_COLORSTYLE_BASEFILE_MACRO_File "file"
#define OT_COLORSTYLE_BASEFILE_MACRO_Int "int"
#define OT_COLORSTYLE_BASEFILE_MACRO_Double "number"

#define OT_COLORSTYLE_FILE_PropertyItemDelete "Property Item Delete"
#define OT_COLORSTYLE_FILE_PropertyGroupExpanded "Property Group Expanded"
#define OT_COLORSTYLE_FILE_PropertyGroupCollapsed "Property Group Collapsed"

#define OT_COLORSTYLE_VALUE_ControlsBackground "Controls Background"
#define OT_COLORSTYLE_VALUE_ControlsDisabledBackground "Controls Disabled Background"
#define OT_COLORSTYLE_VALUE_ControlsForeground "Controls Foreground"
#define OT_COLORSTYLE_VALUE_ControlsHoverBackground "Controls Hover Background"
#define OT_COLORSTYLE_VALUE_ControlsHoverForeground "Controls Hover Foreground"
#define OT_COLORSTYLE_VALUE_ControlsSelectedBackground "Controls Selected Background"
#define OT_COLORSTYLE_VALUE_ControlsSelectedForeground "Controls Selected Foreground"
#define OT_COLORSTYLE_VALUE_ControlsBorderColor "Controls Border Color"
#define OT_COLORSTYLE_VALUE_TitleBackground "Title Background"
#define OT_COLORSTYLE_VALUE_TitleForeground "Title Foreground"
#define OT_COLORSTYLE_VALUE_WindowBackground "Window Background"
#define OT_COLORSTYLE_VALUE_WindowForeground "Window Foreground"
#define OT_COLORSTYLE_VALUE_ErrorForeground "Error Foreground"
#define OT_COLORSTYLE_VALUE_TextEditHighlightBorder "Text Editor Highlight Border"
#define OT_COLORSTYLE_VALUE_TextEditHighlight "Text Editor Highlight"

#define OT_COLORSTYLE_INT_SplitterHandleWidth "Splitter Handle Width"

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
		PropertyItemDeleteIcon,
		PropertyGroupExpandedIcon,
		PropertyGroupCollapsedIcon,
		LogInBackgroundImage
	};

	//! \brief Contains default painters set in a color style.
	enum class ColorStylePainterEntry {
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
		HeaderHoverBackground,
		HeaderSelectionBackground,

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

		ErrorForeground
	};

	//! \brief Contains default integer values set in a color style.
	enum class ColorStyleIntegerEntry {
		SplitterHandleWidth,
		SplitterBorderRadius
	};

	//! \brief Contains default double values set in a color style.
	enum class ColorStyleDoubleEntry {
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

	OT_GUI_API_EXPORT std::string toString(ColorStylePainterEntry _colorStylePainterEntry);
	OT_GUI_API_EXPORT ColorStylePainterEntry stringToColorStylePainterEntry(const std::string& _colorStylePainterEntry);

	OT_GUI_API_EXPORT std::string toString(ColorStyleIntegerEntry _colorStyleIntegerEntry);
	OT_GUI_API_EXPORT ColorStyleIntegerEntry stringToColorStyleIntegerEntry(const std::string& _colorStyleIntegerEntry);

	OT_GUI_API_EXPORT std::string toString(ColorStyleDoubleEntry _colorStyleDoubleEntry);
	OT_GUI_API_EXPORT ColorStyleDoubleEntry stringToColorStyleDoubleEntry(const std::string& _colorStyleDoubleEntry);

}