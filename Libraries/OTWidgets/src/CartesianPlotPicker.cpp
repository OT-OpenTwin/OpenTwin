//! @file CartesianPlotPicker.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/CartesianPlot.h"
#include "OTWidgets/CartesianPlotPicker.h"

ot::CartesianPlotPicker::CartesianPlotPicker(CartesianPlot* _plot)
	: QwtPlotPicker(_plot->canvas()), m_plot(_plot) 
{

}

ot::CartesianPlotPicker::~CartesianPlotPicker() {

}