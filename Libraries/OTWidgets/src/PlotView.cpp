//! @file PlotView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/Plot.h"
#include "OTWidgets/PlotView.h"

ot::PlotView::PlotView(Plot* _plot)
	: WidgetView(WidgetViewBase::View1D), m_plot(_plot)
{
	if (!m_plot) {
		m_plot = new Plot;
	}

	this->addWidgetToDock(this->getViewWidget());
}

ot::PlotView::~PlotView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::PlotView::getViewWidget(void) {
	return m_plot->getQWidget();
}
