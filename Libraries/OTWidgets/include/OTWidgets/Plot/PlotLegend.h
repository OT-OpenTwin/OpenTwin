// @otlicense
// File: PlotLegend.h
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
class QVBoxLayout;

namespace ot {

	class PlotBase;
	class PlotLegendItem;

	class OT_WIDGETS_API_EXPORT PlotLegend : public QObject, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(PlotLegend)
		OT_DECL_NOMOVE(PlotLegend)
		OT_DECL_NODEFAULT(PlotLegend)
	public:
		explicit PlotLegend(PlotBase* _plot, QWidget* _parentWidget);
		virtual ~PlotLegend();

		virtual QWidget* getQWidget() override { return m_rootWidget; };
		virtual const QWidget* getQWidget() const override { return m_rootWidget; };

		void addItem(PlotLegendItem* _item);
		void removeItem(PlotLegendItem* _item);

		PlotBase* getPlot() const { return m_plot; };

	private:
		friend class PlotLegendItem;

		PlotBase* m_plot;
		QWidget* m_rootWidget;
		QVBoxLayout* m_selectedLayout;
		QVBoxLayout* m_dimmedLayout;

		std::list<PlotLegendItem*> m_items;
	};

}