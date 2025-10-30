// @otlicense

// OpenTwin header
#include "OTWidgets/PolarPlot.h"
#include "OTWidgets/PolarPlotPanner.h"

ot::PolarPlotPanner::PolarPlotPanner(PolarPlot* _plot)
	: QwtPolarPanner(_plot->canvas()), m_plot(_plot) 
{
	this->setMouseButton(Qt::MouseButton::MiddleButton);
	this->setEnabled(true);
}

ot::PolarPlotPanner::~PolarPlotPanner() {

}
