//! @file WrappedRectItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedRectItem.h"

// OpenTwin header
#include "OTWidgets/GraphicsScene.h"

WrappedRectItem::WrappedRectItem() {

}

WrappedRectItem::~WrappedRectItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

ot::TreeWidgetItemInfo WrappedRectItem::createNavigationInformation(void) {
	ot::TreeWidgetItemInfo info;
	info.setText(QString::fromStdString(this->getGraphicsItemName()));
	info.setIcon(ot::IconManager::getIcon("GraphicsEditor/Rect.png"));

	return info;
}

void WrappedRectItem::controlPointsChanged(void) {
	if (this->getControlPoints().size() != 2) return;

	ot::GraphicsScene* gscene = this->getGraphicsScene();
	if (!gscene) {
		OT_LOG_E("Graphics scene not set");
		return;
	}

	this->prepareGeometryChange();

	QPointF p1 = this->getControlPoints().front();
	QPointF p2 = this->getControlPoints().back();

	QPointF topLeft = QPointF(std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y()));
	QSizeF newSize = QSizeF(
		std::max(p1.x(), p2.x()) - std::min(p1.x(), p2.x()),
		std::max(p1.y(), p2.y()) - std::min(p1.y(), p2.y())
	);
	this->setPos(topLeft);
	this->setRectangleSize(newSize);
}

void WrappedRectItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	PropertyGroup* generalGroup = new PropertyGroup("General");
	generalGroup->addProperty(new PropertyString("Name", this->getGraphicsItem()->getGraphicsItemName()));

	PropertyGroup* geometryGroup = new PropertyGroup("Geometry");
	geometryGroup->addProperty(new PropertyDouble("X", this->pos().x()));
	geometryGroup->addProperty(new PropertyDouble("Y", this->pos().y()));
	geometryGroup->addProperty(new PropertyDouble("Width", this->rectangleSize().width()));
	geometryGroup->addProperty(new PropertyDouble("Height", this->rectangleSize().height()));

	cfg.addRootGroup(generalGroup);
	cfg.addRootGroup(geometryGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedRectItem::propertyChanged(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {
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
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "X") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->prepareGeometryChange();
		this->setPos(input->getValue(), this->y());
		this->setGeometry(QRectF(this->pos(), this->rectangleSize()));
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Y") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->prepareGeometryChange();
		this->setPos(this->x(), input->getValue());
		this->setGeometry(QRectF(this->pos(), this->rectangleSize()));
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Width") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setRectangleSize(QSizeF(input->getValue(), this->rectangleSize().height()));
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Height") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		this->setRectangleSize(QSizeF(this->rectangleSize().width(), input->getValue()));
	}
}

void WrappedRectItem::propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {

}

QVariant WrappedRectItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsRectangularItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}