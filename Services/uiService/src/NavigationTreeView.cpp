//! @file NavigationTreeView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "stdafx.h"

// OpenTwin header
#include "NavigationTreeView.h"

ot::NavigationTreeView::NavigationTreeView()
	: WidgetView(WidgetViewBase::ViewNavigation)
{
	this->addWidgetToDock(this->widget());
}

ot::NavigationTreeView::~NavigationTreeView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::NavigationTreeView::getViewWidget(void) {
	return this->widget();
}
