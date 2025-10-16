//! @file WrappedLineItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedLineItem.h"
#include "WrappedItemFactory.h"
#include "GraphicsItemDesignerNavigation.h"

// OpenTwin header
#include "OTGui/FillPainter2D.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsLineItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsScene.h"

static WrappedItemFactoryRegistrar<WrappedLineItem> circleRegistrar(ot::GraphicsLineItemCfg::className());

WrappedLineItem::WrappedLineItem() {
	this->setLineStyle(ot::PenFCfg(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemLineColor)));
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

void WrappedLineItem::setupDesignerItemFromConfig(const ot::GraphicsItemCfg* _config) {
	if (!this->setupFromConfig(_config)) return;

	QList<QPointF> newControlPoints;
	newControlPoints.append(ot::QtFactory::toQPoint(this->getFrom()) + this->pos());
	newControlPoints.append(ot::QtFactory::toQPoint(this->getTo()) + this->pos());

	this->initializeBaseData(newControlPoints, this->pos());
}

// ###########################################################################################################################################################################################################################################################################################################################

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

	this->setGraphicsItemPos(delta);
	this->setLine(p1 - delta, p2 - delta);
}

void WrappedLineItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	this->fillBasePropertyGrid(cfg);
	
	PropertyGroup* lineGroup = new PropertyGroup("Line");
	PropertyDouble* x1Prop = new PropertyDouble("X1", this->getLine().x1(), 0., std::numeric_limits<double>::max());
	x1Prop->setPropertyTip("The X1 line position relative to the item's X position.");
	lineGroup->addProperty(x1Prop);
	PropertyDouble* y1Prop = new PropertyDouble("Y1", this->getLine().y1(), 0., std::numeric_limits<double>::max());
	y1Prop->setPropertyTip("The Y1 line position relative to the item's Y position.");
	lineGroup->addProperty(y1Prop);
	PropertyDouble* x2Prop = new PropertyDouble("X2", this->getLine().x2(), 0., std::numeric_limits<double>::max());
	x2Prop->setPropertyTip("The X2 line position relative to the item's X position.");
	lineGroup->addProperty(x2Prop);
	PropertyDouble* y2Prop = new PropertyDouble("Y2", this->getLine().y2(), 0., std::numeric_limits<double>::max());
	y2Prop->setPropertyTip("The Y2 line position relative to the item's Y position.");
	lineGroup->addProperty(y2Prop);
	lineGroup->addProperty(new PropertyPainter2D("Line Painter", this->getLineStyle().getPainter()));
	lineGroup->addProperty(new PropertyDouble("Line Width", this->getLineStyle().getWidth(), 0., std::numeric_limits<double>::max()));
	
	cfg.addRootGroup(lineGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedLineItem::propertyChanged(const ot::Property* _property) {
	using namespace ot;

	if (this->basePropertyChanged(_property)) return;

	const ot::PropertyGroup* group = _property->getParentGroup();
	if (!group) {
		OT_LOG_EA("Data mismatch");
		return;
	}

	if (group->getName() == "Line" && _property->getPropertyName() == "X1") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setLine(actualProperty->getValue(), this->getLine().y1(), this->getLine().x2(), this->getLine().y2());
	}
	else if (group->getName() == "Line" && _property->getPropertyName() == "Y1") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setLine(this->getLine().x1(), actualProperty->getValue(), this->getLine().x2(), this->getLine().y2());
	}
	else if (group->getName() == "Line" && _property->getPropertyName() == "X2") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setLine(this->getLine().x1(), this->getLine().y1(), actualProperty->getValue(), this->getLine().y2());
	}
	else if (group->getName() == "Line" && _property->getPropertyName() == "Y2") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setLine(this->getLine().x1(), this->getLine().y1(), this->getLine().x2(), actualProperty->getValue());
	}
	else if (group->getName() == "Line" && _property->getPropertyName() == "Line Painter") {
		const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		ot::PenFCfg lineStyle(this->getLineStyle());
		lineStyle.setPainter(actualProperty->getPainter()->createCopy());
		this->setLineStyle(lineStyle);
	}
	else if (group->getName() == "Line" && _property->getPropertyName() == "Line Width") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		ot::PenFCfg lineStyle(this->getLineStyle());
		lineStyle.setWidth(actualProperty->getValue());
		this->setLineStyle(lineStyle);
	}
	else {
		OT_LOG_E("Unknown property { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
	}
}

void WrappedLineItem::propertyDeleteRequested(const ot::Property* _property) {
	if (this->basePropertyDeleteRequested(_property)) return;

}

QVariant WrappedLineItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsLineItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}

