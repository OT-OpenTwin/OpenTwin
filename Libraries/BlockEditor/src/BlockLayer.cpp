//! @file BlockLayer.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockLayer.h"
#include "OTBlockEditor/DefaultBlock.h"

ot::BlockLayer::BlockLayer(ot::DefaultBlock* _block) : m_block(_block) {

};

ot::BlockLayer::~BlockLayer() {

}

QRectF ot::BlockLayer::layerRect(void) const {
	QRectF r = m_block->boundingRect();
	r.marginsRemoved(m_margins);

	return r;
}

void ot::BlockLayer::setMargins(const ot::MarginsD& _margins) {
	m_margins.setBottom(_margins.bottom());
	m_margins.setLeft(_margins.left());
	m_margins.setRight(_margins.right());
	m_margins.setTop(_margins.top());
}