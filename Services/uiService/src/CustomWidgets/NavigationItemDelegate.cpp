// @otlicense
// File: NavigationItemDelegate.cpp
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
#include "CustomWidgets/NavigationItemDelegate.h"
#include "OTWidgets/Syntax/EntityNameValidator.h"
#include "OTWidgets/Widgets/LineEdit.h"

QWidget* NavigationItemDelegate::createEditor(QWidget* _parent, const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	ot::LineEdit* edit = new ot::LineEdit(_parent);
	edit->setValidator(new ot::EntityNameValidator);
	return edit;
}
