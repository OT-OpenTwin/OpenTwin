//! @file GraphicsConnectionItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsConnectionItem.h"
#include "OTWidgets/GraphicsItem.h"
#include "OpenTwinCore/otAssert.h"

ot::GraphicsConnectionItem::GraphicsConnectionItem() : m_dest(nullptr), m_origin(nullptr) {

}

ot::GraphicsConnectionItem::~GraphicsConnectionItem() {

}

ot::GraphicsConnectionCfg ot::GraphicsConnectionItem::getConnectionInformation(void) const {
	ot::GraphicsConnectionCfg info;
	if (m_origin) {
		info.setOriginUid(m_origin->getRootItem()->graphicsItemUid());
		info.setOriginConnectable(m_origin->graphicsItemName());
	}
	else {
		OT_LOG_WA("No origin item set");
	}
	if (m_dest) {
		info.setDestUid(m_dest->getRootItem()->graphicsItemUid());
		info.setDestConnectable(m_dest->graphicsItemName());
	}
	else {
		OT_LOG_WA("No dest item set");
	}
	return info;
}

void ot::GraphicsConnectionItem::connectItems(GraphicsItem* _origin, GraphicsItem* _dest) {
	OTAssertNullptr(_origin);
	OTAssertNullptr(_dest);
	otAssert(m_origin == nullptr, "Origin already set");
	otAssert(m_dest == nullptr, "Destination already set");
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

}
