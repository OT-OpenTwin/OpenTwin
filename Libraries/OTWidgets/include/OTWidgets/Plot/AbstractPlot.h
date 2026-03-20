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

	//! @brief Abstract base class for 1D and Polar plots.
	class OT_WIDGETS_API_EXPORT AbstractPlot : public WidgetBase {
		OT_DECL_NOCOPY(AbstractPlot)
		OT_DECL_NODEFAULT(AbstractPlot)
	public:
		explicit AbstractPlot(PlotBase* _ownerPlot);

		virtual ~AbstractPlot();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Plot

		//! @brief Updates the grid properties (visibility, color, line width, ...) on the plot.
		virtual void updateGrid() = 0;

		virtual void clearPlot() = 0;

		virtual void resetPlotView() = 0;

		virtual Plot1DCfg::PlotType getPlotType() const = 0;

		virtual void updateAllAxes() = 0;

		//! @brief Update the entire plot by updating the axes and calling replot() on the plot widget.
		//! After replotting the zoomer and panner will be reset if needed.
		virtual void updateWholePlot() = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Axis

		bool hasPlotAxis(AbstractPlotAxis::AxisID _id) const;

		//! @brief Returns the plot axis for the given identifier.
		//! The plot keeps ownership of the axis.
		//! @throws ot::InvalidArgumentException if the provided axis identifier is not valid for this plot type (e.g. xBottom or xTop for Polar plots).
		//! @throws ot::ObjectNotFoundException if the axis for the provided identifier does not exist (e.g. yRight axis for a plot that only has a left Y-axis).
		//! @param _id Identifier of the axis to return.
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