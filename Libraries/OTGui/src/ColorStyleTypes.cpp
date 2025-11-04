// @otlicense
// File: ColorStyleTypes.cpp
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

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/ColorStyleTypes.h"

// std header
#include <map>

namespace ot {
	namespace intern {

		static std::map<ot::ColorStyleFileEntry, std::string>& colorStyleFileEntryMap() {
			static std::map<ot::ColorStyleFileEntry, std::string> g_instance {
				{ ot::ColorStyleFileEntry::TransparentIcon, "Transparent Icon" },

				{ ot::ColorStyleFileEntry::ArrowUpIcon, "Arrow Up Icon" },
				{ ot::ColorStyleFileEntry::ArrowUpDisabledIcon, "Arrow Up Disabled Icon" },
				{ ot::ColorStyleFileEntry::ArrowUpFocusIcon, "Arrow Up Focus Icon" },
				{ ot::ColorStyleFileEntry::ArrowUpPressed, "Arrow Up Pressed Icon" },
				{ ot::ColorStyleFileEntry::ArrowDownIcon, "Arrow Down Icon" },
				{ ot::ColorStyleFileEntry::ArrowDownDisabledIcon, "Arrow Down Disabled Icon" },
				{ ot::ColorStyleFileEntry::ArrowDownFocusIcon, "Arrow Down Focus Icon" },
				{ ot::ColorStyleFileEntry::ArrowDownPressed, "Arrow Down Pressed Icon" },
				{ ot::ColorStyleFileEntry::ArrowLeftIcon, "Arrow Left Icon" },
				{ ot::ColorStyleFileEntry::ArrowLeftDisabledIcon, "Arrow Left Disabled Icon" },
				{ ot::ColorStyleFileEntry::ArrowLeftFocusIcon, "Arrow Left Focus Icon" },
				{ ot::ColorStyleFileEntry::ArrowLeftPressed, "Arrow Left Pressed Icon" },
				{ ot::ColorStyleFileEntry::ArrowRightIcon, "Arrow Right Icon" },
				{ ot::ColorStyleFileEntry::ArrowRightDisabledIcon, "Arrow Right Disabled Icon" },
				{ ot::ColorStyleFileEntry::ArrowRightFocusIcon, "Arrow Right Focus Icon" },
				{ ot::ColorStyleFileEntry::ArrowRightPressed, "Arrow Right Pressed Icon" },

				{ ot::ColorStyleFileEntry::TreeBranchClosedIcon, "Tree Branch Closed Icon" },
				{ ot::ColorStyleFileEntry::TreeBranchClosedFocusIcon, "Tree Branch Closed Focus Icon" },
				{ ot::ColorStyleFileEntry::TreeBranchEndIcon, "Tree Branch End Icon" },
				{ ot::ColorStyleFileEntry::TreeBranchLineIcon, "Tree Branch Line Icon" },
				{ ot::ColorStyleFileEntry::TreeBranchMoreIcon, "Tree Branch More Icon" },
				{ ot::ColorStyleFileEntry::TreeBranchOpenIcon, "Tree Branch Open Icon" },
				{ ot::ColorStyleFileEntry::TreeBranchOpenFocusIcon, "Tree Branch Open Focus Icon" },

				{ ot::ColorStyleFileEntry::CheckBoxCheckedIcon, "CheckBox Checked Icon" },
				{ ot::ColorStyleFileEntry::CheckBoxCheckedDisabledIcon, "CheckBox Checked Disabled Icon" },
				{ ot::ColorStyleFileEntry::CheckBoxCheckedFocusIcon, "CheckBox Checked Focus Icon" },
				{ ot::ColorStyleFileEntry::CheckBoxCheckedPressedIcon, "CheckBox Checked Pressed Icon" },
				{ ot::ColorStyleFileEntry::CheckBoxIndeterminateIcon, "CheckBox Indeterminate Icon" },
				{ ot::ColorStyleFileEntry::CheckBoxIndeterminateDisabledIcon, "CheckBox Indeterminate Disabled Icon" },
				{ ot::ColorStyleFileEntry::CheckBoxIndeterminateFocusIcon, "CheckBox Indeterminate Focus Icon" },
				{ ot::ColorStyleFileEntry::CheckBoxIndeterminatePressedIcon, "CheckBox Indeterminate Pressed Icon" },
				{ ot::ColorStyleFileEntry::CheckBoxUncheckedIcon, "CheckBox Unchecked Icon" },
				{ ot::ColorStyleFileEntry::CheckBoxUncheckedDisabledIcon, "CheckBox Unchecked Disabled Icon" },
				{ ot::ColorStyleFileEntry::CheckBoxUncheckedFocusIcon, "CheckBox Unchecked Focus Icon" },
				{ ot::ColorStyleFileEntry::CheckBoxUncheckedPressedIcon, "CheckBox Unchecked Pressed Icon" },

				{ ot::ColorStyleFileEntry::LogInCheckBoxCheckedIcon, "LogIn CheckBox Checked Icon" },
				{ ot::ColorStyleFileEntry::LogInCheckBoxCheckedDisabledIcon, "LogIn CheckBox Checked Disabled Icon" },
				{ ot::ColorStyleFileEntry::LogInCheckBoxCheckedFocusIcon, "LogIn CheckBox Checked Focus Icon" },
				{ ot::ColorStyleFileEntry::LogInCheckBoxCheckedPressedIcon, "LogIn CheckBox Checked Pressed Icon" },
				{ ot::ColorStyleFileEntry::LogInCheckBoxIndeterminateIcon, "LogIn CheckBox Indeterminate Icon" },
				{ ot::ColorStyleFileEntry::LogInCheckBoxIndeterminateDisabledIcon, "LogIn CheckBox Indeterminate Disabled Icon" },
				{ ot::ColorStyleFileEntry::LogInCheckBoxIndeterminateFocusIcon, "LogIn CheckBox Indeterminate Focus Icon" },
				{ ot::ColorStyleFileEntry::LogInCheckBoxIndeterminatePressedIcon, "LogIn CheckBox Indeterminate Pressed Icon" },
				{ ot::ColorStyleFileEntry::LogInCheckBoxUncheckedIcon, "LogIn CheckBox Unchecked Icon" },
				{ ot::ColorStyleFileEntry::LogInCheckBoxUncheckedDisabledIcon, "LogIn CheckBox Unchecked Disabled Icon" },
				{ ot::ColorStyleFileEntry::LogInCheckBoxUncheckedFocusIcon, "LogIn CheckBox Unchecked Focus Icon" },
				{ ot::ColorStyleFileEntry::LogInCheckBoxUncheckedPressedIcon, "LogIn CheckBox Unchecked Pressed Icon" },

				{ ot::ColorStyleFileEntry::LogInBackgroundImage, "Log In Background Image" },

				{ ot::ColorStyleFileEntry::RadioButtonCheckedIcon, "RadioButton Checked Icon" },
				{ ot::ColorStyleFileEntry::RadioButtonCheckedDisabledIcon, "RadioButton Checked Disabled Icon" },
				{ ot::ColorStyleFileEntry::RadioButtonCheckedFocusIcon, "RadioButton Checked Focus Icon" },
				{ ot::ColorStyleFileEntry::RadioButtonUncheckedIcon, "RadioButton Unchecked Icon" },
				{ ot::ColorStyleFileEntry::RadioButtonUncheckedDisabledIcon, "RadioButton Unchecked Disabled Icon" },
				{ ot::ColorStyleFileEntry::RadioButtonUncheckedFocusIcon, "RadioButton Unchecked Focus Icon" },

				{ ot::ColorStyleFileEntry::ToolBarMoveHorizontalIcon, "ToolBar Move Horizontal Icon" },
				{ ot::ColorStyleFileEntry::ToolBarMoveVerticalIcon, "ToolBar Move Vertical Icon" },
				{ ot::ColorStyleFileEntry::ToolBarSeparatorHorizontalIcon, "ToolBar Separator Horizontal Icon" },
				{ ot::ColorStyleFileEntry::ToolBarSeparatorVerticalIcon, "ToolBar Separator Vertical Icon" },

				{ ot::ColorStyleFileEntry::HeaderFilterIcon, "Header Filter Icon" },
				{ ot::ColorStyleFileEntry::HeaderFilterHoverIcon, "Header Filter Hover Icon" },
				{ ot::ColorStyleFileEntry::HeaderFilterPressedIcon, "Header Filter Pressed Icon" },
				{ ot::ColorStyleFileEntry::HeaderFilterDisabledIcon, "Header Filter Disabled Icon" },

				{ ot::ColorStyleFileEntry::WindowCloseIcon, "Window Close Icon" },
				{ ot::ColorStyleFileEntry::WindowCloseDisabledIcon, "Window Close Disabled Icon" },
				{ ot::ColorStyleFileEntry::WindowCloseFocusIcon, "Window Close Focus Icon" },
				{ ot::ColorStyleFileEntry::WindowClosePressedIcon, "Window Close Pressed Icon" },
				{ ot::ColorStyleFileEntry::WindowPinIcon, "Window Pin Icon" },
				{ ot::ColorStyleFileEntry::WindowUnpinIcon, "Window Unpin Icon" },
				{ ot::ColorStyleFileEntry::WindowPinDisabledIcon, "Window Pin Disabled Icon" },
				{ ot::ColorStyleFileEntry::WindowUnpinDisabledIcon, "Window Unpin Disabled Icon" },
				{ ot::ColorStyleFileEntry::WindowPinFocusIcon, "Window Pin Focus Icon" },
				{ ot::ColorStyleFileEntry::WindowUnpinFocusIcon, "Window Unpin Focus Icon" },
				{ ot::ColorStyleFileEntry::WindowPinPressedIcon, "Window Pin Pressed Icon" },
				{ ot::ColorStyleFileEntry::WindowUnpinPressedIcon, "Window Unpin Pressed Icon" },
				{ ot::ColorStyleFileEntry::WindowLockIcon, "Window Lock Icon" },
				{ ot::ColorStyleFileEntry::WindowUnlockIcon, "Window Unlock Icon" },
				{ ot::ColorStyleFileEntry::WindowLockDisabledIcon, "Window Lock Disabled Icon" },
				{ ot::ColorStyleFileEntry::WindowUnlockDisabledIcon, "Window Unlock Disabled Icon" },
				{ ot::ColorStyleFileEntry::WindowLockFocusIcon, "Window Lock Focus Icon" },
				{ ot::ColorStyleFileEntry::WindowUnlockFocusIcon, "Window Unlock Focus Icon" },
				{ ot::ColorStyleFileEntry::WindowLockPressedIcon, "Window Lock Pressed Icon" },
				{ ot::ColorStyleFileEntry::WindowUnlockPressedIcon, "Window Unlock Pressed Icon" },
				{ ot::ColorStyleFileEntry::WindowGripIcon, "Window Grip Icon" },
				{ ot::ColorStyleFileEntry::WindowUndockIcon, "Window Undock Icon" },
				{ ot::ColorStyleFileEntry::WindowUndockDisabledIcon, "Window Undock Disabled Icon" },
				{ ot::ColorStyleFileEntry::WindowUndockFocusIcon, "Window Undock Focus Icon" },
				{ ot::ColorStyleFileEntry::WindowUndockPressedIcon, "Window Undock Pressed Icon" },

				{ ot::ColorStyleFileEntry::PropertyItemDeleteIcon, "Property Item Delete Icon" },
				{ ot::ColorStyleFileEntry::PropertyGroupExpandedIcon, "Property Group Expanded Icon" },
				{ ot::ColorStyleFileEntry::PropertyGroupCollapsedIcon, "Property Group Collapsed Icon" }

			};

			return g_instance;
		}

