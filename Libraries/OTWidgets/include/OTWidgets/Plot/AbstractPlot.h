// @otlicense
// File: AbstractPlot.h
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
#include "OTGui/Plot/Plot1DCfg.h"
#include "OTWidgets/Widgets/WidgetBase.h"
#include "OTWidgets/Plot/AbstractPlotAxis.h"

// Qt header
#include <QtGui/qcolor.h>

namespace ot {

	class PlotBase;

	class OT_WIDGETS_API_EXPORT AbstractPlot : public WidgetBase {
		OT_DECL_NOCOPY(AbstractPlot)
		OT_DECL_NODEFAULT(AbstractPlot)
	public:
		AbstractPlot(PlotBase* _ownerPlot);

		virtual ~AbstractPlot();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Plot

		virtual void updateGrid() = 0;

		virtual void updateWholePlot() = 0;

		virtual void clearPlot() = 0;

		virtual void resetPlotView() = 0;

		virtual Plot1DCfg::PlotType getPlotType() const = 0;

		virtual void updateAllAxes() = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Axis

		AbstractPlotAxis* getPlotAxis(AbstractPlotAxis::AxisID _id);

		void setPlotAxisTitle(AbstractPlotAxis::AxisID _axis, const QString& _title);

		void setPlotAxisAutoScale(AbstractPlotAxis::AxisID _axis, bool _isAutoScale);

		void setPlotAxisLogScale(AbstractPlotAxis::AxisID _axis, bool _isLogScale);

		void setPlotAxisMin(AbstractPlotAxis::AxisID _axis, double _minValue);

		void setPlotAxisMax(AbstractPlotAxis::AxisID _axis, double _maxValue);

		void repaintPlotAxis(AbstractPlotAxis::AxisID _axis);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		PlotBase* getOwner() const { return m_owner; }

		const Plot1DCfg& getConfig() const;

	protected:
		void setPlotAxis(AbstractPlotAxis* _axisXBottom, AbstractPlotAxis* _axisXTop, AbstractPlotAxis* _axisYLeft, AbstractPlotAxis* _axisYRight);

	private:
		PlotBase* m_owner;

		AbstractPlotAxis* m_axisXBottom;
		AbstractPlotAxis* m_axisXTop;
		AbstractPlotAxis* m_axisYLeft;
		AbstractPlotAxis* m_axisYRight;
	};

}