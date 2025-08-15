//! @file PolarPlotAxis.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PolarPlot.h"
#include "OTWidgets/PolarPlotAxis.h"

// Qwt header
#include <qwt_scale_engine.h>

ot::PolarPlotAxis::PolarPlotAxis(AxisID _axisID, ot::PolarPlot* _plot)
	: AbstractPlotAxis(_axisID), m_plot(_plot) 
{
	m_plot->setScaleEngine(this->getPolarAxisID(), new QwtLinearScaleEngine());
}

ot::PolarPlotAxis::~PolarPlotAxis() {

}

void ot::PolarPlotAxis::updateAxis(void) {
	if (this->getIsLogScale() && !this->getIsLogScaleSet()) {
		this->setIsLogScaleSet(true);
		m_plot->setScaleEngine(this->getCartesianAxisID(), new QwtLogScaleEngine());

	}
	else if (!this->getIsLogScale() && this->getIsLogScaleSet()) {
		this->setIsLogScaleSet(false);
		m_plot->setScaleEngine(this->getCartesianAxisID(), new QwtLinearScaleEngine());
	}
	
	//NOTE, auto scale required?
	//m_plot->setAutoScale(this->getPolarAxisID());

	//NOTE, axis title?

	if (!this->getIsAutoScale()) {
		//m_plot->setAxisMaxMajor(this->getPolarAxisID(), m_maxValue);
		//m_plot->setAxisMaxMinor(this->getPolarAxisID(), m_minValue);
	}

}
