// @otlicense
// File: GraphicsItemDesignerInfoOverlay.cpp
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
#include "GraphicsItemDesignerInfoOverlay.h"

// Qt header
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qscrollbar.h>

GraphicsItemDesignerInfoOverlay::GraphicsItemDesignerInfoOverlay(const QString& _text, GraphicsItemDesignerView* _view, const QMargins& _overlayMargins)
	: ot::OverlayWidgetBase(_view, ot::Alignment::Top, _overlayMargins)
{
	QHBoxLayout* lay = new QHBoxLayout(this);
	m_label = new QLabel(_text);
	m_label->setAlignment(Qt::AlignCenter);
	lay->addWidget(m_label);

	this->setMaximumHeight(40);
}

void GraphicsItemDesignerInfoOverlay::setText(const QString& _text) {
	m_label->setText(_text);
}