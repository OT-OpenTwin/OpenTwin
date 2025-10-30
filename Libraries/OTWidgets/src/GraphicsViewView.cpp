// @otlicense
// File: GraphicsViewView.cpp
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

// OpenTwin header
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/GraphicsViewView.h"

ot::GraphicsViewView::GraphicsViewView(GraphicsView* _graphicsView)
	: WidgetView(WidgetViewBase::ViewGraphics), m_graphicsView(_graphicsView)
{
	if (!m_graphicsView) {
		m_graphicsView = new GraphicsView;
	}

	this->addWidgetInterfaceToDock(m_graphicsView);
}

ot::GraphicsViewView::~GraphicsViewView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::GraphicsViewView::getViewWidget(void) {
	return m_graphicsView;
}

void ot::GraphicsViewView::viewRenamed() {
	ot::WidgetView::viewRenamed();
	this->getGraphicsView()->setGraphicsViewName(this->getViewData().getEntityName());
}
