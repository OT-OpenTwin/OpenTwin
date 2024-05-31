//! @file ColorStyleTypes.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/ColorStyleTypes.h"

std::string ot::toString(ColorStyleName _colorStyleName) {
	switch (_colorStyleName)
	{
	case ot::ColorStyleName::BrightStyle: return "Bright";
	case ot::ColorStyleName::DarkStyle: return "Dark";
	case ot::ColorStyleName::BlueStyle: return "Blue";
	default:
		OT_LOG_EAS("Unknown ColorStyleName (" + std::to_string((int)_colorStyleName) + ")");
		return "Bright";
	}
}

ot::ColorStyleName ot::stringToColorStyleName(const std::string& _colorStyleName) {
	if (_colorStyleName == toString(ColorStyleName::BrightStyle)) return ColorStyleName::BrightStyle;
	else if (_colorStyleName == toString(ColorStyleName::DarkStyle)) return ColorStyleName::DarkStyle;
	else if (_colorStyleName == toString(ColorStyleName::BlueStyle)) return ColorStyleName::BlueStyle;
	else {
		OT_LOG_EAS("Unknown ColorStyleName \"" + _colorStyleName + "\"");
		return ColorStyleName::BrightStyle;
	}
}

std::string ot::toString(ColorStyleFileKey _colorStyleFileKey) {
	switch (_colorStyleFileKey)
	{
	case ot::ColorStyleFileKey::NameKey: return "name:";
	case ot::ColorStyleFileKey::IntegerKey: return "integers:";
	case ot::ColorStyleFileKey::DoubleKey: return "doubles:";
	case ot::ColorStyleFileKey::FileKey: return "files:";
	case ot::ColorStyleFileKey::PainterKey: return "painter:";
	case ot::ColorStyleFileKey::SheetKey: return "sheet:";
	default:
		OT_LOG_EAS("Unknown ColorStyleFileKey (" + std::to_string((int)_colorStyleFileKey) + ")");
		return "name:";
	}
}

ot::ColorStyleFileKey ot::stringToColorStyleFileKey(const std::string& _colorStyleFileKey) {
	if (_colorStyleFileKey == toString(ColorStyleFileKey::NameKey)) return ColorStyleFileKey::NameKey;
	else if (_colorStyleFileKey == toString(ColorStyleFileKey::IntegerKey)) return ColorStyleFileKey::IntegerKey;
	else if (_colorStyleFileKey == toString(ColorStyleFileKey::DoubleKey)) return ColorStyleFileKey::DoubleKey;
	else if (_colorStyleFileKey == toString(ColorStyleFileKey::FileKey)) return ColorStyleFileKey::FileKey;
	else if (_colorStyleFileKey == toString(ColorStyleFileKey::PainterKey)) return ColorStyleFileKey::PainterKey;
	else if (_colorStyleFileKey == toString(ColorStyleFileKey::SheetKey)) return ColorStyleFileKey::SheetKey;
	else {
		OT_LOG_EAS("Unknown ColorStyleFileKey \"" + _colorStyleFileKey + "\"");
		return ColorStyleFileKey::NameKey;
	}
}

std::string ot::toString(ColorStyleFileMacro _colorStyleFileMacro) {
	switch (_colorStyleFileMacro)
	{
	case ot::ColorStyleFileMacro::RootMacro: return "%root%";
	default:
		OT_LOG_EAS("Unknown ColorStyleFileMacro (" + std::to_string((int)_colorStyleFileMacro) + ")");
		return "%root%";
	}
}

ot::ColorStyleFileMacro ot::stringToColorStyleFileMacro(const std::string& _colorStyleFileMacro) {
	if (_colorStyleFileMacro == toString(ColorStyleFileMacro::RootMacro)) return ColorStyleFileMacro::RootMacro;
	else {
		OT_LOG_EAS("Unknown ColorStyleFileMacro \"" + _colorStyleFileMacro + "\"");
		return ColorStyleFileMacro::RootMacro;
	}
}

std::string ot::toString(ColorStyleFileValue _colorStyleFileValue) {
	switch (_colorStyleFileValue)
	{
	case ot::ColorStyleFileValue::NameValue: return "Name";
	case ot::ColorStyleFileValue::PathValue: return "Path";
	case ot::ColorStyleFileValue::ValueValue: return "Value";
	default:
		OT_LOG_EAS("Unknown ColorStyleFileValue (" + std::to_string((int)_colorStyleFileValue) + ")");
		return "Name";
	}
}

