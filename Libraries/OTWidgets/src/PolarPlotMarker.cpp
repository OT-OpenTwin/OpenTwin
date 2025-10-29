// @otlicense

//! @file PolarPlotMarker.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTWidgets/PolarPlot.h"
#include "OTWidgets/PolarPlotMarker.h"

ot::PolarPlotMarker::PolarPlotMarker(PolarPlot* _plot) : m_plot(_plot) {
	OTAssertNullptr(m_plot);
	this->attach(m_plot);
}

ot::PolarPlotMarker::~PolarPlotMarker() {

}