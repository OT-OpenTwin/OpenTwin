// @otlicense
// File: Label.h
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
#include <QtWidgets/qlabel.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT Label : public QLabel, public ot::WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(Label)
		OT_DECL_NOMOVE(Label)
		OT_DECL_NODEFAULT(Label)
	public:
		explicit Label(QWidget* _parentWidget);
		explicit Label(const QString& _text, QWidget* _parentWidget);
		virtual ~Label() {};

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

	Q_SIGNALS:
		void mouseClicked();
		void mouseDoubleClicked();

	protected:
		virtual void mousePressEvent(QMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QMouseEvent* _event) override;
		virtual void mouseDoubleClickEvent(QMouseEvent* _event) override;

	private:
		bool m_mouseIsPressed;
	};
}