ot::ColorStyleFileValue ot::stringToColorStyleFileValue(const std::string& _colorStyleFileValue) {
	if (_colorStyleFileValue == toString(ColorStyleFileValue::NameValue)) return ColorStyleFileValue::NameValue;
	else if (_colorStyleFileValue == toString(ColorStyleFileValue::PathValue)) return ColorStyleFileValue::PathValue;
	else if (_colorStyleFileValue == toString(ColorStyleFileValue::ValueValue)) return ColorStyleFileValue::ValueValue;
	else {
		OT_LOG_EAS("Unknown ColorStyleFileValue \"" + _colorStyleFileValue + "\"");
		return ColorStyleFileValue::NameValue;
	}
}

std::string ot::toString(ColorStyleBaseFileMacro _colorStyleBaseFileMacro) {
	switch (_colorStyleBaseFileMacro)
	{
	case ot::ColorStyleBaseFileMacro::PainterMacro: return "color";
	case ot::ColorStyleBaseFileMacro::FileMacro: return "file";
	case ot::ColorStyleBaseFileMacro::IntMacro: return "int";
	case ot::ColorStyleBaseFileMacro::DoubleMacro: return "double";
	default:
		OT_LOG_EAS("Unknown ColorStyleBaseFileMacro (" + std::to_string((int)_colorStyleBaseFileMacro) + ")");
		return "color";
	}
}

ot::ColorStyleBaseFileMacro ot::stringToColorStyleBaseFileMacro(const std::string& _colorStyleBaseFileMacro) {
	if (_colorStyleBaseFileMacro == toString(ColorStyleBaseFileMacro::PainterMacro)) return ColorStyleBaseFileMacro::PainterMacro;
	else if (_colorStyleBaseFileMacro == toString(ColorStyleBaseFileMacro::FileMacro)) return ColorStyleBaseFileMacro::FileMacro;
	else if (_colorStyleBaseFileMacro == toString(ColorStyleBaseFileMacro::IntMacro)) return ColorStyleBaseFileMacro::IntMacro;
	else if (_colorStyleBaseFileMacro == toString(ColorStyleBaseFileMacro::DoubleMacro)) return ColorStyleBaseFileMacro::DoubleMacro;
	else {
		OT_LOG_EAS("Unknown ColorStyleBaseFileMacro \"" + _colorStyleBaseFileMacro + "\"");
		return ColorStyleBaseFileMacro::PainterMacro;
	}
}

std::string ot::toString(ColorStyleFileEntry _colorStyleFileEntry) {
	switch (_colorStyleFileEntry)
	{
	case ot::ColorStyleFileEntry::PropertyItemDeleteIcon: return "Property Item Delete Icon";
	case ot::ColorStyleFileEntry::PropertyGroupExpandedIcon: return "Property Group Expanded Icon";
	case ot::ColorStyleFileEntry::PropertyGroupCollapsedIcon: return "Property Group Collapsed Icon";
	case ot::ColorStyleFileEntry::LogInBackgroundImage: return "Log In Background Image";
	default:
		OT_LOG_EAS("Unknown ColorStyleFileEntry (" + std::to_string((int)_colorStyleFileEntry) + ")");
		return "Property Item Delete Icon";
	}
}

ot::ColorStyleFileEntry ot::stringToColorStyleFileEntry(const std::string& _colorStyleFileEntry) {
	if (_colorStyleFileEntry == toString(ColorStyleFileEntry::LogInBackgroundImage)) return ColorStyleFileEntry::LogInBackgroundImage;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::PropertyGroupCollapsedIcon)) return ColorStyleFileEntry::PropertyGroupCollapsedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::PropertyGroupExpandedIcon)) return ColorStyleFileEntry::PropertyGroupExpandedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::PropertyItemDeleteIcon)) return ColorStyleFileEntry::PropertyItemDeleteIcon;
	else {
		OT_LOG_EAS("Unknown ColorStyleFileEntry \"" + _colorStyleFileEntry + "\"");
		return ColorStyleFileEntry::LogInBackgroundImage;
	}
}

