// @otlicense
// File: Action.cpp
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
#include "OTWidgets/Action.h"

ot::Action::Action(QObject* _parent)
	: QAction(_parent)
{

}

ot::Action::Action(const QString& _text, QObject* _parent) 
	: QAction(_text, _parent)
{

}

ot::Action::Action(const QIcon& _icon, const QString& _text, QObject* _parent) 
	: QAction(_text, _parent)
{
	this->setIcon(_icon);
}

ot::Action::~Action() {}