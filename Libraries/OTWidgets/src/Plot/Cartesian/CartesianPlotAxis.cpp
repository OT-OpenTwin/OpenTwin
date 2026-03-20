// @otlicense
// File: CartesianPlotAxis.cpp
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
#include "OTCore/Logging/LogDispatcher.h"
#include "OTWidgets/Plot/PlotBase.h"
#include "OTWidgets/Plot/PlotScaleDraw.h"
#include "OTWidgets/Plot/Cartesian/CartesianPlot.h"
#include "OTWidgets/Plot/Cartesian/CartesianPlotAxis.h"

// Qwt header
#include <qwt_scale_engine.h>

ot::CartesianPlotAxis::CartesianPlotAxis(AxisID _axisID, CartesianPlot* _plot) :
	AbstractPlotAxis(_axisID), m_plot(_plot), m_scaleDraw(nullptr)
{
	OTAssertNullptr(m_plot);
	m_plot->setAxisScaleEngine(getCartesianAxisID(), new QwtLinearScaleEngine());

	m_scaleDraw = new PlotScaleDraw;
	m_plot->setAxisScaleDraw(getCartesianAxisID(), m_scaleDraw);
}

ot::CartesianPlotAxis::~CartesianPlotAxis()
{

}

void ot::CartesianPlotAxis::updateAxis()
{
	m_plot->setAxisTitle(getCartesianAxisID(), getTitle());

	if (getIsLogScale() && !getIsLogScaleSet())
	{
		setIsLogScaleSet(true);
		m_plot->setAxisScaleEngine(getCartesianAxisID(), new QwtLogScaleEngine());

	}
	else if (!getIsLogScale() && getIsLogScaleSet())
	{
		setIsLogScaleSet(false);
		m_plot->setAxisScaleEngine(getCartesianAxisID(), new QwtLinearScaleEngine());
	}

	if (getIsAutoScale())
	{
		m_plot->setAxisAutoScale(getCartesianAxisID(), getIsAutoScale());
	}
	else
	{
		m_plot->setAxisScale(getCartesianAxisID(), getMin(), getMax());
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