// @otlicense
// File: Painter2DEditButton.h
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
#include "OTGui/Painter2DDialogFilter.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qframe.h>

namespace ot{

	class Painter2D;
	class PushButton;
	class Painter2DPreview;

	class OT_WIDGETS_API_EXPORT Painter2DEditButton : public QFrame, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(Painter2DEditButton)
		OT_DECL_NOMOVE(Painter2DEditButton)
		OT_DECL_NODEFAULT(Painter2DEditButton)
	public:
		
		//! @brief Creates an instance.
		explicit Painter2DEditButton(QWidget* _parent);

		//! @brief Creates an instance.
		//! Objects creates a copy of the painter.
		explicit Painter2DEditButton(const Painter2D* _painter, QWidget* _parent);

		//! @brief Creates an instance.
		//! Object takes ownership of the painter.
		explicit Painter2DEditButton(Painter2D* _painter, QWidget* _parent);

		virtual ~Painter2DEditButton();

		virtual bool eventFilter(QObject* _obj, QEvent* _event) override;

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		PushButton* getPushButton() const { return m_btn; };

		void setPainter(Painter2D* _painter);
		void setPainter(const Painter2D* _painter);
		const Painter2D* getPainter() { return m_painter; };

		void setFilter(const Painter2DDialogFilter& _filter) { m_filter = _filter; };
		const Painter2DDialogFilter& getFilter() const { return m_filter; };

	Q_SIGNALS:
		void painter2DChanged();

	private Q_SLOTS:
		void slotClicked();

	private:
		Painter2DDialogFilter m_filter;
		Painter2D* m_painter;
		PushButton* m_btn;
		QString m_btnTip;
		QString m_btnText;
		Painter2DPreview* m_preview;

		void updateText(void);
	};
}

