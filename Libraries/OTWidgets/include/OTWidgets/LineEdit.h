// @otlicense
// File: LineEdit.h
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
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qlineedit.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT LineEdit : public QLineEdit, public ot::WidgetBase {
		Q_OBJECT
	public:
		LineEdit(QWidget* _parentWidget = (QWidget*)nullptr);
		LineEdit(const QString& _initialText, QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~LineEdit() {};

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

	public Q_SLOTS:
		void setInputErrorStateProperty(void);
		void unsetInputErrorStateProperty(void);
		void setInputFocus(void);

	Q_SIGNALS:
		void leftMouseButtonPressed(void);

	protected:
		virtual void mousePressEvent(QMouseEvent* _event) override;
	};
}