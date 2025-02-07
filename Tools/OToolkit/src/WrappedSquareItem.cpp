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
	this->setOutline(ot::PenFCfg(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)));
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
	this->setOutline(ot::PenFCfg(0., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent)));
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

	this->setGraphicsItemPos(topLeft);
	this->setRectangleSize(newSize);
}

void WrappedSquareItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	this->fillBasePropertyGrid(cfg);
	
	PropertyGroup* squareGroup = new PropertyGroup("Square");
	squareGroup->addProperty(new PropertyDouble("Size", this->getRectangleSize().width(), 0., std::numeric_limits<double>::max()));
	squareGroup->addProperty(new PropertyPainter2D("Border Painter", this->getOutline().painter()));
	squareGroup->addProperty(new PropertyDouble("Border Width", this->getOutline().width(), 0., std::numeric_limits<double>::max()));
	squareGroup->addProperty(new PropertyPainter2D("Background Painter", this->getBackgroundPainter()));
	squareGroup->addProperty(new PropertyInt("Corner Radius", this->getCornerRadius(), 0, 9999));
	
	cfg.addRootGroup(squareGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedSquareItem::propertyChanged(const ot::Property* _property) {
	using namespace ot;

	if (this->basePropertyChanged(_property)) return;

	const ot::PropertyGroup* group = _property->getParentGroup();
	if (!group) {
		OT_LOG_EA("Data mismatch");
		return;
	}

	if (group->getName() == "Square" && _property->getPropertyName() == "Size") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setRectangleSize(QSizeF(actualProperty->getValue(), actualProperty->getValue()));
	}
	else if (group->getName() == "Square" && _property->getPropertyName() == "Border Painter") {
		const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		ot::PenFCfg lineStyle = this->getOutline();
		lineStyle.setPainter(actualProperty->getPainter()->createCopy());
		this->setOutline(lineStyle);
	}
	else if (group->getName() == "Square" && _property->getPropertyName() == "Border Width") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		ot::PenFCfg lineStyle = this->getOutline();
		lineStyle.setWidth(actualProperty->getValue());
		this->setOutline(lineStyle);
	}
	else if (group->getName() == "Square" && _property->getPropertyName() == "Background Painter") {
		const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setBackgroundPainter(actualProperty->getPainter()->createCopy());
	}
	else if (group->getName() == "Square" && _property->getPropertyName() == "Corner Radius") {
		const PropertyInt* actualProperty = dynamic_cast<const PropertyInt*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setCornerRadius(actualProperty->getValue());
	}
	else {
		OT_LOG_E("Unknown property { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
	}
}

void WrappedSquareItem::propertyDeleteRequested(const ot::Property* _property) {
	if (this->basePropertyDeleteRequested(_property)) return;

}

QVariant WrappedSquareItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsRectangularItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}