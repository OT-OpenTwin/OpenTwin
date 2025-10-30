// @otlicense

// OpenTwin header
#include "OTWidgets/PolarPlot.h"
#include "OTWidgets/PolarPlotGrid.h"

ot::PolarPlotGrid::PolarPlotGrid(PolarPlot* _plot) : m_plot(_plot) {
	this->attach(m_plot);
}

ot::PolarPlotGrid::~PolarPlotGrid() {

}
