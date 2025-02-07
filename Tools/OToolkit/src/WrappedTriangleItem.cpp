//! @file WrappedTriangleItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedTriangleItem.h"
#include "WrappedItemFactory.h"
#include "GraphicsItemDesignerNavigation.h"

// OpenTwin header
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsTriangleItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsScene.h"

static WrappedItemFactoryRegistrar<WrappedTriangleItem> circleRegistrar(OT_FactoryKey_GraphicsTriangleItem);

WrappedTriangleItem::WrappedTriangleItem() {
	this->setOutline(ot::PenFCfg(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)));
	this->setBackgroundPainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBackground));
}

WrappedTriangleItem::~WrappedTriangleItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Public base class methods

ot::TreeWidgetItemInfo WrappedTriangleItem::createNavigationInformation(void) {
	ot::TreeWidgetItemInfo info;
	info.setText(QString::fromStdString(this->getGraphicsItemName()));
	info.setIcon(ot::IconManager::getIcon("GraphicsEditor/Triangle.png"));

	return info;
}

void WrappedTriangleItem::makeItemTransparent(void) {
	this->setOutline(ot::PenFCfg(0., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent)));
	this->setBackgroundPainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent));
}

void WrappedTriangleItem::setupDesignerItemFromConfig(const ot::GraphicsItemCfg* _config) {
	if (!this->setupFromConfig(_config)) return;

	QRectF newRect(this->pos(), ot::QtFactory::toQSize(this->getTriangleSize()));

	QList<QPointF> newControlPoints;
	newControlPoints.append(newRect.topLeft());
	newControlPoints.append(newRect.bottomRight());

	this->initializeBaseData(newControlPoints, newRect.topLeft());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected base class methods

void WrappedTriangleItem::controlPointsChanged(void) {
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
	this->setGraphicsItemPos(topLeft);
	this->setTriangleSize(newSize);
}

void WrappedTriangleItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	this->fillBasePropertyGrid(cfg);

	PropertyGroup* triangleGroup = new PropertyGroup("Triangle");
	triangleGroup->addProperty(new PropertyDouble("Width", this->getTriangleSize().width(), 0., std::numeric_limits<double>::max()));
	triangleGroup->addProperty(new PropertyDouble("Height", this->getTriangleSize().height(), 0., std::numeric_limits<double>::max()));
	{
		std::list<std::string> possibleSelection;
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleShapeToString(GraphicsTriangleItemCfg::Triangle));
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleShapeToString(GraphicsTriangleItemCfg::Kite));
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleShapeToString(GraphicsTriangleItemCfg::IceCone));

		triangleGroup->addProperty(new PropertyStringList("Shape", GraphicsTriangleItemCfg::triangleShapeToString(this->getTriangleShape()), possibleSelection));
	}
	{
		std::list<std::string> possibleSelection;
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleDirectionToString(GraphicsTriangleItemCfg::Up));
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleDirectionToString(GraphicsTriangleItemCfg::Left));
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleDirectionToString(GraphicsTriangleItemCfg::Right));
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleDirectionToString(GraphicsTriangleItemCfg::Down));

		triangleGroup->addProperty(new PropertyStringList("Direction", GraphicsTriangleItemCfg::triangleDirectionToString(this->getTrianlgeDirection()), possibleSelection));
	}

	triangleGroup->addProperty(new PropertyPainter2D("Border Painter", this->getOutline().painter()));
	triangleGroup->addProperty(new PropertyDouble("Border Width", this->getOutline().width(), 0., std::numeric_limits<double>::max()));
	triangleGroup->addProperty(new PropertyPainter2D("Background Painter", this->getBackgroundPainter()));

	cfg.addRootGroup(triangleGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedTriangleItem::propertyChanged(const ot::Property* _property) {
	using namespace ot;

	if (this->basePropertyChanged(_property)) return;

	const ot::PropertyGroup* group = _property->getParentGroup();
	if (!group) {
		OT_LOG_EA("Data mismatch");
		return;
	}

	if (group->getName() == "Triangle" && _property->getPropertyName() == "Width") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setTriangleSize(QSizeF(actualProperty->getValue(), this->getTriangleSize().height()));
	}
	else if (group->getName() == "Triangle" && _property->getPropertyName() == "Height") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setTriangleSize(QSizeF(this->getTriangleSize().width(), actualProperty->getValue()));
	}
	else if (group->getName() == "Triangle" && _property->getPropertyName() == "Shape") {
		const PropertyStringList* actualProperty = dynamic_cast<const PropertyStringList*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setTriangleShape(GraphicsTriangleItemCfg::stringToTriangleShape(actualProperty->getCurrent()));
	}
	else if (group->getName() == "Triangle" && _property->getPropertyName() == "Direction") {
		const PropertyStringList* actualProperty = dynamic_cast<const PropertyStringList*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setTriangleDirection(GraphicsTriangleItemCfg::stringToTriangleDirection(actualProperty->getCurrent()));
	}
	else if (group->getName() == "Triangle" && _property->getPropertyName() == "Border Painter") {
		const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		ot::PenFCfg lineStyle = this->getOutline();
		lineStyle.setPainter(actualProperty->getPainter()->createCopy());
		this->setOutline(lineStyle);
	}
	else if (group->getName() == "Triangle" && _property->getPropertyName() == "Border Width") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		ot::PenFCfg lineStyle = this->getOutline();
		lineStyle.setWidth(actualProperty->getValue());
		this->setOutline(lineStyle);
	}
	else if (group->getName() == "Triangle" && _property->getPropertyName() == "Background Painter") {
		const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setBackgroundPainter(actualProperty->getPainter()->createCopy());
	}
	else {
		OT_LOG_E("Unknown property { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
	}
}

void WrappedTriangleItem::propertyDeleteRequested(const ot::Property* _property) {
	if (this->basePropertyDeleteRequested(_property)) return;

}

QVariant WrappedTriangleItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsTriangleItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}