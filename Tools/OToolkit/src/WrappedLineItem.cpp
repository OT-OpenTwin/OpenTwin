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

// ###########################################################################################################################################################################################################################################################################################################################

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
	info.setText(QString::fromStdString(this->getGraphicsItemName()));
	info.setIcon(ot::IconManager::getIcon("GraphicsEditor/Line.png"));

	return info;
}

void WrappedLineItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	PropertyGroup* generalGroup = new PropertyGroup("General");
	generalGroup->addProperty(new PropertyString("Name", this->getGraphicsItem()->getGraphicsItemName()));

	cfg.addRootGroup(generalGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedLineItem::propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {

}

void WrappedLineItem::propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {

}