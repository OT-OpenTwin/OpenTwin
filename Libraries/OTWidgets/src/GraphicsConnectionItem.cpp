//! @file GraphicsConnectionItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsConnectionItem.h"

ot::GraphicsConnectionItem::GraphicsConnectionItem() : m_origin(nullptr), m_dest(nullptr) {

}

ot::GraphicsConnectionItem::~GraphicsConnectionItem() {

}

void ot::GraphicsConnectionItem::connectItems(GraphicsItem* _origin, GraphicsItem* _dest) {
	OTAssertNullptr(_origin);
	OTAssertNullptr(_dest);
	otAssert(m_origin == nullptr, "Origin already set");
	otAssert(m_dest == nullptr, "Origin already set");
	m_origin = _origin;
	m_dest = _dest;
	m_origin->storeConnection(this);
	m_dest->storeConnection(this);
	this->updateConnection();
}

void ot::GraphicsConnectionItem::disconnectItems(void) {
	if (m_origin) {
		m_origin->forgetConnection(this);
		m_origin = nullptr;
	}
	if (m_dest) {
		m_dest->forgetConnection(this);
		m_dest = nullptr;
	}
}

void ot::GraphicsConnectionItem::updateConnection(void) {
	if (m_origin == nullptr || m_dest == nullptr) {
		OT_LOG_EA("Can not draw connection since to item were set");
		return;
	}

	QPointF orig = m_origin->getQGraphicsItem()->scenePos() + m_origin->getQGraphicsItem()->boundingRect().center();
	QPointF dest = m_dest->getQGraphicsItem()->scenePos() + m_dest->getQGraphicsItem()->boundingRect().center();

	//OT_LOG_W("Updating coonection { O.X = " + std::to_string(orig.x()) + "; O.Y = " + std::to_string(orig.y()) + "; D.X = " + std::to_string(dest.x()) + "; D.Y = " + std::to_string(dest.y()) + " }");

	this->setLine(QLineF(orig, dest));
}
