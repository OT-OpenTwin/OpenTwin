//! @file PlotManagerView.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "stdafx.h"

// OpenTwin header
#include "PlotManager.h"
#include "PlotManagerView.h"

ot::PlotManagerView::PlotManagerView()
	: WidgetView(WidgetViewBase::ViewVersion)
{
	m_plotManager = new PlotManager;

	this->addWidgetInterfaceToDock(m_plotManager);
}

ot::PlotManagerView::~PlotManagerView() {
	delete m_plotManager;
}

QWidget* ot::PlotManagerView::getViewWidget(void) {
	return m_plotManager->getQWidget();
}
