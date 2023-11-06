//! @file GraphicsDirectConnectionItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsDirectConnectionItem.h"

ot::GraphicsDirectConnectionItem::GraphicsDirectConnectionItem() {

}

ot::GraphicsDirectConnectionItem::~GraphicsDirectConnectionItem() {

}

void ot::GraphicsDirectConnectionItem::updateConnection(void) {
	if (this->originItem() == nullptr || this->destItem() == nullptr) {
		OT_LOG_EA("Can not draw connection since to item were set");
		return;
	}

	QPointF orig = this->originItem()->getQGraphicsItem()->scenePos() + this->originItem()->getQGraphicsItem()->boundingRect().center();
	QPointF dest = this->destItem()->getQGraphicsItem()->scenePos() + this->destItem()->getQGraphicsItem()->boundingRect().center();

	this->setLine(QLineF(orig, dest));
}
