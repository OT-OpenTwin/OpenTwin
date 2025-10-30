// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Plot1DCfg.h"
#include "OTWidgets/AbstractPlotAxis.h"

// Qt header
#include <QtGui/qcolor.h>

namespace ot {

	class PlotBase;

	class OT_WIDGETS_API_EXPORT AbstractPlot {
		OT_DECL_NOCOPY(AbstractPlot)
		OT_DECL_NODEFAULT(AbstractPlot)
	public:
		AbstractPlot(PlotBase* _ownerPlot);

		virtual ~AbstractPlot();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Plot

		void setPlotLegendVisible(bool _isVisible = true, bool _repaint = true);

		virtual void updateLegend(void) = 0;

		virtual void updateGrid(void) = 0;

		virtual void updateWholePlot(void) = 0;

		virtual void clearPlot(void) = 0;

		virtual void resetPlotView(void) = 0;

		virtual Plot1DCfg::PlotType getPlotType(void) const = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Axis

		void setPlotAxis(AbstractPlotAxis* _axisXBottom, AbstractPlotAxis* _axisXTop, AbstractPlotAxis* _axisYLeft, AbstractPlotAxis* _axisYRight);

		AbstractPlotAxis* getPlotAxis(AbstractPlotAxis::AxisID _id);

		void setPlotAxisTitle(AbstractPlotAxis::AxisID _axis, const QString& _title);

		void setPlotAxisAutoScale(AbstractPlotAxis::AxisID _axis, bool _isAutoScale);

		void setPlotAxisLogScale(AbstractPlotAxis::AxisID _axis, bool _isLogScale);

		void setPlotAxisMin(AbstractPlotAxis::AxisID _axis, double _minValue);

		void setPlotAxisMax(AbstractPlotAxis::AxisID _axis, double _maxValue);

		void repaintPlotAxis(AbstractPlotAxis::AxisID _axis);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Grid

		void setPlotGridVisible(bool _isVisible, bool _repaint = true);

		void setPlotGridColor(const Color& _color, bool _repaint = true);

		void setPlotGridLineWidth(double _width, bool _repaint = true);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		PlotBase* getOwner(void) const { return m_owner; }

		const Plot1DCfg& getConfiguration(void) const { return m_config; };

	private:
		PlotBase* m_owner;
		Plot1DCfg m_config;

		AbstractPlotAxis* m_axisXBottom;
		AbstractPlotAxis* m_axisXTop;
		AbstractPlotAxis* m_axisYLeft;
		AbstractPlotAxis* m_axisYRight;
	};

}