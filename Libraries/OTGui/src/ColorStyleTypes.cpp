//! @file ColorStyleTypes.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/ColorStyleTypes.h"

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
	switch (_colorStyleFileEntry)
	{
	case ot::ColorStyleFileEntry::TransparentIcon: return "Transparent Icon"; 

	case ot::ColorStyleFileEntry::ArrowUpIcon: return "Arrow Up Icon";
	case ot::ColorStyleFileEntry::ArrowUpDisabledIcon: return "Arrow Up Disabled Icon";
	case ot::ColorStyleFileEntry::ArrowUpFocusIcon: return "Arrow Up Focus Icon";
	case ot::ColorStyleFileEntry::ArrowUpPressed: return "Arrow Up Pressed Icon";
	case ot::ColorStyleFileEntry::ArrowDownIcon: return "Arrow Down Icon";
	case ot::ColorStyleFileEntry::ArrowDownDisabledIcon: return "Arrow Down Disabled Icon";
	case ot::ColorStyleFileEntry::ArrowDownFocusIcon: return "Arrow Down Focus Icon";
	case ot::ColorStyleFileEntry::ArrowDownPressed: return "Arrow Down Pressed Icon";
	case ot::ColorStyleFileEntry::ArrowLeftIcon: return "Arrow Left Icon";
	case ot::ColorStyleFileEntry::ArrowLeftDisabledIcon: return "Arrow Left Disabled Icon";
	case ot::ColorStyleFileEntry::ArrowLeftFocusIcon: return "Arrow Left Focus Icon";
	case ot::ColorStyleFileEntry::ArrowLeftPressed: return "Arrow Left Pressed Icon";
	case ot::ColorStyleFileEntry::ArrowRightIcon: return "Arrow Right Icon";
	case ot::ColorStyleFileEntry::ArrowRightDisabledIcon: return "Arrow Right Disabled Icon";
	case ot::ColorStyleFileEntry::ArrowRightFocusIcon: return "Arrow Right Focus Icon";
	case ot::ColorStyleFileEntry::ArrowRightPressed: return "Arrow Right Pressed Icon";

	case ot::ColorStyleFileEntry::TreeBranchClosedIcon: return "Tree Branch Closed Icon";
	case ot::ColorStyleFileEntry::TreeBranchClosedFocusIcon: return "Tree Branch Closed Focus Icon";
	case ot::ColorStyleFileEntry::TreeBranchEndIcon: return "Tree Branch End Icon";
	case ot::ColorStyleFileEntry::TreeBranchLineIcon: return "Tree Branch Line Icon";
	case ot::ColorStyleFileEntry::TreeBranchMoreIcon: return "Tree Branch More Icon";
	case ot::ColorStyleFileEntry::TreeBranchOpenIcon: return "Tree Branch Open Icon";
	case ot::ColorStyleFileEntry::TreeBranchOpenFocusIcon: return "Tree Branch Open Focus Icon";

	case ot::ColorStyleFileEntry::CheckBoxCheckedIcon: return "CheckBox Checked Icon";
	case ot::ColorStyleFileEntry::CheckBoxCheckedDisabledIcon: return "CheckBox Checked Disabled Icon";
	case ot::ColorStyleFileEntry::CheckBoxCheckedFocusIcon: return "CheckBox Checked Focus Icon";
	case ot::ColorStyleFileEntry::CheckBoxCheckedPressedIcon: return "CheckBox Checked Pressed Icon";
	case ot::ColorStyleFileEntry::CheckBoxIndeterminateIcon: return "CheckBox Indeterminate Icon";
	case ot::ColorStyleFileEntry::CheckBoxIndeterminateDisabledIcon: return "CheckBox Indeterminate Disabled Icon";
	case ot::ColorStyleFileEntry::CheckBoxIndeterminateFocusIcon: return "CheckBox Indeterminate Focus Icon";
	case ot::ColorStyleFileEntry::CheckBoxIndeterminatePressedIcon: return "CheckBox Indeterminate Pressed Icon";
	case ot::ColorStyleFileEntry::CheckBoxUncheckedIcon: return "CheckBox Unchecked Icon";
	case ot::ColorStyleFileEntry::CheckBoxUncheckedDisabledIcon: return "CheckBox Unchecked Disabled Icon";
	case ot::ColorStyleFileEntry::CheckBoxUncheckedFocusIcon: return "CheckBox Unchecked Focus Icon";
	case ot::ColorStyleFileEntry::CheckBoxUncheckedPressedIcon: return "CheckBox Unchecked Pressed Icon";

	case ot::ColorStyleFileEntry::LogInCheckBoxCheckedIcon: return "LogIn CheckBox Checked Icon";
	case ot::ColorStyleFileEntry::LogInCheckBoxCheckedDisabledIcon: return "LogIn CheckBox Checked Disabled Icon";
	case ot::ColorStyleFileEntry::LogInCheckBoxCheckedFocusIcon: return "LogIn CheckBox Checked Focus Icon";
	case ot::ColorStyleFileEntry::LogInCheckBoxCheckedPressedIcon: return "LogIn CheckBox Checked Pressed Icon";
	case ot::ColorStyleFileEntry::LogInCheckBoxIndeterminateIcon: return "LogIn CheckBox Indeterminate Icon";
	case ot::ColorStyleFileEntry::LogInCheckBoxIndeterminateDisabledIcon: return "LogIn CheckBox Indeterminate Disabled Icon";
	case ot::ColorStyleFileEntry::LogInCheckBoxIndeterminateFocusIcon: return "LogIn CheckBox Indeterminate Focus Icon";
	case ot::ColorStyleFileEntry::LogInCheckBoxIndeterminatePressedIcon: return "LogIn CheckBox Indeterminate Pressed Icon";
	case ot::ColorStyleFileEntry::LogInCheckBoxUncheckedIcon: return "LogIn CheckBox Unchecked Icon";
	case ot::ColorStyleFileEntry::LogInCheckBoxUncheckedDisabledIcon: return "LogIn CheckBox Unchecked Disabled Icon";
	case ot::ColorStyleFileEntry::LogInCheckBoxUncheckedFocusIcon: return "LogIn CheckBox Unchecked Focus Icon";
	case ot::ColorStyleFileEntry::LogInCheckBoxUncheckedPressedIcon: return "LogIn CheckBox Unchecked Pressed Icon";

	case ot::ColorStyleFileEntry::RadioButtonCheckedIcon: return "RadioButton Checked Icon";
	case ot::ColorStyleFileEntry::RadioButtonCheckedDisabledIcon: return "RadioButton Checked Disabled Icon";
	case ot::ColorStyleFileEntry::RadioButtonCheckedFocusIcon: return "RadioButton Checked Focus Icon";
	case ot::ColorStyleFileEntry::RadioButtonUncheckedIcon: return "RadioButton Unchecked Icon";
	case ot::ColorStyleFileEntry::RadioButtonUncheckedDisabledIcon: return "RadioButton Unchecked Disabled Icon";
	case ot::ColorStyleFileEntry::RadioButtonUncheckedFocusIcon: return "RadioButton Unchecked Focus Icon";

	case ot::ColorStyleFileEntry::ToolBarMoveHorizontalIcon: return "ToolBar Move Horizontal Icon";
	case ot::ColorStyleFileEntry::ToolBarMoveVerticalIcon: return "ToolBar Move Vertical Icon";
	case ot::ColorStyleFileEntry::ToolBarSeparatorHorizontalIcon: return "ToolBar Separator Horizontal Icon";
	case ot::ColorStyleFileEntry::ToolBarSeparatorVerticalIcon: return "ToolBar Separator Vertical Icon";

	case ot::ColorStyleFileEntry::WindowCloseIcon: return "Window Close Icon";
	case ot::ColorStyleFileEntry::WindowCloseDisabledIcon: return "Window Close Disabled Icon";
	case ot::ColorStyleFileEntry::WindowCloseFocusIcon: return "Window Close Focus Icon";
	case ot::ColorStyleFileEntry::WindowClosePressedIcon: return "Window Close Pressed Icon";
	case ot::ColorStyleFileEntry::WindowPinIcon: return "Window Pin Icon";
	case ot::ColorStyleFileEntry::WindowUnpinIcon: return "Window Unpin Icon";
	case ot::ColorStyleFileEntry::WindowPinDisabledIcon: return "Window Pin Disabled Icon";
	case ot::ColorStyleFileEntry::WindowUnpinDisabledIcon: return "Window Unpin Disabled Icon";
	case ot::ColorStyleFileEntry::WindowPinFocusIcon: return "Window Pin Focus Icon";
	case ot::ColorStyleFileEntry::WindowUnpinFocusIcon: return "Window Unpin Focus Icon";
	case ot::ColorStyleFileEntry::WindowPinPressedIcon: return "Window Pin Pressed Icon";
	case ot::ColorStyleFileEntry::WindowUnpinPressedIcon: return "Window Unpin Pressed Icon";
	case ot::ColorStyleFileEntry::WindowLockIcon: return "Window Lock Icon";
	case ot::ColorStyleFileEntry::WindowUnlockIcon: return "Window Unlock Icon";
	case ot::ColorStyleFileEntry::WindowLockDisabledIcon: return "Window Lock Disabled Icon";
	case ot::ColorStyleFileEntry::WindowUnlockDisabledIcon: return "Window Unlock Disabled Icon";
	case ot::ColorStyleFileEntry::WindowLockFocusIcon: return "Window Lock Focus Icon";
	case ot::ColorStyleFileEntry::WindowUnlockFocusIcon: return "Window Unlock Focus Icon";
	case ot::ColorStyleFileEntry::WindowLockPressedIcon: return "Window Lock Pressed Icon";
	case ot::ColorStyleFileEntry::WindowUnlockPressedIcon: return "Window Unlock Pressed Icon";
	case ot::ColorStyleFileEntry::WindowGripIcon: return "Window Grip Icon";
	case ot::ColorStyleFileEntry::WindowUndockIcon: return "Window Undock Icon";
	case ot::ColorStyleFileEntry::WindowUndockDisabledIcon: return "Window Undock Disabled Icon";
	case ot::ColorStyleFileEntry::WindowUndockFocusIcon: return "Window Undock Focus Icon";
	case ot::ColorStyleFileEntry::WindowUndockPressedIcon: return "Window Undock Pressed Icon";

	case ot::ColorStyleFileEntry::PropertyItemDeleteIcon: return "Property Item Delete Icon";
	case ot::ColorStyleFileEntry::PropertyGroupExpandedIcon: return "Property Group Expanded Icon";
	case ot::ColorStyleFileEntry::PropertyGroupCollapsedIcon: return "Property Group Collapsed Icon";
	case ot::ColorStyleFileEntry::LogInBackgroundImage: return "Log In Background Image";
	default:
		OT_LOG_EAS("Unknown ColorStyleFileEntry (" + std::to_string((int)_colorStyleFileEntry) + ")");
		return "Transparent Icon";
	}

}

