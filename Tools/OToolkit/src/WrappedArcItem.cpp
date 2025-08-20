//! @file WrappedArcItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedArcItem.h"
#include "WrappedItemFactory.h"
#include "GraphicsItemDesignerNavigation.h"

// OpenTwin header
#include "OTGui/FillPainter2D.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsArcItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsScene.h"

static WrappedItemFactoryRegistrar<WrappedArcItem> arcRegistrar(OT_FactoryKey_GraphicsArcItem);

WrappedArcItem::WrappedArcItem() {
	this->setLineStyle(ot::PenFCfg(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemLineColor)));
}

WrappedArcItem::~WrappedArcItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

ot::TreeWidgetItemInfo WrappedArcItem::createNavigationInformation(void) {
	ot::TreeWidgetItemInfo info;
	info.setText(QString::fromStdString(this->getGraphicsItemName()));
	info.setIcon(ot::IconManager::getIcon("GraphicsEditor/Arc.png"));

	return info;
}

void WrappedArcItem::setupDesignerItemFromConfig(const ot::GraphicsItemCfg* _config) {
	if (!this->setupFromConfig(_config)) return;

	QRectF newRect = ot::QtFactory::toQRect(this->getArcRect());
	newRect.moveTo(this->pos());

	QList<QPointF> newControlPoints;
	newControlPoints.append(newRect.topLeft());
	newControlPoints.append(newRect.bottomRight());

	this->initializeBaseData(newControlPoints, newRect.topLeft());
}

// ###########################################################################################################################################################################################################################################################################################################################

void WrappedArcItem::controlPointsChanged(void) {
	if (this->getControlPoints().size() != 2) return;

	ot::GraphicsScene* gscene = this->getGraphicsScene();
	if (!gscene) {
		OT_LOG_E("Graphics scene not set");
		return;
	}

	this->prepareGeometryChange();

	QPointF topLeftPoint = QPointF(std::min(this->getControlPoints().front().x(), this->getControlPoints().back().x()), std::min(this->getControlPoints().front().y(), this->getControlPoints().back().y()));
	QPointF bottomRightPoint = QPointF(std::max(this->getControlPoints().front().x(), this->getControlPoints().back().x()), std::max(this->getControlPoints().front().y(), this->getControlPoints().back().y()));

	this->setGraphicsItemPos(topLeftPoint);
	QRectF newArcRect(topLeftPoint, bottomRightPoint);
	newArcRect.moveTo(0., 0.);
	this->setArcRect(newArcRect);
}

void WrappedArcItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	this->fillBasePropertyGrid(cfg);

	PropertyGroup* arcGroup = new PropertyGroup("Arc");
	PropertyDouble* arcXProp = new PropertyDouble("Arc X", this->getArcRect().getLeft(), 0., std::numeric_limits<double>::max());
	arcXProp->setPropertyTip("X arc rect position relative to the item's X position.");
	arcGroup->addProperty(arcXProp);
	PropertyDouble* arcYProp = new PropertyDouble("Arc Y", this->getArcRect().getTop(), 0., std::numeric_limits<double>::max());
	arcYProp->setPropertyTip("Y arc rect position relative to the item's Y position.");
	arcGroup->addProperty(arcYProp);
	arcGroup->addProperty(new PropertyDouble("Width", this->getArcRect().getWidth(), 0., std::numeric_limits<double>::max()));
	arcGroup->addProperty(new PropertyDouble("Height", this->getArcRect().getHeight(), 0., std::numeric_limits<double>::max()));
	arcGroup->addProperty(new PropertyDouble("Start Angle", this->getStartAngle() / 16., 0., std::numeric_limits<double>::max()));
	arcGroup->addProperty(new PropertyDouble("Span Angle", this->getSpanAngle() / 16., 0., std::numeric_limits<double>::max()));
	arcGroup->addProperty(new PropertyPainter2D("Line Painter", this->getLineStyle().getPainter()));
	arcGroup->addProperty(new PropertyDouble("Line Width", this->getLineStyle().getWidth(), 0., std::numeric_limits<double>::max()));
	
	cfg.addRootGroup(arcGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedArcItem::propertyChanged(const ot::Property* _property) {
	using namespace ot;

	if (this->basePropertyChanged(_property)) return;

	const ot::PropertyGroup* group = _property->getParentGroup();
	if (!group) {
		OT_LOG_EA("Data mismatch");
		return;
	}
	
	if (group->getName() == "Arc" && _property->getPropertyName() == "Arc X") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		RectD newRect = this->getArcRect();
		newRect.moveTo(Point2DD(actualProperty->getValue(), newRect.getTop()));
		this->setArcRect(newRect);
	}
	else if (group->getName() == "Arc" && _property->getPropertyName() == "Arc Y") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		RectD newRect = this->getArcRect();
		newRect.moveTo(Point2DD(newRect.getLeft(), actualProperty->getValue()));
		this->setArcRect(newRect);
	}
	else if (group->getName() == "Arc" && _property->getPropertyName() == "Width") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		RectD newRect = this->getArcRect();
		newRect.setWidth(actualProperty->getValue());
		this->setArcRect(newRect);
	}
	else if (group->getName() == "Arc" && _property->getPropertyName() == "Height") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		RectD newRect = this->getArcRect();
		newRect.setHeight(actualProperty->getValue());
		this->setArcRect(newRect);
	}
	else if (group->getName() == "Arc" && _property->getPropertyName() == "Start Angle") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setStartAngle(actualProperty->getValue() * 16.);
	}
	else if (group->getName() == "Arc" && _property->getPropertyName() == "Span Angle") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setSpanAngle(actualProperty->getValue() * 16.);
	}
	else if (group->getName() == "Arc" && _property->getPropertyName() == "Line Painter") {
		const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		ot::PenFCfg lineStyle(this->getLineStyle());
		lineStyle.setPainter(actualProperty->getPainter()->createCopy());
		this->setLineStyle(lineStyle);
	}
	else if (group->getName() == "Arc" && _property->getPropertyName() == "Line Width") {
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

void WrappedArcItem::propertyDeleteRequested(const ot::Property* _property) {
	if (this->basePropertyDeleteRequested(_property)) return;

}

QVariant WrappedArcItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsArcItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}

