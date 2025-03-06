//! @file GraphicsItemDesignerPreview.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

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