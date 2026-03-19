// @otlicense
// File: PolarPlotAxis.cpp
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
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Plot/PlotBase.h"
#include "OTWidgets/Plot/PlotScaleDraw.h"
#include "OTWidgets/Plot/Polar/PolarPlot.h"
#include "OTWidgets/Plot/Polar/PolarPlotAxis.h"

// Qwt header
#include <qwt_scale_engine.h>

ot::PolarPlotAxis::PolarPlotAxis(AxisID _axisID, ot::PolarPlot* _plot)
	: AbstractPlotAxis(_axisID), m_plot(_plot) 
{
	m_plot->setScaleEngine(this->getPolarAxisID(), new QwtLinearScaleEngine());

	m_scaleDraw = new PlotScaleDraw;
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
	
	PlotBase* base = m_plot->getOwner();
	OTAssertNullptr(base);
	const Plot1DCfg& cfg = base->getConfig();
	String::DisplayNumberFormat numberFormat = String::Auto;
	int decimals = 3;

	AbstractPlotAxis::AxisID axisId = getAxisID();
	if (axisId == xBottom || axisId == xTop)
	{
		numberFormat = cfg.getXAxisDisplayNumberFormat();
		decimals = cfg.getXAxisDisplayNumberPrecision();
	}
	else
	{
		numberFormat = cfg.getYAxisDisplayNumberFormat();
		decimals = cfg.getYAxisDisplayNumberPrecision();
	}

	m_scaleDraw->setNumberFormat(numberFormat);
	m_scaleDraw->setNumberPrecision(decimals);
	m_scaleDraw->invalidateCache();
}
