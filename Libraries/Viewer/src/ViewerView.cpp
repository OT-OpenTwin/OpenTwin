//! @file ViewerView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "stdafx.h"

// OpenTwin header
#include "ViewerView.h"

ot::ViewerView::ViewerView(ot::UID _modelID, ot::UID _viewerID, double _scaleWidth, double _scaleHeight, int _backgroundR, int _backgroundG, int _backgroundB, int _overlayTextR, int _overlayTextG, int _overlayTextB)
	: Viewer(_modelID, _viewerID, _scaleWidth, _scaleHeight, _backgroundR, _backgroundG, _backgroundB, _overlayTextR, _overlayTextG, _overlayTextB)
{
	this->addWidgetToDock(this);
}

ot::ViewerView::~ViewerView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::ViewerView::getViewWidget(void) {
	return this;
}
