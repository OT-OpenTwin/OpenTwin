//! @file PushButton.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/PushButton.h"

// Qt header
#include <QtWidgets/qstyle.h>

ot::PushButton::PushButton(QWidget* _parent)
	: QPushButton(_parent)
{
	this->setObjectName("OT_PushButton");
	this->setProperty("IsSelected", false);
}

ot::PushButton::PushButton(const QString& _text, QWidget* _parent)
	: QPushButton(_text, _parent)
{
	this->setObjectName("OT_PushButton");
	this->setProperty("IsSelected", false);
}

ot::PushButton::PushButton(const QIcon& _icon, const QString& _text, QWidget* _parent)
	: QPushButton(_icon, _text, _parent)
{
	this->setObjectName("OT_PushButton");
	this->setProperty("IsSelected", false);
}

void ot::PushButton::setSelectedProperty(void) {
	this->setProperty("IsSelected", true);
	this->style()->unpolish(this);
	this->style()->polish(this);
}

void ot::PushButton::unsetSelectedProperty(void) {
	this->setProperty("IsSelected", false);
	this->style()->unpolish(this);
	this->style()->polish(this);
}