ot::ColorStyleFileEntry ot::stringToColorStyleFileEntry(const std::string& _colorStyleFileEntry) {
	if (_colorStyleFileEntry == toString(ColorStyleFileEntry::TransparentIcon)) return ColorStyleFileEntry::TransparentIcon;
	
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowUpIcon)) return ColorStyleFileEntry::ArrowUpIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowUpDisabledIcon)) return ColorStyleFileEntry::ArrowUpDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowUpFocusIcon)) return ColorStyleFileEntry::ArrowUpFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowUpPressed)) return ColorStyleFileEntry::ArrowUpPressed;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowDownIcon)) return ColorStyleFileEntry::ArrowDownIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowDownDisabledIcon)) return ColorStyleFileEntry::ArrowDownDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowDownFocusIcon)) return ColorStyleFileEntry::ArrowDownFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowDownPressed)) return ColorStyleFileEntry::ArrowDownPressed;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowLeftIcon)) return ColorStyleFileEntry::ArrowLeftIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowLeftDisabledIcon)) return ColorStyleFileEntry::ArrowLeftDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowLeftFocusIcon)) return ColorStyleFileEntry::ArrowLeftFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowLeftPressed)) return ColorStyleFileEntry::ArrowLeftPressed;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowRightIcon)) return ColorStyleFileEntry::ArrowRightIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowRightDisabledIcon)) return ColorStyleFileEntry::ArrowRightDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowRightFocusIcon)) return ColorStyleFileEntry::ArrowRightFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ArrowRightPressed)) return ColorStyleFileEntry::ArrowRightPressed;

	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::TreeBranchClosedIcon)) return ColorStyleFileEntry::TreeBranchClosedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::TreeBranchClosedFocusIcon)) return ColorStyleFileEntry::TreeBranchClosedFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::TreeBranchEndIcon)) return ColorStyleFileEntry::TreeBranchEndIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::TreeBranchLineIcon)) return ColorStyleFileEntry::TreeBranchLineIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::TreeBranchMoreIcon)) return ColorStyleFileEntry::TreeBranchMoreIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::TreeBranchOpenIcon)) return ColorStyleFileEntry::TreeBranchOpenIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::TreeBranchOpenFocusIcon)) return ColorStyleFileEntry::TreeBranchOpenFocusIcon;

	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::CheckBoxCheckedIcon)) return ColorStyleFileEntry::CheckBoxCheckedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::CheckBoxCheckedDisabledIcon)) return ColorStyleFileEntry::CheckBoxCheckedDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::CheckBoxCheckedFocusIcon)) return ColorStyleFileEntry::CheckBoxCheckedFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::CheckBoxCheckedPressedIcon)) return ColorStyleFileEntry::CheckBoxCheckedPressedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::CheckBoxIndeterminateIcon)) return ColorStyleFileEntry::CheckBoxIndeterminateIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::CheckBoxIndeterminateDisabledIcon)) return ColorStyleFileEntry::CheckBoxIndeterminateDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::CheckBoxIndeterminateFocusIcon)) return ColorStyleFileEntry::CheckBoxIndeterminateFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::CheckBoxIndeterminatePressedIcon)) return ColorStyleFileEntry::CheckBoxIndeterminatePressedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::CheckBoxUncheckedIcon)) return ColorStyleFileEntry::CheckBoxUncheckedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::CheckBoxUncheckedDisabledIcon)) return ColorStyleFileEntry::CheckBoxUncheckedDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::CheckBoxUncheckedFocusIcon)) return ColorStyleFileEntry::CheckBoxUncheckedFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::CheckBoxUncheckedPressedIcon)) return ColorStyleFileEntry::CheckBoxUncheckedPressedIcon;

	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::LogInCheckBoxCheckedIcon)) return ColorStyleFileEntry::LogInCheckBoxCheckedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::LogInCheckBoxCheckedDisabledIcon)) return ColorStyleFileEntry::LogInCheckBoxCheckedDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::LogInCheckBoxCheckedFocusIcon)) return ColorStyleFileEntry::LogInCheckBoxCheckedFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::LogInCheckBoxCheckedPressedIcon)) return ColorStyleFileEntry::LogInCheckBoxCheckedPressedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::LogInCheckBoxIndeterminateIcon)) return ColorStyleFileEntry::LogInCheckBoxIndeterminateIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::LogInCheckBoxIndeterminateDisabledIcon)) return ColorStyleFileEntry::LogInCheckBoxIndeterminateDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::LogInCheckBoxIndeterminateFocusIcon)) return ColorStyleFileEntry::LogInCheckBoxIndeterminateFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::LogInCheckBoxIndeterminatePressedIcon)) return ColorStyleFileEntry::LogInCheckBoxIndeterminatePressedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::LogInCheckBoxUncheckedIcon)) return ColorStyleFileEntry::LogInCheckBoxUncheckedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::LogInCheckBoxUncheckedDisabledIcon)) return ColorStyleFileEntry::LogInCheckBoxUncheckedDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::LogInCheckBoxUncheckedFocusIcon)) return ColorStyleFileEntry::LogInCheckBoxUncheckedFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::LogInCheckBoxUncheckedPressedIcon)) return ColorStyleFileEntry::LogInCheckBoxUncheckedPressedIcon;

	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::RadioButtonCheckedIcon)) return ColorStyleFileEntry::RadioButtonCheckedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::RadioButtonCheckedDisabledIcon)) return ColorStyleFileEntry::RadioButtonCheckedDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::RadioButtonCheckedFocusIcon)) return ColorStyleFileEntry::RadioButtonCheckedFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::RadioButtonUncheckedIcon)) return ColorStyleFileEntry::RadioButtonUncheckedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::RadioButtonUncheckedDisabledIcon)) return ColorStyleFileEntry::RadioButtonUncheckedDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::RadioButtonUncheckedFocusIcon)) return ColorStyleFileEntry::RadioButtonUncheckedFocusIcon;

	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ToolBarMoveHorizontalIcon)) return ColorStyleFileEntry::ToolBarMoveHorizontalIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ToolBarMoveVerticalIcon)) return ColorStyleFileEntry::ToolBarMoveVerticalIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ToolBarSeparatorHorizontalIcon)) return ColorStyleFileEntry::ToolBarSeparatorHorizontalIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::ToolBarSeparatorVerticalIcon)) return ColorStyleFileEntry::ToolBarSeparatorVerticalIcon;

	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowCloseIcon)) return ColorStyleFileEntry::WindowCloseIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowCloseDisabledIcon)) return ColorStyleFileEntry::WindowCloseDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowCloseFocusIcon)) return ColorStyleFileEntry::WindowCloseFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowClosePressedIcon)) return ColorStyleFileEntry::WindowClosePressedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowPinIcon)) return ColorStyleFileEntry::WindowPinIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowUnpinIcon)) return ColorStyleFileEntry::WindowUnpinIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowPinDisabledIcon)) return ColorStyleFileEntry::WindowPinDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowUnpinDisabledIcon)) return ColorStyleFileEntry::WindowUnpinDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowPinFocusIcon)) return ColorStyleFileEntry::WindowPinFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowUnpinFocusIcon)) return ColorStyleFileEntry::WindowUnpinFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowPinPressedIcon)) return ColorStyleFileEntry::WindowPinPressedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowUnpinPressedIcon)) return ColorStyleFileEntry::WindowUnpinPressedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowLockIcon)) return ColorStyleFileEntry::WindowLockIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowUnlockIcon)) return ColorStyleFileEntry::WindowUnlockIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowLockDisabledIcon)) return ColorStyleFileEntry::WindowLockDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowUnlockDisabledIcon)) return ColorStyleFileEntry::WindowUnlockDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowLockFocusIcon)) return ColorStyleFileEntry::WindowLockFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowUnlockFocusIcon)) return ColorStyleFileEntry::WindowUnlockFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowLockPressedIcon)) return ColorStyleFileEntry::WindowLockPressedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowUnlockPressedIcon)) return ColorStyleFileEntry::WindowUnlockPressedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowGripIcon)) return ColorStyleFileEntry::WindowGripIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowUndockIcon)) return ColorStyleFileEntry::WindowUndockIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowUndockDisabledIcon)) return ColorStyleFileEntry::WindowUndockDisabledIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowUndockFocusIcon)) return ColorStyleFileEntry::WindowUndockFocusIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::WindowUndockPressedIcon)) return ColorStyleFileEntry::WindowUndockPressedIcon;

	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::PropertyGroupCollapsedIcon)) return ColorStyleFileEntry::PropertyGroupCollapsedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::PropertyGroupExpandedIcon)) return ColorStyleFileEntry::PropertyGroupExpandedIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::PropertyItemDeleteIcon)) return ColorStyleFileEntry::PropertyItemDeleteIcon;
	else if (_colorStyleFileEntry == toString(ColorStyleFileEntry::LogInBackgroundImage)) return ColorStyleFileEntry::LogInBackgroundImage;
	else {
		OT_LOG_EAS("Unknown ColorStyleFileEntry \"" + _colorStyleFileEntry + "\"");
		return ColorStyleFileEntry::LogInBackgroundImage;
	}
}

