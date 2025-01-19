//! @file NavigationTreeView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "stdafx.h"

// OpenTwin header
#include "NavigationTreeView.h"

#include "akWidgets/aTreeWidget.h"

ot::NavigationTreeView::NavigationTreeView()
	: WidgetView(WidgetViewBase::ViewNavigation)
{
	m_tree = new ak::aTreeWidget;
	this->addWidgetToDock(m_tree->widget());
}

ot::NavigationTreeView::~NavigationTreeView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::NavigationTreeView::getViewWidget(void) {
	return m_tree->widget();
}
