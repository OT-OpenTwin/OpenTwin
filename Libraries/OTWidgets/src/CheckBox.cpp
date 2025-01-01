//! @file CheckBox.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/CheckBox.h"

ot::CheckBox::CheckBox(QWidget* _parent) 
	: QCheckBox(_parent) 
{}

ot::CheckBox::CheckBox(const QString& _text, QWidget* _parent)
	: QCheckBox(_text, _parent) 
{}
