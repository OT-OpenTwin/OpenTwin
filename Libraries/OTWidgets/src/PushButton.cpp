// @otlicense
// File: PushButton.cpp
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