std::string ot::toString(ColorStyleValueEntry _colorStyleValueEntry) {
	switch (_colorStyleValueEntry)
	{
	case ot::ColorStyleValueEntry::Transparent: return "Transparent";

	case ot::ColorStyleValueEntry::WidgetBackground: return "Widget Background";
	case ot::ColorStyleValueEntry::WidgetAlternateBackground: return "Widget Alternate Background";
	case ot::ColorStyleValueEntry::WidgetForeground: return "Widget Foreground";
	case ot::ColorStyleValueEntry::WidgetDisabledBackground: return "Widget Disabled Background";
	case ot::ColorStyleValueEntry::WidgetDisabledForeground: return "Widget Disabled Foreground";

	case ot::ColorStyleValueEntry::WidgetHoverBackground: return "Widget Hover Background";
	case ot::ColorStyleValueEntry::WidgetHoverForeground: return "Widget Hover Foreground";
	case ot::ColorStyleValueEntry::WidgetSelectionBackground: return "Widget Selection Background";
	case ot::ColorStyleValueEntry::WidgetSelectionForeground: return "Widget Selection Foreground";

	case ot::ColorStyleValueEntry::TitleBackground: return "Title Background";
	case ot::ColorStyleValueEntry::TitleForeground: return "Title Foreground";
	case ot::ColorStyleValueEntry::TitleBorder: return "Title Border";

	case ot::ColorStyleValueEntry::HeaderBackground: return "Header Background";
	case ot::ColorStyleValueEntry::HeaderForeground: return "Header Foregorund";
	case ot::ColorStyleValueEntry::HeaderHoverBackground: return "Header Hover Background";
	case ot::ColorStyleValueEntry::HeaderHoverForeground: return "Header Hover Foreground";
	case ot::ColorStyleValueEntry::HeaderSelectionBackground: return "Header Selection Background";
	case ot::ColorStyleValueEntry::HeaderSelectionForeground: return "Header Selection Foreground";

	case ot::ColorStyleValueEntry::InputBackground: return "Input Background";
	case ot::ColorStyleValueEntry::InputForeground: return "Input Foreground";

	case ot::ColorStyleValueEntry::TextEditorLineBorder: return "TextEditor Line Border";
	case ot::ColorStyleValueEntry::TextEditorHighlightBackground: return "TextEditor Highlight Background";

	case ot::ColorStyleValueEntry::DialogBackground: return "Dialog Background";
	case ot::ColorStyleValueEntry::DialogForeground: return "Dialog Foreground";

	case ot::ColorStyleValueEntry::WindowBackground: return "Window Background";
	case ot::ColorStyleValueEntry::WindowForeground: return "Window Foreground";

	case ot::ColorStyleValueEntry::Border: return "Border Color";
	case ot::ColorStyleValueEntry::BorderLight: return "Border Light Color";
	case ot::ColorStyleValueEntry::BorderDisabled: return "Border Disabled Color";
	case ot::ColorStyleValueEntry::BorderHover: return "Border Hover Color";
	case ot::ColorStyleValueEntry::BorderSelection: return "Border Selection Color";

	case ot::ColorStyleValueEntry::Rainbow0: return "Rainbow 0";
	case ot::ColorStyleValueEntry::Rainbow1: return "Rainbow 1";
	case ot::ColorStyleValueEntry::Rainbow2: return "Rainbow 2";
	case ot::ColorStyleValueEntry::Rainbow3: return "Rainbow 3";
	case ot::ColorStyleValueEntry::Rainbow4: return "Rainbow 4";
	case ot::ColorStyleValueEntry::Rainbow5: return "Rainbow 5";
	case ot::ColorStyleValueEntry::Rainbow6: return "Rainbow 6";
	case ot::ColorStyleValueEntry::Rainbow7: return "Rainbow 7";
	case ot::ColorStyleValueEntry::Rainbow8: return "Rainbow 8";
	case ot::ColorStyleValueEntry::Rainbow9: return "Rainbow 9";

	case ot::ColorStyleValueEntry::ToolBarFirstTabBackground: return "TabToolBar First Tab Background";
	case ot::ColorStyleValueEntry::ToolBarFirstTabForeground: return "TabToolBar First Tab Foreground";

	case ot::ColorStyleValueEntry::GraphicsItemBorder: return "GraphicsItem Border Color";
	case ot::ColorStyleValueEntry::GraphicsItemConnection: return "GraphicsItem Connection Color";
	case ot::ColorStyleValueEntry::GraphicsItemBackground: return "GraphicsItem Background";
	case ot::ColorStyleValueEntry::GraphicsItemForeground: return "GraphicsItem Foreground";
	case ot::ColorStyleValueEntry::GraphicsItemSelectionBorder: return "GraphicsItem Selection Border";
	case ot::ColorStyleValueEntry::GraphicsItemHoverBorder: return "GraphicsItem Hover Border";
	case ot::ColorStyleValueEntry::GraphicsItemConnectableBackground: return "GraphicsItem Connectable Background";
	case ot::ColorStyleValueEntry::GraphicsItemLineColor: return "GraphicsItem Line Color";

	case ot::ColorStyleValueEntry::StyledTextHighlight: return "Styled Text Highlight";
	case ot::ColorStyleValueEntry::StyledTextLightHighlight: return "Styled Light Highlight";
	case ot::ColorStyleValueEntry::StyledTextComment: return "Styled Text Comment";
	case ot::ColorStyleValueEntry::StyledTextWarning: return "Styled Text Warning";
	case ot::ColorStyleValueEntry::StyledTextError: return "Styled Text Error";

	case ot::ColorStyleValueEntry::PythonKeyword: return "Python Keyword";
	case ot::ColorStyleValueEntry::PythonClass: return "Python Class";
	case ot::ColorStyleValueEntry::PythonString: return "Python String";
	case ot::ColorStyleValueEntry::PythonFunction: return "Python Function";
	case ot::ColorStyleValueEntry::PythonComment: return "Python Comment";

	case ot::ColorStyleValueEntry::PlotCurve: return "Plot Curve";
	case ot::ColorStyleValueEntry::PlotCurveDimmed: return "Plot Curve Dimmed";
	case ot::ColorStyleValueEntry::PlotCurveHighlight: return "Plot Curve Highlight";
	case ot::ColorStyleValueEntry::PlotCurveSymbol: return "Plot Curve Symbol";

	case ot::ColorStyleValueEntry::ErrorForeground: return "Error Foreground";

	default:
		OT_LOG_EAS("Unknown ColorStyleValueEntry (" + std::to_string((int)_colorStyleValueEntry) + ")");
		return "Property Item Delete Icon";
	}
}

