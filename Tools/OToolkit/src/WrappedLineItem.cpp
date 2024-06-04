//! @file WrappedLineItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedLineItem.h"
#include "GraphicsItemDesignerNavigation.h"

// OpenTwin header
#include "OTGui/FillPainter2D.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTWidgets/GraphicsScene.h"

WrappedLineItem::WrappedLineItem() {
	this->setLineStyle(ot::OutlineF(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)));
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
	geometryGroup->addProperty(new PropertyPainter2D("Line Painter", this->getLineStyle().painter()));
	geometryGroup->addProperty(new PropertyDouble("Line Width", this->getLineStyle().width()));

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
		OTAssertNullptr(this->getNavigation());

		if (input->getCurrentText().isEmpty()) return;
		if (!this->getNavigation()->updateItemName(QString::fromStdString(this->getGraphicsItemName()), input->getCurrentText())) return;

		this->setGraphicsItemName(input->getCurrentText().toStdString());
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
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Line Painter") {
		PropertyInputPainter2D* input = dynamic_cast<PropertyInputPainter2D*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		ot::OutlineF lineStyle = this->getLineStyle();
		lineStyle.setPainter(input->getPainter()->createCopy());
		this->setLineStyle(lineStyle);
	}
	else if (_item->getGroupName() == "Geometry" && _itemData.propertyName() == "Line Width") {
		PropertyInputDouble* input = dynamic_cast<PropertyInputDouble*>(_item->getInput());
		if (!input) {
			OT_LOG_E("Input cast failed");
			return;
		}

		ot::OutlineF lineStyle = this->getLineStyle();
		lineStyle.setWidth(input->getValue());
		this->setLineStyle(lineStyle);
	}

}

void WrappedLineItem::propertyDeleteRequested(ot::PropertyGridItem* _item, const ot::PropertyBase& _itemData) {

}

QVariant WrappedLineItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsLineItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}

