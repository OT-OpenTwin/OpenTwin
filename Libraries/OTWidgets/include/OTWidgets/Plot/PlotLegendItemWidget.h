// @otlicense
// File: PlotLegendItemWidget.h
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
#include "OTWidgets/Widgets/WidgetBase.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;

namespace ot {
	
	class Painter2D;
	class Painter2DPreview;
	class InteractiveLabel;

	class OT_WIDGETS_API_EXPORT PlotLegendItemWidget : public QObject, public WidgetBase
	{
		Q_OBJECT
		OT_DECL_NOCOPY(PlotLegendItemWidget)
		OT_DECL_NOMOVE(PlotLegendItemWidget)
	public:
		explicit PlotLegendItemWidget();
		virtual ~PlotLegendItemWidget();

		virtual QWidget* getQWidget() override { return m_rootWidget; };
		virtual const QWidget* getQWidget() const override { return m_rootWidget; };

		void setLabel(const QString& _text);
		void setToolTip(const QString& _toolTip);

		void setPainter(const ot::Painter2D* _painter);

	Q_SIGNALS:
		void hovered();
		void unhovered();
		void clicked(Qt::KeyboardModifiers _modifiers);

	private Q_SLOTS:
		void slotMouseEntered();
		void slotMouseLeft();
		void slotMouseClicked(Qt::KeyboardModifiers _modifiers);

	private:
		QWidget* m_rootWidget;

		Painter2DPreview* m_colorPreview;
		InteractiveLabel* m_label;

	};
}