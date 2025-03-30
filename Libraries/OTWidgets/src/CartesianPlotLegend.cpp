//! @file CartesianPlotLegend.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/CartesianPlot.h"
#include "OTWidgets/CartesianPlotLegend.h"

ot::CartesianPlotLegend::CartesianPlotLegend(CartesianPlot* _owner) :
	m_owner(_owner) 
{
	m_owner->insertLegend(this);
}

ot::CartesianPlotLegend::~CartesianPlotLegend() {

}
