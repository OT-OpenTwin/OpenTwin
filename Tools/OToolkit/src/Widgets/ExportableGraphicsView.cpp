// @otlicense
// File: ExportableGraphicsView.cpp
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
#include "Widgets/ExportableGraphicsView.h"
#include "OTWidgets/Graphics/GraphicsView.h"
#include "OTWidgets/WidgetView/WidgetViewDock.h"

// Qt header
#include <QtWidgets/qlayout.h>

using namespace ot;

ExportableGraphicsView::ExportableGraphicsView()
	: WidgetView(WidgetViewBase::ViewType::CustomView, nullptr),
	m_centralWidget(nullptr), m_graphicsView(nullptr)
{
	m_centralWidget = new QWidget(getViewDockWidget());
	QVBoxLayout* centralLayout = new QVBoxLayout(m_centralWidget);
	QHBoxLayout* topLayout = new QHBoxLayout;
	centralLayout->addLayout(topLayout, 0);
	
	m_graphicsView = new GraphicsView(m_centralWidget);
	centralLayout->addWidget(m_graphicsView, 1);
}

ExportableGraphicsView::~ExportableGraphicsView()
{
	m_centralWidget->deleteLater();
}