ot::ColorStyleValueEntry ot::stringToColorStyleValueEntry(const std::string& _colorStyleValueEntry) {
	if (_colorStyleValueEntry == toString(ColorStyleValueEntry::Transparent)) return ColorStyleValueEntry::Transparent;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::WidgetBackground)) return ColorStyleValueEntry::WidgetBackground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::WidgetAlternateBackground)) return ColorStyleValueEntry::WidgetAlternateBackground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::WidgetForeground)) return ColorStyleValueEntry::WidgetForeground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::WidgetDisabledBackground)) return ColorStyleValueEntry::WidgetDisabledBackground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::WidgetDisabledForeground)) return ColorStyleValueEntry::WidgetDisabledForeground;

	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::WidgetHoverBackground)) return ColorStyleValueEntry::WidgetHoverBackground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::WidgetHoverForeground)) return ColorStyleValueEntry::WidgetHoverForeground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::WidgetSelectionBackground)) return ColorStyleValueEntry::WidgetSelectionBackground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::WidgetSelectionForeground)) return ColorStyleValueEntry::WidgetSelectionForeground;

	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::TitleBackground)) return ColorStyleValueEntry::TitleBackground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::TitleForeground)) return ColorStyleValueEntry::TitleForeground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::TitleBorder)) return ColorStyleValueEntry::TitleBorder;

	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::HeaderBackground)) return ColorStyleValueEntry::HeaderBackground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::HeaderForeground)) return ColorStyleValueEntry::HeaderForeground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::HeaderHoverBackground)) return ColorStyleValueEntry::HeaderHoverBackground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::HeaderHoverForeground)) return ColorStyleValueEntry::HeaderHoverForeground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::HeaderSelectionBackground)) return ColorStyleValueEntry::HeaderSelectionBackground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::HeaderSelectionForeground)) return ColorStyleValueEntry::HeaderSelectionForeground;

	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::InputBackground)) return ColorStyleValueEntry::InputBackground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::InputForeground)) return ColorStyleValueEntry::InputForeground;

	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::TextEditorLineBorder)) return ColorStyleValueEntry::TextEditorLineBorder;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::TextEditorHighlightBackground)) return ColorStyleValueEntry::TextEditorHighlightBackground;

	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::DialogBackground)) return ColorStyleValueEntry::DialogBackground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::DialogForeground)) return ColorStyleValueEntry::DialogForeground;

	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::WindowBackground)) return ColorStyleValueEntry::WindowBackground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::WindowForeground)) return ColorStyleValueEntry::WindowForeground;

	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::Border)) return ColorStyleValueEntry::Border;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::BorderLight)) return ColorStyleValueEntry::BorderLight;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::BorderDisabled)) return ColorStyleValueEntry::BorderDisabled;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::BorderHover)) return ColorStyleValueEntry::BorderHover;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::BorderSelection)) return ColorStyleValueEntry::BorderSelection;

	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::Rainbow0)) return ColorStyleValueEntry::Rainbow0;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::Rainbow1)) return ColorStyleValueEntry::Rainbow1;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::Rainbow2)) return ColorStyleValueEntry::Rainbow2;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::Rainbow3)) return ColorStyleValueEntry::Rainbow3;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::Rainbow4)) return ColorStyleValueEntry::Rainbow4;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::Rainbow5)) return ColorStyleValueEntry::Rainbow5;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::Rainbow6)) return ColorStyleValueEntry::Rainbow6;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::Rainbow7)) return ColorStyleValueEntry::Rainbow7;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::Rainbow8)) return ColorStyleValueEntry::Rainbow8;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::Rainbow9)) return ColorStyleValueEntry::Rainbow9;

	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::ToolBarFirstTabBackground)) return ColorStyleValueEntry::ToolBarFirstTabBackground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::ToolBarFirstTabForeground)) return ColorStyleValueEntry::ToolBarFirstTabForeground;

	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::GraphicsItemBorder)) return ColorStyleValueEntry::GraphicsItemBorder;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::GraphicsItemConnection)) return ColorStyleValueEntry::GraphicsItemConnection;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::GraphicsItemBackground)) return ColorStyleValueEntry::GraphicsItemBackground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::GraphicsItemForeground)) return ColorStyleValueEntry::GraphicsItemForeground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::GraphicsItemSelectionBorder)) return ColorStyleValueEntry::GraphicsItemSelectionBorder;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::GraphicsItemHoverBorder)) return ColorStyleValueEntry::GraphicsItemHoverBorder;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::GraphicsItemConnectableBackground)) return ColorStyleValueEntry::GraphicsItemConnectableBackground;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::GraphicsItemLineColor)) return ColorStyleValueEntry::GraphicsItemLineColor;

	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::StyledTextHighlight)) return ColorStyleValueEntry::StyledTextHighlight;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::StyledTextLightHighlight)) return ColorStyleValueEntry::StyledTextLightHighlight;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::StyledTextComment)) return ColorStyleValueEntry::StyledTextComment;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::StyledTextWarning)) return ColorStyleValueEntry::StyledTextWarning;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::StyledTextError)) return ColorStyleValueEntry::StyledTextError;
	
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::PythonKeyword)) return ColorStyleValueEntry::PythonKeyword;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::PythonClass)) return ColorStyleValueEntry::PythonClass;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::PythonString)) return ColorStyleValueEntry::PythonString;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::PythonFunction)) return ColorStyleValueEntry::PythonFunction;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::PythonComment)) return ColorStyleValueEntry::PythonComment;

	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::PlotCurve)) return ColorStyleValueEntry::PlotCurve;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::PlotCurveDimmed)) return ColorStyleValueEntry::PlotCurveDimmed;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::PlotCurveHighlight)) return ColorStyleValueEntry::PlotCurveHighlight;
	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::PlotCurveSymbol)) return ColorStyleValueEntry::PlotCurveSymbol;

	else if (_colorStyleValueEntry == toString(ColorStyleValueEntry::ErrorForeground)) return ColorStyleValueEntry::ErrorForeground;
	else {
		OT_LOG_EAS("Unknown ColorStyleValueEntry \"" + _colorStyleValueEntry + "\"");
		return ColorStyleValueEntry::WidgetBackground;
	}
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
