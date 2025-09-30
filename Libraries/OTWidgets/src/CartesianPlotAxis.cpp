//! @file CartesianPlotAxis.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/CartesianPlot.h"
#include "OTWidgets/CartesianPlotAxis.h"

// Qwt header
#include <qwt_scale_engine.h>

ot::CartesianPlotAxis::CartesianPlotAxis(AxisID _axisID, CartesianPlot* _plot) :
	AbstractPlotAxis(_axisID), m_plot(_plot)
{
	OTAssertNullptr(m_plot);
	m_plot->setAxisScaleEngine(this->getCartesianAxisID(), new QwtLinearScaleEngine());
}

ot::CartesianPlotAxis::~CartesianPlotAxis() {
	
}

void ot::CartesianPlotAxis::updateAxis(void) {
	m_plot->setAxisTitle(this->getCartesianAxisID(), this->getTitle());

	if (this->getIsLogScale() && !this->getIsLogScaleSet()) {
		this->setIsLogScaleSet(true);
		m_plot->setAxisScaleEngine(this->getCartesianAxisID(), new QwtLogScaleEngine());

	}
	else if (!this->getIsLogScale() && this->getIsLogScaleSet()) {
		this->setIsLogScaleSet(false);
		m_plot->setAxisScaleEngine(this->getCartesianAxisID(), new QwtLinearScaleEngine());
	}
	
	if (this->getIsAutoScale()) {
		m_plot->setAxisAutoScale(this->getCartesianAxisID(), this->getIsAutoScale());
	}
	else {
		m_plot->setAxisScale(this->getCartesianAxisID(), this->getMin(), this->getMax());
	}	

}
