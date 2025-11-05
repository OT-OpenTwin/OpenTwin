// @otlicense
// File: ViewerView.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "stdafx.h"

// OpenTwin header
#include "Viewer.h"
#include "ViewerView.h"
#include "OTWidgets/WidgetViewDock.h"

ot::ViewerView::ViewerView(ot::UID _modelID, ot::UID _viewerID, double _scaleWidth, double _scaleHeight, int _backgroundR, int _backgroundG, int _backgroundB, int _overlayTextR, int _overlayTextG, int _overlayTextB, QWidget* _parent)
	: WidgetView(WidgetViewBase::View3D, _parent)
{
	m_viewer = new Viewer(_modelID, _viewerID, _scaleWidth, _scaleHeight, _backgroundR, _backgroundG, _backgroundB, _overlayTextR, _overlayTextG, _overlayTextB, getViewDockWidget());
	this->addWidgetInterfaceToDock(m_viewer);
}

ot::ViewerView::~ViewerView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::ViewerView::getViewWidget(void) {
	return m_viewer;
}
