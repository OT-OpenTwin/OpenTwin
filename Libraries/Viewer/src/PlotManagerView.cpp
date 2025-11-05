// @otlicense
// File: PlotManagerView.cpp
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
#include "PlotManager.h"
#include "PlotManagerView.h"
#include "OTWidgets/WidgetViewDock.h"

ot::PlotManagerView::PlotManagerView(QWidget* _parent)
	: WidgetView(WidgetViewBase::View1D, _parent)
{
	m_plotManager = new PlotManager(getViewDockWidget());

	this->addWidgetInterfaceToDock(m_plotManager);
}

ot::PlotManagerView::~PlotManagerView() {
	delete m_plotManager;
}

QWidget* ot::PlotManagerView::getViewWidget(void) {
	return m_plotManager->getQWidget();
}
