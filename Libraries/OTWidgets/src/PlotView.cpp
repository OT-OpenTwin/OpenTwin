// @otlicense

//! @file PlotView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PlotView.h"

ot::PlotView::PlotView()
	: WidgetView(WidgetViewBase::View1D) {
	m_plot = new Plot();

	this->addWidgetInterfaceToDock(m_plot);
}

ot::PlotView::~PlotView() {
	delete m_plot;
}

QWidget* ot::PlotView::getViewWidget(void) {
	return m_plot->getQWidget();
}
