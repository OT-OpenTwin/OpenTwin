//! @file WrappedLineItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedLineItem.h"

// OpenTwin header
#include "OTWidgets/GraphicsScene.h"

WrappedLineItem::WrappedLineItem() {

}

WrappedLineItem::~WrappedLineItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

ot::TreeWidgetItemInfo WrappedLineItem::createNavigationInformation(void) {
	ot::TreeWidgetItemInfo info;
	info.setText(QString::fromStdString(this->getGraphicsItemName()));
	info.setIcon(ot::IconManager::getIcon("GraphicsEditor/Line.png"));

	return info;
}

void WrappedLineItem::controlPointsChanged(void) {
	if (this->getControlPoints().size() != 2) return;

	ot::GraphicsScene* gscene = this->getGraphicsScene();
	if (!gscene) {
		OT_LOG_E("Graphics scene not set");
		return;
	}

	this->prepareGeometryChange();

	QPointF p1 = this->getControlPoints().front();
	QPointF p2 = this->getControlPoints().back();

	QPointF delta = QPointF(std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y()));
	
	this->setPos(delta);
	this->setLine(p1 - delta, p2 - delta);
}

void WrappedLineItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	PropertyGroup* generalGroup = new PropertyGroup("General");
	generalGroup->addProperty(new PropertyString("Name", this->getGraphicsItem()->getGraphicsItemName()));

	PropertyGroup* geometryGroup = new PropertyGroup("Geometry");
	geometryGroup->addProperty(new PropertyDouble("X1", this->getLine().x1()));
	geometryGroup->addProperty(new PropertyDouble("Y1", this->getLine().y1()));
	geometryGroup->addProperty(new PropertyDouble("X2", this->getLine().x2()));
	geometryGroup->addProperty(new PropertyDouble("Y2", this->getLine().y2()));

	cfg.addRootGroup(generalGroup);
	cfg.addRootGroup(geometryGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedLineItem::propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {
	using namespace ot;

	if (_item->getGroupName() == "General" && _itemData.propertyName() == "Name") {
		PropertyInputString* input = dynamic_cast<PropertyInputString*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed { \"Group\": \"" + _item->getGroupName() + "\", \"");
			return;
		}

		OTAssertNullptr(this->getNavigationItem());
		this->setGraphicsItemName(input->getCurrentText().toStdString());
		this->getNavigationItem()->setText(0, input->getCurrentText());
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "X1") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setLine(input->getValue(), this->getLine().y1(), this->getLine().x2(), this->getLine().y2());
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Y1") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setLine(this->getLine().x1(), input->getValue(), this->getLine().x2(), this->getLine().y2());
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "X2") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setLine(this->getLine().x1(), this->getLine().y1(), input->getValue(), this->getLine().y2());
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Y2") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setLine(this->getLine().x1(), this->getLine().y1(), this->getLine().x2(), input->getValue());
	}
}

void WrappedLineItem::propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {

}