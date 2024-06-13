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
	this->setLineStyle(ot::OutlineF(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)));
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

	this->setPos(topLeftPoint);
	QRectF newArcRect(topLeftPoint, bottomRightPoint);
	newArcRect.moveTo(0., 0.);
	this->setArcRect(newArcRect);
}

void WrappedArcItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	PropertyGroup* generalGroup = new PropertyGroup("General");
	generalGroup->addProperty(new PropertyString("Name", this->getGraphicsItem()->getGraphicsItemName()));

	PropertyGroup* geometryGroup = new PropertyGroup("Geometry");
	geometryGroup->addProperty(new PropertyDouble("X", this->getArcRect().getTopLeft().x()));
	geometryGroup->addProperty(new PropertyDouble("Y", this->getArcRect().getTopLeft().y()));
	geometryGroup->addProperty(new PropertyDouble("Width", this->getArcRect().getWidth()));
	geometryGroup->addProperty(new PropertyDouble("Height", this->getArcRect().getHeight()));
	geometryGroup->addProperty(new PropertyDouble("Start Angle", this->getStartAngle() / 16.));
	geometryGroup->addProperty(new PropertyDouble("Span Angle", this->getSpanAngle() / 16.));
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

void WrappedArcItem::propertyChanged(const ot::Property* _property) {
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
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "X") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		RectD newRect = this->getArcRect();
		newRect.moveBy(actualProperty->getValue() - newRect.getLeft(), 0.);
		this->setArcRect(newRect);
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Y") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		RectD newRect = this->getArcRect();
		newRect.moveBy(0., actualProperty->getValue() - newRect.getLeft());
		this->setArcRect(newRect);
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Width") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		RectD newRect = this->getArcRect();
		this->setArcRect(RectD(newRect.getTopLeft(), Size2DD(actualProperty->getValue(), newRect.getHeight())));
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Height") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		RectD newRect = this->getArcRect();
		this->setArcRect(RectD(newRect.getTopLeft(), Size2DD(newRect.getHeight(), actualProperty->getValue())));
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Start Angle") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->setStartAngle(actualProperty->getValue() * 16.);
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Span Angle") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->setSpanAngle(actualProperty->getValue() * 16.);
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

void WrappedArcItem::propertyDeleteRequested(const ot::Property* _property) {

}

QVariant WrappedArcItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsArcItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}

