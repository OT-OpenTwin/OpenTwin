// @otlicense
// File: GraphicsItemDesignerViewStatusOverlay.cpp
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

// OToolkit header
#include "GraphicsItemDesigner.h"
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerViewStatusOverlay.h"

// Qt header
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qscrollbar.h>

GraphicsItemDesignerViewStatusOverlay::GraphicsItemDesignerViewStatusOverlay(GraphicsItemDesignerView* _view) :
	ot::OverlayWidgetBase(_view, ot::Alignment::BottomLeft, QMargins(0, 0, 0, _view->horizontalScrollBar()->height()))
{
	QHBoxLayout* lay = new QHBoxLayout(this);
	m_posLabel = new QLabel("Pos: 0; 0");
	m_posLabel->setAlignment(Qt::AlignLeft);
	lay->addWidget(m_posLabel);

	this->setMaximumHeight(40);
}

void GraphicsItemDesignerViewStatusOverlay::setMousePosition(const QPointF& _pt) {
	m_posLabel->setText("Pos: " + QString::number(_pt.x(), 'f', 2) + "; " + QString::number(_pt.y(), 'f', 2));
}