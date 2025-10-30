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
