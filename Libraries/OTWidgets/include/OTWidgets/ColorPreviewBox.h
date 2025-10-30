// @otlicense
// File: ColorPreviewBox.h
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
#include "OTCore/Color.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qframe.h>

namespace ot {

	class PushButton;

	class OT_WIDGETS_API_EXPORT ColorPreviewBox : public QFrame, public WidgetBase {
		OT_DECL_NOCOPY(ColorPreviewBox)
	public:
		ColorPreviewBox(const QColor& _color = QColor(), QWidget* _parent = (QWidget*)nullptr);
		ColorPreviewBox(const ot::Color& _color, QWidget* _parent = (QWidget*)nullptr);
		virtual ~ColorPreviewBox();

		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		virtual QSize sizeHint(void) const override { return QSize(16, 16); };

		void setColor(const ot::Color& _color);
		void setColor(const QColor& _color);
		const QColor& color(void) const { return m_color; };

	protected:
		virtual void paintEvent(QPaintEvent* _event) override;

	private:
		QColor m_color;
	};

}