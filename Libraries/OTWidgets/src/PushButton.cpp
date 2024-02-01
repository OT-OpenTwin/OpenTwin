//! @file PushButton.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PushButton.h"

ot::PushButton::PushButton(QWidget* _parent)
	: QPushButton(_parent)
{}

ot::PushButton::PushButton(const QString& _text, QWidget* _parent)
	: QPushButton(_text, _parent)
{}

ot::PushButton::PushButton(const QIcon& _icon, const QString& _text, QWidget* _parent)
	: QPushButton(_icon, _text, _parent)
{}