		static std::map<ot::ColorStyleValueEntry, std::string>& colorStyleValueEntryMap() {
			static std::map<ot::ColorStyleValueEntry, std::string> g_instance {
				{ ot::ColorStyleValueEntry::Transparent, "Transparent" },

				{ ot::ColorStyleValueEntry::WidgetBackground, "Widget Background" },
				{ ot::ColorStyleValueEntry::WidgetAlternateBackground, "Widget Alternate Background" },
				{ ot::ColorStyleValueEntry::WidgetForeground, "Widget Foreground" },
				{ ot::ColorStyleValueEntry::WidgetDisabledBackground, "Widget Disabled Background" },
				{ ot::ColorStyleValueEntry::WidgetDisabledForeground, "Widget Disabled Foreground" },

				{ ot::ColorStyleValueEntry::WidgetHoverBackground, "Widget Hover Background" },
				{ ot::ColorStyleValueEntry::WidgetHoverForeground, "Widget Hover Foreground" },
				{ ot::ColorStyleValueEntry::WidgetSelectionBackground, "Widget Selection Background" },
				{ ot::ColorStyleValueEntry::WidgetSelectionForeground, "Widget Selection Foreground" },

				{ ot::ColorStyleValueEntry::TitleBackground, "Title Background" },
				{ ot::ColorStyleValueEntry::TitleForeground, "Title Foreground" },
				{ ot::ColorStyleValueEntry::TitleBorder, "Title Border" },

				{ ot::ColorStyleValueEntry::HeaderBackground, "Header Background" },
				{ ot::ColorStyleValueEntry::HeaderForeground, "Header Foreground" },
				{ ot::ColorStyleValueEntry::HeaderHoverBackground, "Header Hover Background" },
				{ ot::ColorStyleValueEntry::HeaderHoverForeground, "Header Hover Foreground" },
				{ ot::ColorStyleValueEntry::HeaderSelectionBackground, "Header Selection Background" },
				{ ot::ColorStyleValueEntry::HeaderSelectionForeground, "Header Selection Foreground" },

				{ ot::ColorStyleValueEntry::InputBackground, "Input Background" },
				{ ot::ColorStyleValueEntry::InputDisabledBackground, "Input Disabled Background" },
				{ ot::ColorStyleValueEntry::InputForeground, "Input Foreground" },
				{ ot::ColorStyleValueEntry::InputDisabledForeground, "Input Disabled Foreground" },

				{ ot::ColorStyleValueEntry::TextEditorLineBorder, "TextEditor Line Border" },
				{ ot::ColorStyleValueEntry::TextEditorHighlightBackground, "TextEditor Highlight Background" },

				{ ot::ColorStyleValueEntry::DialogBackground, "Dialog Background" },
				{ ot::ColorStyleValueEntry::DialogForeground, "Dialog Foreground" },

				{ ot::ColorStyleValueEntry::WindowBackground, "Window Background" },
				{ ot::ColorStyleValueEntry::WindowForeground, "Window Foreground" },

				{ ot::ColorStyleValueEntry::Border, "Border Color" },
				{ ot::ColorStyleValueEntry::BorderLight, "Border Light Color" },
				{ ot::ColorStyleValueEntry::BorderDisabled, "Border Disabled Color" },
				{ ot::ColorStyleValueEntry::BorderHover, "Border Hover Color" },
				{ ot::ColorStyleValueEntry::BorderSelection, "Border Selection Color" },

				{ ot::ColorStyleValueEntry::Rainbow0, "Rainbow 0" },
				{ ot::ColorStyleValueEntry::Rainbow1, "Rainbow 1" },
				{ ot::ColorStyleValueEntry::Rainbow2, "Rainbow 2" },
				{ ot::ColorStyleValueEntry::Rainbow3, "Rainbow 3" },
				{ ot::ColorStyleValueEntry::Rainbow4, "Rainbow 4" },
				{ ot::ColorStyleValueEntry::Rainbow5, "Rainbow 5" },
				{ ot::ColorStyleValueEntry::Rainbow6, "Rainbow 6" },
				{ ot::ColorStyleValueEntry::Rainbow7, "Rainbow 7" },
				{ ot::ColorStyleValueEntry::Rainbow8, "Rainbow 8" },
				{ ot::ColorStyleValueEntry::Rainbow9, "Rainbow 9" },

				{ ot::ColorStyleValueEntry::ToolBarFirstTabBackground, "TabToolBar First Tab Background" },
				{ ot::ColorStyleValueEntry::ToolBarFirstTabForeground, "TabToolBar First Tab Foreground" },

				{ ot::ColorStyleValueEntry::GraphicsItemBorder, "GraphicsItem Border Color" },
				{ ot::ColorStyleValueEntry::GraphicsItemConnection, "GraphicsItem Connection Color" },
				{ ot::ColorStyleValueEntry::GraphicsItemConnectionConnectable, "GraphicsItem Connection Connectable Color" },
				{ ot::ColorStyleValueEntry::GraphicsItemBackground, "GraphicsItem Background" },
				{ ot::ColorStyleValueEntry::GraphicsItemForeground, "GraphicsItem Foreground" },
				{ ot::ColorStyleValueEntry::GraphicsItemSelectionBorder, "GraphicsItem Selection Border" },
				{ ot::ColorStyleValueEntry::GraphicsItemHoverBorder, "GraphicsItem Hover Border" },
				{ ot::ColorStyleValueEntry::GraphicsItemConnectableBackground, "GraphicsItem Connectable Background" },
				{ ot::ColorStyleValueEntry::GraphicsItemLineColor, "GraphicsItem Line Color" },
				{ ot::ColorStyleValueEntry::GraphicsItemRubberband, "GraphicsItem Rubberband" },

				{ ot::ColorStyleValueEntry::StyledTextHighlight, "Styled Text Highlight" },
				{ ot::ColorStyleValueEntry::StyledTextLightHighlight, "Styled Light Highlight" },
				{ ot::ColorStyleValueEntry::StyledTextComment, "Styled Text Comment" },
				{ ot::ColorStyleValueEntry::StyledTextWarning, "Styled Text Warning" },
				{ ot::ColorStyleValueEntry::StyledTextError, "Styled Text Error" },

				{ ot::ColorStyleValueEntry::PythonKeyword, "Python Keyword" },
				{ ot::ColorStyleValueEntry::PythonClass, "Python Class" },
				{ ot::ColorStyleValueEntry::PythonString, "Python String" },
				{ ot::ColorStyleValueEntry::PythonFunction, "Python Function" },
				{ ot::ColorStyleValueEntry::PythonComment, "Python Comment" },

				{ ot::ColorStyleValueEntry::PlotCurve, "Plot Curve" },
				{ ot::ColorStyleValueEntry::PlotCurveDimmed, "Plot Curve Dimmed" },
				{ ot::ColorStyleValueEntry::PlotCurveHighlight, "Plot Curve Highlight" },
				{ ot::ColorStyleValueEntry::PlotCurveSymbol, "Plot Curve Symbol" },

				{ ot::ColorStyleValueEntry::SuccessForeground, "Success Foreground" },
				{ ot::ColorStyleValueEntry::WarningForeground, "Warning Foreground" },
				{ ot::ColorStyleValueEntry::ErrorForeground, "Error Foreground" }
			};

			return g_instance;
		}
	}
}

