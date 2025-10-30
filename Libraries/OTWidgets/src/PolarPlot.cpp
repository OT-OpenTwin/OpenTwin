// @otlicense
// File: PolarPlot.cpp
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
	: AbstractPlot(_owner), m_legend(nullptr)
{
	m_grid = new PolarPlotGrid(this);
	m_magnifier = new PolarPlotMagnifier(this);
	m_panner = new PolarPlotPanner(this);

	this->setPlotAxis(new PolarPlotAxis(AbstractPlotAxis::xBottom, this), nullptr, new PolarPlotAxis(AbstractPlotAxis::yLeft, this), nullptr);
}

ot::PolarPlot::~PolarPlot() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Plot

void ot::PolarPlot::updateLegend(void) {
	if (this->getConfiguration().getLegendVisible()) {
		if (!m_legend) {
			m_legend = new PolarPlotLegend(this);
			this->insertLegend(m_legend);
		}
	}
	else {
		if (m_legend) {
			// This destroys the legend
			this->insertLegend(nullptr);
			m_legend = nullptr;
		}
	}
}

void ot::PolarPlot::updateWholePlot(void) {
	this->getPlotAxis(AbstractPlotAxis::xBottom)->updateAxis();
	this->getPlotAxis(AbstractPlotAxis::yLeft)->updateAxis();

	this->replot();
}

void ot::PolarPlot::clearPlot(void) {

}

void ot::PolarPlot::resetPlotView(void) {

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