std::string ot::toString(ColorStylePainterEntry _colorStylePainterEntry) {
	switch (_colorStylePainterEntry)
	{
	case ot::ColorStylePainterEntry::WidgetBackground: return "Widget Background";
	case ot::ColorStylePainterEntry::WidgetAlternateBackground: return "Widget Alternate Background";
	case ot::ColorStylePainterEntry::WidgetForeground: return "Widget Foreground";
	case ot::ColorStylePainterEntry::WidgetDisabledBackground: return "Widget Disabled Background";
	case ot::ColorStylePainterEntry::WidgetDisabledForeground: return "Widget Disabled Foreground";

	case ot::ColorStylePainterEntry::WidgetHoverBackground: return "Widget Hover Background";
	case ot::ColorStylePainterEntry::WidgetHoverForeground: return "Widget Hover Foreground";
	case ot::ColorStylePainterEntry::WidgetSelectionBackground: return "Widget Selection Background";
	case ot::ColorStylePainterEntry::WidgetSelectionForeground: return "Widget Selection Foreground";

	case ot::ColorStylePainterEntry::TitleBackground: return "Title Background";
	case ot::ColorStylePainterEntry::TitleForeground: return "Title Foreground";
	case ot::ColorStylePainterEntry::TitleBorder: return "Title Border";

	case ot::ColorStylePainterEntry::HeaderBackground: return "Header Background";
	case ot::ColorStylePainterEntry::HeaderHoverBackground: return "Header Hover Background";
	case ot::ColorStylePainterEntry::HeaderSelectionBackground: return "Header Selection Background";

	case ot::ColorStylePainterEntry::InputBackground: return "Input Background";
	case ot::ColorStylePainterEntry::InputForeground: return "Input Foreground";

	case ot::ColorStylePainterEntry::TextEditorLineBorder: return "TextEditor Line Border";
	case ot::ColorStylePainterEntry::TextEditorHighlightBackground: return "TextEditor Highlight Background";

	case ot::ColorStylePainterEntry::DialogBackground: return "Dialog Background";
	case ot::ColorStylePainterEntry::DialogForeground: return "Dialog Foreground";

	case ot::ColorStylePainterEntry::WindowBackground: return "Window Background";
	case ot::ColorStylePainterEntry::WindowForeground: return "Window Foreground";

	case ot::ColorStylePainterEntry::Border: return "Border Color";
	case ot::ColorStylePainterEntry::BorderLight: return "Border Light Color";
	case ot::ColorStylePainterEntry::BorderDisabled: return "Border Disabled Color";
	case ot::ColorStylePainterEntry::BorderHover: return "Border Hover Color";
	case ot::ColorStylePainterEntry::BorderSelection: return "Border Selection Color";

	case ot::ColorStylePainterEntry::ToolBarFirstTabBackground: return "TabToolBar First Tab Background";
	case ot::ColorStylePainterEntry::ToolBarFirstTabForeground: return "TabToolBar First Tab Foreground";

	case ot::ColorStylePainterEntry::ErrorForeground: return "Error Foreground";

	default:
		OT_LOG_EAS("Unknown ColorStylePainterEntry (" + std::to_string((int)_colorStylePainterEntry) + ")");
		return "Property Item Delete Icon";
	}
}

ot::ColorStylePainterEntry ot::stringToColorStylePainterEntry(const std::string& _colorStylePainterEntry) {
	if (_colorStylePainterEntry == toString(ColorStylePainterEntry::WidgetBackground)) return ColorStylePainterEntry::WidgetBackground;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::WidgetAlternateBackground)) return ColorStylePainterEntry::WidgetAlternateBackground;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::WidgetForeground)) return ColorStylePainterEntry::WidgetForeground;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::WidgetDisabledBackground)) return ColorStylePainterEntry::WidgetDisabledBackground;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::WidgetDisabledForeground)) return ColorStylePainterEntry::WidgetDisabledForeground;

	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::WidgetHoverBackground)) return ColorStylePainterEntry::WidgetHoverBackground;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::WidgetHoverForeground)) return ColorStylePainterEntry::WidgetHoverForeground;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::WidgetSelectionBackground)) return ColorStylePainterEntry::WidgetSelectionBackground;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::WidgetSelectionForeground)) return ColorStylePainterEntry::WidgetSelectionForeground;

	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::TitleBackground)) return ColorStylePainterEntry::TitleBackground;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::TitleForeground)) return ColorStylePainterEntry::TitleForeground;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::TitleBorder)) return ColorStylePainterEntry::TitleBorder;

	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::HeaderBackground)) return ColorStylePainterEntry::HeaderBackground;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::HeaderHoverBackground)) return ColorStylePainterEntry::HeaderHoverBackground;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::HeaderSelectionBackground)) return ColorStylePainterEntry::HeaderSelectionBackground;

	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::InputBackground)) return ColorStylePainterEntry::InputBackground;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::InputForeground)) return ColorStylePainterEntry::InputForeground;

	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::TextEditorLineBorder)) return ColorStylePainterEntry::TextEditorLineBorder;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::TextEditorHighlightBackground)) return ColorStylePainterEntry::TextEditorHighlightBackground;

	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::DialogBackground)) return ColorStylePainterEntry::DialogBackground;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::DialogForeground)) return ColorStylePainterEntry::DialogForeground;

	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::WindowBackground)) return ColorStylePainterEntry::WindowBackground;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::WindowForeground)) return ColorStylePainterEntry::WindowForeground;

	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::Border)) return ColorStylePainterEntry::Border;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::BorderLight)) return ColorStylePainterEntry::BorderLight;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::BorderDisabled)) return ColorStylePainterEntry::BorderDisabled;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::BorderHover)) return ColorStylePainterEntry::BorderHover;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::BorderSelection)) return ColorStylePainterEntry::BorderSelection;

	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::ToolBarFirstTabBackground)) return ColorStylePainterEntry::ToolBarFirstTabBackground;
	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::ToolBarFirstTabForeground)) return ColorStylePainterEntry::ToolBarFirstTabForeground;

	else if (_colorStylePainterEntry == toString(ColorStylePainterEntry::ErrorForeground)) return ColorStylePainterEntry::ErrorForeground;
	else {
		OT_LOG_EAS("Unknown ColorStylePainterEntry \"" + _colorStylePainterEntry + "\"");
		return ColorStylePainterEntry::WidgetBackground;
	}
}