std::string ot::toString(ColorStyleName _colorStyleName) {
	switch (_colorStyleName) {
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
	const auto it = ot::intern::colorStyleFileEntryMap().find(_colorStyleFileEntry);
	if (it != ot::intern::colorStyleFileEntryMap().end()) {
		return it->second;
	}
	else {
		OT_LOG_EAS("Unknown ColorStyleFileEntry (" + std::to_string((int)_colorStyleFileEntry) + ")");
		return "Transparent Icon";
	}
}

ot::ColorStyleFileEntry ot::stringToColorStyleFileEntry(const std::string& _colorStyleFileEntry) {
	for (const auto& it : ot::intern::colorStyleFileEntryMap()) {
		if (it.second == _colorStyleFileEntry) {
			return it.first;
		}
	}

	OT_LOG_EAS("Unknown ColorStyleFileEntry \"" + _colorStyleFileEntry + "\"");
	return ColorStyleFileEntry::TransparentIcon;
}

std::string ot::toString(ColorStyleValueEntry _colorStyleValueEntry) {
	const auto it = ot::intern::colorStyleValueEntryMap().find(_colorStyleValueEntry);
	if (it != ot::intern::colorStyleValueEntryMap().end()) {
		return it->second;
	}
	else {
		OT_LOG_EAS("Unknown ColorStyleValueEntry (" + std::to_string((int)_colorStyleValueEntry) + ")");
		return "Transparent";
	}
}

ot::ColorStyleValueEntry ot::stringToColorStyleValueEntry(const std::string& _colorStyleValueEntry) {
	for (const auto& it : ot::intern::colorStyleValueEntryMap()) {
		if (it.second == _colorStyleValueEntry) {
			return it.first;
		}
	}

	// Legacy support for "Header Foreground"
	if (_colorStyleValueEntry == "Header Foregorund") {
		return ColorStyleValueEntry::HeaderForeground;
	}

	OT_LOG_EAS("Unknown ColorStyleValueEntry \"" + _colorStyleValueEntry + "\"");
	return ColorStyleValueEntry::Transparent;
}

std::list<std::string> ot::getAllColorStyleValueEntryStrings() {
	std::list<std::string> ret;
	for (int i = 0; i < (int)ColorStyleValueEntry::ColorStyleValueEntry_End; i++) {
		ret.push_back(toString((ColorStyleValueEntry)i));
	}
	return ret;
}

std::list<ot::ColorStyleValueEntry> ot::getAllColorStyleValueEntries() {
	std::list<ot::ColorStyleValueEntry> ret;
	for (int i = 0; i < (int)ColorStyleValueEntry::ColorStyleValueEntry_End; i++) {
		ret.push_back((ColorStyleValueEntry)i);
	}
	return ret;
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
	case ot::ColorStyleDoubleEntry::BorderWidth: return "Border Width";
	case ot::ColorStyleDoubleEntry::BorderRadiusBig: return "Border Radius Big";
	case ot::ColorStyleDoubleEntry::BorderRadiusSmall: return "Border Radius Small";
	case ot::ColorStyleDoubleEntry::ToolTipOpacity: return "ToolTip Opacity";
	default:
		OT_LOG_EAS("Unknown ColorStyleDoubleEntry (" + std::to_string((int)_colorStyleDoubleEntry) + ")");
		return "Splitter Handle Width";
	}
}

ot::ColorStyleDoubleEntry ot::stringToColorStyleDoubleEntry(const std::string& _colorStyleDoubleEntry) {
	if (_colorStyleDoubleEntry == toString(ColorStyleDoubleEntry::BorderWidth)) return ColorStyleDoubleEntry::BorderWidth;
	else if (_colorStyleDoubleEntry == toString(ColorStyleDoubleEntry::BorderRadiusBig)) return ColorStyleDoubleEntry::BorderRadiusBig;
	else if (_colorStyleDoubleEntry == toString(ColorStyleDoubleEntry::BorderRadiusSmall)) return ColorStyleDoubleEntry::BorderRadiusSmall;
	else if (_colorStyleDoubleEntry == toString(ColorStyleDoubleEntry::ToolTipOpacity)) return ColorStyleDoubleEntry::ToolTipOpacity;
	else {
		OT_LOG_EAS("Unknown ColorStyleDoubleEntry \"" + _colorStyleDoubleEntry + "\"");
		return ColorStyleDoubleEntry::BorderRadiusBig;
	}
}
