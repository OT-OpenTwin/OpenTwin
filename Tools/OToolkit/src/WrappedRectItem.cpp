//! @file WrappedRectItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedRectItem.h"
#include "WrappedItemFactory.h"
#include "GraphicsItemDesignerNavigation.h"

// OpenTwin header
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsScene.h"

static WrappedItemFactoryRegistrar<WrappedRectItem> circleRegistrar(ot::GraphicsRectangularItemCfg::className());

WrappedRectItem::WrappedRectItem() {
	this->setOutline(ot::PenFCfg(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)));
	this->setBackgroundPainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBackground));
}

WrappedRectItem::~WrappedRectItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Public base class methods

ot::TreeWidgetItemInfo WrappedRectItem::createNavigationInformation(void) {
	ot::TreeWidgetItemInfo info;
	info.setText(QString::fromStdString(this->getGraphicsItemName()));
	info.setIcon(ot::IconManager::getIcon("GraphicsEditor/Rect.png"));

	return info;
}

void WrappedRectItem::makeItemTransparent(void) {
	this->setOutline(ot::PenFCfg(0., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent)));
	this->setBackgroundPainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent));
}

void WrappedRectItem::setupDesignerItemFromConfig(const ot::GraphicsItemCfg* _config) {
	if (!this->setupFromConfig(_config)) return;

	QRectF newRect(this->pos(), ot::QtFactory::toQSize(this->getRectangleSize()));

	QList<QPointF> newControlPoints;
	newControlPoints.append(newRect.topLeft());
	newControlPoints.append(newRect.bottomRight());

	this->initializeBaseData(newControlPoints, newRect.topLeft());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected base class methods

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
	this->setGraphicsItemPos(topLeft);
	this->setRectangleSize(newSize);
}

void WrappedRectItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	this->fillBasePropertyGrid(cfg);
	
	PropertyGroup* rectangleGroup = new PropertyGroup("Rectangle");
	rectangleGroup->addProperty(new PropertyDouble("Width", this->getRectangleSize().width(), 0., std::numeric_limits<double>::max()));
	rectangleGroup->addProperty(new PropertyDouble("Height", this->getRectangleSize().height(), 0., std::numeric_limits<double>::max()));
	rectangleGroup->addProperty(new PropertyPainter2D("Border Painter", this->getOutline().getPainter()));
	rectangleGroup->addProperty(new PropertyDouble("Border Width", this->getOutline().getWidth(), 0., std::numeric_limits<double>::max()));
	rectangleGroup->addProperty(new PropertyPainter2D("Background Painter", this->getBackgroundPainter()));
	rectangleGroup->addProperty(new PropertyInt("Corner Radius", this->getCornerRadius(), 0, 9999));

	cfg.addRootGroup(rectangleGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedRectItem::propertyChanged(const ot::Property* _property) {
	using namespace ot;

	if (this->basePropertyChanged(_property)) return;

	const ot::PropertyGroup* group = _property->getParentGroup();
	if (!group) {
		OT_LOG_EA("Data mismatch");
		return;
	}

	if (group->getName() == "Rectangle" && _property->getPropertyName() == "Width") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setRectangleSize(QSizeF(actualProperty->getValue(), this->getRectangleSize().height()));
	}
	else if (group->getName() == "Rectangle" && _property->getPropertyName() == "Height") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setRectangleSize(QSizeF(this->getRectangleSize().width(), actualProperty->getValue()));
	}
	else if (group->getName() == "Rectangle" && _property->getPropertyName() == "Border Painter") {
		const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		ot::PenFCfg lineStyle(this->getOutline());
		lineStyle.setPainter(actualProperty->getPainter()->createCopy());
		this->setOutline(lineStyle);
	}
	else if (group->getName() == "Rectangle" && _property->getPropertyName() == "Border Width") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		ot::PenFCfg lineStyle(this->getOutline());
		lineStyle.setWidth(actualProperty->getValue());
		this->setOutline(lineStyle);
	}
	else if (group->getName() == "Rectangle" && _property->getPropertyName() == "Background Painter") {
		const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setBackgroundPainter(actualProperty->getPainter()->createCopy());
	}
	else if (group->getName() == "Rectangle" && _property->getPropertyName() == "Corner Radius") {
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

void WrappedRectItem::propertyDeleteRequested(const ot::Property* _property) {
	if (this->basePropertyDeleteRequested(_property)) return;

}

QVariant WrappedRectItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsRectangularItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}