std::string ot::toString(ColorStyleIntegerEntry _colorStyleIntegerEntry) {
	switch (_colorStyleIntegerEntry)
	{
	case ot::ColorStyleIntegerEntry::SplitterHandleWidth: return "Splitter Handle Width";
	case ot::ColorStyleIntegerEntry::SplitterBorderRadius: return "Splitter Border Radius";
	default:
		OT_LOG_EAS("Unknown ColorStyleIntegerEntry (" + std::to_string((int)_colorStyleIntegerEntry) + ")");
		return "Splitter Handle Width";
	}
}

ot::ColorStyleIntegerEntry ot::stringToColorStyleIntegerEntry(const std::string& _colorStyleIntegerEntry) {
	if (_colorStyleIntegerEntry == toString(ColorStyleIntegerEntry::SplitterHandleWidth)) return ColorStyleIntegerEntry::SplitterHandleWidth;
	else if (_colorStyleIntegerEntry == toString(ColorStyleIntegerEntry::SplitterBorderRadius)) return ColorStyleIntegerEntry::SplitterBorderRadius;
	else {
		OT_LOG_EAS("Unknown ColorStyleIntegerEntry \"" + _colorStyleIntegerEntry + "\"");
		return ColorStyleIntegerEntry::SplitterHandleWidth;
	}
}

std::string ot::toString(ColorStyleDoubleEntry _colorStyleDoubleEntry) {
	switch (_colorStyleDoubleEntry)
	{
	case ot::ColorStyleDoubleEntry::BorderRadiusBig: return "Border Radius Big";
	case ot::ColorStyleDoubleEntry::BorderRadiusSmall: return "Border Radius Small";
	case ot::ColorStyleDoubleEntry::ToolTipOpacity: return "ToolTip Opacity";
	default:
		OT_LOG_EAS("Unknown ColorStyleDoubleEntry (" + std::to_string((int)_colorStyleDoubleEntry) + ")");
		return "Splitter Handle Width";
	}
}

ot::ColorStyleDoubleEntry ot::stringToColorStyleDoubleEntry(const std::string& _colorStyleDoubleEntry) {
	if (_colorStyleDoubleEntry == toString(ColorStyleDoubleEntry::BorderRadiusBig)) return ColorStyleDoubleEntry::BorderRadiusBig;
	else if (_colorStyleDoubleEntry == toString(ColorStyleDoubleEntry::BorderRadiusSmall)) return ColorStyleDoubleEntry::BorderRadiusSmall;
	else if (_colorStyleDoubleEntry == toString(ColorStyleDoubleEntry::ToolTipOpacity)) return ColorStyleDoubleEntry::ToolTipOpacity;
	else {
		OT_LOG_EAS("Unknown ColorStyleDoubleEntry \"" + _colorStyleDoubleEntry + "\"");
		return ColorStyleDoubleEntry::BorderRadiusBig;
	}
}
