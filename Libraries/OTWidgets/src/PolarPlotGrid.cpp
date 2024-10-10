//! @file PolarPlotGrid.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PolarPlot.h"
#include "OTWidgets/PolarPlotGrid.h"

ot::PolarPlotGrid::PolarPlotGrid(PolarPlot* _plot)
	: m_plot(_plot)
{
	this->attach(m_plot);
}

ot::PolarPlotGrid::~PolarPlotGrid() {

}
