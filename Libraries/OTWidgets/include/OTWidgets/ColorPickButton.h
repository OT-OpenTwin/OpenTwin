// @otlicense
// File: ColorPickButton.h
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
	class ColorPreviewBox;

	class OT_WIDGETS_API_EXPORT ColorPickButton : public QFrame, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(ColorPickButton)
	public:
		ColorPickButton(const QColor& _color = QColor(), QWidget* _parent = (QWidget*)nullptr);
		ColorPickButton(const ot::Color& _color, QWidget* _parent = (QWidget*)nullptr);
		virtual ~ColorPickButton();

		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		void setColor(const ot::Color& _color);
		void setColor(const QColor& _color);
		const QColor& color(void) const;
		ot::Color otColor(void) const;

		void useCustomToolTip(bool _use = true);
		bool isUseCustomToolTip(void) const { return m_useCustomToolTip; };

		void setEditAlpha(bool _use = true);
		bool getEditAlpha(void) const { return m_editAlpha; };

		void replaceButtonText(const QString& _text);

		ColorPreviewBox* getPreviewBox(void) const { return m_view; };
		PushButton* getPushButton(void) const { return m_btn; };

	Q_SIGNALS:
		void colorChanged(void);

	public Q_SLOTS:
		void slotBrowse(void);

	protected:
		void updateButtonText(void);

	private:
		void ini(const QColor& _color);

		bool m_useCustomToolTip;
		bool m_editAlpha;
		ColorPreviewBox* m_view;
		PushButton* m_btn;
	};

}