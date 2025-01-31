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
	this->addWidgetInterfaceToDock(m_tree);
}

ot::NavigationTreeView::~NavigationTreeView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::NavigationTreeView::getViewWidget(void) {
	return m_tree->widget();
}
