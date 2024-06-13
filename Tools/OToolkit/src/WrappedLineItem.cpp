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

static WrappedItemFactoryRegistrar<WrappedLineItem> circleRegistrar(OT_FactoryKey_GraphicsLineItem);

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
	{
		PropertyBool* newStateProperty = new PropertyBool("Handle State", this->getGraphicsItemFlags() & GraphicsItemCfg::ItemHandlesState);
		newStateProperty->setPropertyTip("If enabled the item will update its appearance according to the current item state (e.g. ItemSelected or ItemHover)");
		geometryGroup->addProperty(newStateProperty);
	}

	cfg.addRootGroup(generalGroup);
	cfg.addRootGroup(geometryGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedLineItem::propertyChanged(const ot::Property* _property) {
	using namespace ot;

	const ot::PropertyGroup* group = _property->getParentGroup();
	if (!group) {
		OT_LOG_EA("Data mismatch");
		return;
	}

	if (group->getName() == "General" && _property->getPropertyName() == "Name") {
		const PropertyString* actualProperty = dynamic_cast<const PropertyString*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		OTAssertNullptr(this->getNavigation());
		if (actualProperty->getValue().empty()) return;
		if (!this->getNavigation()->updateItemName(QString::fromStdString(this->getGraphicsItemName()), QString::fromStdString(actualProperty->getValue()))) return;

		this->setGraphicsItemName(actualProperty->getValue());
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "X1") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->setLine(actualProperty->getValue(), this->getLine().y1(), this->getLine().x2(), this->getLine().y2());
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Y1") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->setLine(this->getLine().x1(), actualProperty->getValue(), this->getLine().x2(), this->getLine().y2());
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "X2") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->setLine(this->getLine().x1(), this->getLine().y1(), actualProperty->getValue(), this->getLine().y2());
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Y2") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->setLine(this->getLine().x1(), this->getLine().y1(), this->getLine().x2(), actualProperty->getValue());
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Line Painter") {
		const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		ot::OutlineF lineStyle = this->getLineStyle();
		lineStyle.setPainter(actualProperty->getPainter()->createCopy());
		this->setLineStyle(lineStyle);
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Line Width") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		ot::OutlineF lineStyle = this->getLineStyle();
		lineStyle.setWidth(actualProperty->getValue());
		this->setLineStyle(lineStyle);
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Handle State") {
		const PropertyBool* actualProperty = dynamic_cast<const PropertyBool*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->setGraphicsItemFlag(ot::GraphicsItemCfg::ItemHandlesState, actualProperty->getValue());
	}

}

void WrappedLineItem::propertyDeleteRequested(const ot::Property* _property) {

}

QVariant WrappedLineItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsLineItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}

