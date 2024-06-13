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
	this->setOutline(ot::OutlineF(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)));
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
	this->setOutline(ot::OutlineF(0., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent)));
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
	this->setPos(topLeft);
	this->setTriangleSize(newSize);
}

void WrappedTriangleItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	PropertyGroup* generalGroup = new PropertyGroup("General");
	generalGroup->addProperty(new PropertyString("Name", this->getGraphicsItem()->getGraphicsItemName()));
	generalGroup->addProperty(new PropertyBool("Connectable", this->getGraphicsItemFlags() & GraphicsItemCfg::ItemIsConnectable));

	PropertyGroup* geometryGroup = new PropertyGroup("Geometry");
	geometryGroup->addProperty(new PropertyDouble("X", this->pos().x()));
	geometryGroup->addProperty(new PropertyDouble("Y", this->pos().y()));
	geometryGroup->addProperty(new PropertyDouble("Width", this->getTriangleSize().width()));
	geometryGroup->addProperty(new PropertyDouble("Height", this->getTriangleSize().height()));
	{
		std::list<std::string> possibleSelection;
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleShapeToString(GraphicsTriangleItemCfg::Triangle));
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleShapeToString(GraphicsTriangleItemCfg::Kite));
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleShapeToString(GraphicsTriangleItemCfg::IceCone));

		geometryGroup->addProperty(new PropertyStringList("Shape", GraphicsTriangleItemCfg::triangleShapeToString(this->getTriangleShape()), possibleSelection));
	}
	{
		std::list<std::string> possibleSelection;
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleDirectionToString(GraphicsTriangleItemCfg::Up));
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleDirectionToString(GraphicsTriangleItemCfg::Left));
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleDirectionToString(GraphicsTriangleItemCfg::Right));
		possibleSelection.push_back(GraphicsTriangleItemCfg::triangleDirectionToString(GraphicsTriangleItemCfg::Down));

		geometryGroup->addProperty(new PropertyStringList("Direction", GraphicsTriangleItemCfg::triangleDirectionToString(this->getTrianlgeDirection()), possibleSelection));
	}

	geometryGroup->addProperty(new PropertyPainter2D("Border Painter", this->getOutline().painter()));
	geometryGroup->addProperty(new PropertyDouble("Border Width", this->getOutline().width()));
	geometryGroup->addProperty(new PropertyPainter2D("Background Painter", this->getBackgroundPainter()));
	{
		PropertyBool* newStateProperty = new PropertyBool("Handle State", this->getGraphicsItemFlags() & GraphicsItemCfg::ItemHandlesState);
		newStateProperty->setPropertyTip("If enabled the item will update its appearance according to the current item state (e.g. ItemSelected or ItemHover)");
		geometryGroup->addProperty(newStateProperty);
	}

	cfg.addRootGroup(generalGroup);
	cfg.addRootGroup(geometryGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedTriangleItem::propertyChanged(const ot::Property* _property) {
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
	if (group->getName() == "General" && _property->getPropertyName() == "Connectable") {
		const PropertyBool* actualProperty = dynamic_cast<const PropertyBool*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->setGraphicsItemFlag(GraphicsItemCfg::ItemIsConnectable, actualProperty->getValue());
		this->update();
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "X") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->prepareGeometryChange();
		this->setPos(actualProperty->getValue(), this->y());
		this->setGeometry(QRectF(this->pos(), ot::QtFactory::toQSize(this->getTriangleSize())));
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Y") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->prepareGeometryChange();
		this->setPos(this->x(), actualProperty->getValue());
		this->setGeometry(QRectF(this->pos(), ot::QtFactory::toQSize(this->getTriangleSize())));
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Width") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->setTriangleSize(QSizeF(actualProperty->getValue(), this->getTriangleSize().height()));
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Height") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->setTriangleSize(QSizeF(this->getTriangleSize().width(), actualProperty->getValue()));
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Shape") {
		const PropertyStringList* actualProperty = dynamic_cast<const PropertyStringList*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->setTriangleShape(GraphicsTriangleItemCfg::stringToTriangleShape(actualProperty->getCurrent()));
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Direction") {
		const PropertyStringList* actualProperty = dynamic_cast<const PropertyStringList*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->setTriangleDirection(GraphicsTriangleItemCfg::stringToTriangleDirection(actualProperty->getCurrent()));
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Border Painter") {
		const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		ot::OutlineF lineStyle = this->getOutline();
		lineStyle.setPainter(actualProperty->getPainter()->createCopy());
		this->setOutline(lineStyle);
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Border Width") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		ot::OutlineF lineStyle = this->getOutline();
		lineStyle.setWidth(actualProperty->getValue());
		this->setOutline(lineStyle);
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Background Painter") {
		const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->setBackgroundPainter(actualProperty->getPainter()->createCopy());
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

void WrappedTriangleItem::propertyDeleteRequested(const ot::Property* _property) {

}

QVariant WrappedTriangleItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsTriangleItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}