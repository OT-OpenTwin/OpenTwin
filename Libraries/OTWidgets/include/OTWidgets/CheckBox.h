// @otlicense
// File: CheckBox.h
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

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qcheckbox.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT CheckBox : public QCheckBox, public WidgetBase {
		Q_OBJECT
	public:
		CheckBox(QWidget* _parent = (QWidget*)nullptr);
		CheckBox(const QString& _text, QWidget* _parent = (QWidget*)nullptr);
		CheckBox(Qt::CheckState _state, const QString& _text, QWidget* _parent = (QWidget*)nullptr);
		virtual ~CheckBox() {};

		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };
	};

}
