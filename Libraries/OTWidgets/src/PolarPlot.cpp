//! @file PolarPlot.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/PlotBase.h"
#include "OTWidgets/PolarPlot.h"
#include "OTWidgets/PolarPlotGrid.h"
#include "OTWidgets/PolarPlotAxis.h"
#include "OTWidgets/PolarPlotLegend.h"
#include "OTWidgets/PolarPlotPanner.h"
#include "OTWidgets/PolarPlotMagnifier.h"

// Qt header
#include <QtGui/qevent.h>

ot::PolarPlot::PolarPlot(PlotBase* _owner)
	: AbstractPlot(_owner)
{
	m_grid = new PolarPlotGrid(this);
	m_legend = new PolarPlotLegend(this);
	m_magnifier = new PolarPlotMagnifier(this);
	m_panner = new PolarPlotPanner(this);

	this->setPlotAxis(new PolarPlotAxis(AbstractPlotAxis::xBottom, this), nullptr, new PolarPlotAxis(AbstractPlotAxis::yLeft, this), nullptr);
}

ot::PolarPlot::~PolarPlot() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Plot

void ot::PolarPlot::updateLegend(void) {
	
}

void ot::PolarPlot::updateWholePlot(void) {
	this->getPlotAxis(AbstractPlotAxis::xBottom)->updateAxis();
	this->getPlotAxis(AbstractPlotAxis::yLeft)->updateAxis();

	this->replot();
}

void ot::PolarPlot::clearPlot(void) {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Grid

void ot::PolarPlot::updateGrid(void) {
	const Plot1DCfg& config = this->getConfiguration();

	QPen gridPen(QColor(), 0., Qt::NoPen);

	if (config.getGridVisible()) {
		gridPen = QPen(QtFactory::toQColor(config.getGridColor()), config.getGridLineWidth(), Qt::SolidLine);
	}
	m_grid->setPen(gridPen);
	m_grid->setAxisPen(QwtPolar::Axis::AxisAzimuth, gridPen);
	m_grid->setAxisPen(QwtPolar::Axis::AxisBottom, gridPen);
	m_grid->setAxisPen(QwtPolar::Axis::AxisLeft, gridPen);
	m_grid->setAxisPen(QwtPolar::Axis::AxisRight, gridPen);
	m_grid->setAxisPen(QwtPolar::Axis::AxisTop, gridPen);
}
