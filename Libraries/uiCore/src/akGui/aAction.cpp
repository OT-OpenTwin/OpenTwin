// @otlicense
// File: aAction.cpp
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

// AK header
#include <akGui/aAction.h>

ak::aAction::aAction(QToolButton::ToolButtonPopupMode _popupMode, QObject * _parent)
	: QAction(_parent), m_popupMode(_popupMode), aObject(otAction) {}

ak::aAction::aAction(const QString & _text, QToolButton::ToolButtonPopupMode _popupMode, QObject * _parent)
	: QAction(_text, _parent), m_popupMode(_popupMode), aObject(otAction) {}

ak::aAction::aAction(const QIcon & _icon, const QString & _text, QToolButton::ToolButtonPopupMode _popupMode, QObject * _parent)
	: QAction(_icon, _text, _parent), m_popupMode(_popupMode), aObject(otAction) {}

ak::aAction::~aAction() { A_OBJECT_DESTROYING }
