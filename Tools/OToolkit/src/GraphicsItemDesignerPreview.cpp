// @otlicense
// File: GraphicsItemDesignerPreview.cpp
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
#include "GraphicsItemDesignerPreview.h"

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsItemFactory.h"

GraphicsItemDesignerPreview::GraphicsItemDesignerPreview() 
	: m_item(nullptr)
{
	this->getGraphicsScene()->setGridFlags(ot::Grid::ShowNormalLines | ot::Grid::ShowWideLines | ot::Grid::AutoScaleGrid);
}

GraphicsItemDesignerPreview::~GraphicsItemDesignerPreview() {

}


void GraphicsItemDesignerPreview::updateCurrentItem(const ot::GraphicsItemCfg* _newConfiguration) {
	if (!_newConfiguration) return;

	if (m_item) {
		this->removeItem(m_item->getGraphicsItemUid());
		m_item = nullptr;
	}

	m_item = ot::GraphicsItemFactory::itemFromConfig(_newConfiguration);
	if (!m_item) return;
	this->addItem(m_item);

	this->fitInView(m_item->getQGraphicsItem(), Qt::KeepAspectRatio);

	const ViewportAnchor anchor = this->transformationAnchor();
	this->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
	qreal factor = 0.1;
	
	this->scale(factor, factor);
	this->update();

	this->setTransformationAnchor(anchor);

	this->ensureViewInBounds();
}