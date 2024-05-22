//! @file WrappedLineItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedLineItem.h"

WrappedLineItem::WrappedLineItem() {

}

WrappedLineItem::~WrappedLineItem() {

}

bool WrappedLineItem::rebuildItem(void) {
	if (this->controlPoints().size() != 2) return false;
	
	QPointF p1 = this->controlPoints().front();
	QPointF p2 = this->controlPoints().back();

	this->prepareGeometryChange();
	
	QPointF delta = QPointF(std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y()));
	this->setPos(delta);

	this->setLine(p1 - delta, p2 - delta);
	
	return true;
}

ot::TreeWidgetItemInfo WrappedLineItem::createNavigationInformation(void) const {
	ot::TreeWidgetItemInfo info;
	info.setText(QString::fromStdString(this->graphicsItemName()));

	return info;
}