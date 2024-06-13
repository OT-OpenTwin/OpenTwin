//! @file WrappedSquareItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedSquareItem.h"
#include "WrappedItemFactory.h"
#include "GraphicsItemDesignerNavigation.h"

// OpenTwin header
#include "OTGui/StyleRefPainter2D.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsScene.h"

WrappedSquareItem::WrappedSquareItem() {
	this->setOutline(ot::OutlineF(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)));
	this->setBackgroundPainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBackground));
}

WrappedSquareItem::~WrappedSquareItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Public base class methods

ot::TreeWidgetItemInfo WrappedSquareItem::createNavigationInformation(void) {
	ot::TreeWidgetItemInfo info;
	info.setText(QString::fromStdString(this->getGraphicsItemName()));
	info.setIcon(ot::IconManager::getIcon("GraphicsEditor/Square.png"));

	return info;
}

void WrappedSquareItem::makeItemTransparent(void) {
	this->setOutline(ot::OutlineF(0., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent)));
	this->setBackgroundPainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent));
}

void WrappedSquareItem::setupDesignerItemFromConfig(const ot::GraphicsItemCfg* _config) {
	OT_LOG_E("Item can not be setup from config");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected base class methods

void WrappedSquareItem::controlPointsChanged(void) {
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
	newSize.setWidth(std::min(newSize.width(), newSize.height()));
	newSize.setHeight(newSize.width());

	this->setPos(topLeft);
	this->setRectangleSize(newSize);
}

void WrappedSquareItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	PropertyGroup* generalGroup = new PropertyGroup("General");
	generalGroup->addProperty(new PropertyString("Name", this->getGraphicsItem()->getGraphicsItemName()));
	generalGroup->addProperty(new PropertyBool("Connectable", this->getGraphicsItemFlags() & GraphicsItemCfg::ItemIsConnectable));

	PropertyGroup* geometryGroup = new PropertyGroup("Geometry");
	geometryGroup->addProperty(new PropertyDouble("X", this->pos().x()));
	geometryGroup->addProperty(new PropertyDouble("Y", this->pos().y()));
	geometryGroup->addProperty(new PropertyDouble("Width", this->getRectangleSize().width()));
	geometryGroup->addProperty(new PropertyPainter2D("Border Painter", this->getOutline().painter()));
	geometryGroup->addProperty(new PropertyDouble("Border Width", this->getOutline().width()));
	geometryGroup->addProperty(new PropertyPainter2D("Background Painter", this->getBackgroundPainter()));
	geometryGroup->addProperty(new PropertyInt("Corner Radius", this->getCornerRadius(), 0, 9999));
	{
		PropertyBool* newStateProperty = new PropertyBool("Handle State", this->getGraphicsItemFlags() & GraphicsItemCfg::ItemHandlesState);
		newStateProperty->setPropertyTip("If enabled the item will update its appearance according to the current item state (e.g. ItemSelected or ItemHover)");
		geometryGroup->addProperty(newStateProperty);
	}


	cfg.addRootGroup(generalGroup);
	cfg.addRootGroup(geometryGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedSquareItem::propertyChanged(const ot::Property* _property) {
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
		this->setGeometry(QRectF(this->pos(), ot::QtFactory::toQSize(this->getRectangleSize())));
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Y") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->prepareGeometryChange();
		this->setPos(this->x(), actualProperty->getValue());
		this->setGeometry(QRectF(this->pos(), ot::QtFactory::toQSize(this->getRectangleSize())));
	}
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Width") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->setRectangleSize(QSizeF(actualProperty->getValue(), actualProperty->getValue()));
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
	else if (group->getName() == "Geometry" && _property->getPropertyName() == "Corner Radius") {
		const PropertyInt* actualProperty = dynamic_cast<const PropertyInt*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Group\": \"" + group->getName() + "\", \"");
			return;
		}

		this->setCornerRadius(actualProperty->getValue());
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

void WrappedSquareItem::propertyDeleteRequested(const ot::Property* _property) {

}

QVariant WrappedSquareItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsRectangularItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}