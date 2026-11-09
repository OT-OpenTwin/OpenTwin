// @otlicense
// File: PlotLegendItem.h
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
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;

namespace ot {

	class Painter2D;
	class PlotLegend;
	class PlotDataset;
	class PlotLegendItemWidget;

	class OT_WIDGETS_API_EXPORT PlotLegendItem : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(PlotLegendItem)
		OT_DECL_NOMOVE(PlotLegendItem)
		OT_DECL_NODEFAULT(PlotLegendItem)
	public:
		explicit PlotLegendItem(PlotDataset* _dataset);
		virtual ~PlotLegendItem();

		QWidget* getSelectedWidget() const;
		QWidget* getDimmedWidget() const;

		void updateVisibility();

		void setLegend(PlotLegend* _legend) { m_legend = _legend; };
		PlotLegend* getLegend() const { return m_legend; };
		void forgetLegend() { m_legend = nullptr; };

		void setLabel(const QString& _text);
		void setSelectedPainter(const ot::Painter2D* _painter);
		void setDimmedPainter(const ot::Painter2D* _painter);

		void setToolTip(const QString& _toolTip);

		void attach();
		void detach();
		
		PlotDataset* getDataset() const { return m_dataset; };

	private Q_SLOTS:
		void slotHovered();
		void slotUnhovered();
		void slotClicked(Qt::KeyboardModifiers _modifiers);

	private:
		PlotLegend* m_legend;
		PlotDataset* m_dataset;

		PlotLegendItemWidget* m_selectedWidget;
		PlotLegendItemWidget* m_dimmedWidget;

	};

}
