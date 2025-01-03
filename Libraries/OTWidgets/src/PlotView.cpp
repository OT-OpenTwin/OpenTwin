//! @file PlotView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PlotView.h"

ot::PlotView::PlotView() 
	: WidgetView(WidgetViewBase::View1D)
{
	this->addWidgetToDock(this->getQWidget());
}

ot::PlotView::~PlotView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::PlotView::getViewWidget(void) {
	return this->getQWidget();
}
