// @otlicense

// OToolkit header
#include "WrappedEllipseItem.h"
#include "WrappedItemFactory.h"
#include "GraphicsItemDesignerNavigation.h"

// OpenTwin header
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsScene.h"

static WrappedItemFactoryRegistrar<WrappedEllipseItem> circleRegistrar(ot::GraphicsEllipseItemCfg::className());

WrappedEllipseItem::WrappedEllipseItem() {
	this->setOutline(ot::PenFCfg(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)));
	this->setBackgroundPainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBackground));
}

WrappedEllipseItem::~WrappedEllipseItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Public base class methods

ot::TreeWidgetItemInfo WrappedEllipseItem::createNavigationInformation(void) {
	ot::TreeWidgetItemInfo info;
	info.setText(QString::fromStdString(this->getGraphicsItemName()));
	info.setIcon(ot::IconManager::getIcon("GraphicsEditor/Ellipse.png"));

	return info;
}

void WrappedEllipseItem::makeItemTransparent(void) {
	this->setOutline(ot::PenFCfg(0., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent)));
	this->setBackgroundPainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent));
}

void WrappedEllipseItem::setupDesignerItemFromConfig(const ot::GraphicsItemCfg* _config) {
	if (!this->setupFromConfig(_config)) return;

	QRectF newRect(this->pos(), QSizeF(this->getRadiusX() * 2., this->getRadiusY() * 2.));

	QList<QPointF> newControlPoints;
	newControlPoints.append(newRect.topLeft());
	newControlPoints.append(newRect.bottomRight());

	this->initializeBaseData(newControlPoints, newRect.topLeft());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected base class methods

void WrappedEllipseItem::controlPointsChanged(void) {
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
	this->setRadius(newSize.width() / 2., newSize.height() / 2.);
}

void WrappedEllipseItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	this->fillBasePropertyGrid(cfg);

	PropertyGroup* ellipseGroup = new PropertyGroup("Ellipse");
	ellipseGroup->addProperty(new PropertyDouble("Radius X", this->getRadiusX(), 0., std::numeric_limits<double>::max()));
	ellipseGroup->addProperty(new PropertyDouble("Radius Y", this->getRadiusY(), 0., std::numeric_limits<double>::max()));
	ellipseGroup->addProperty(new PropertyPainter2D("Border Painter", this->getOutline().getPainter()));
	ellipseGroup->addProperty(new PropertyDouble("Border Width", this->getOutline().getWidth(), 0., std::numeric_limits<double>::max()));
	ellipseGroup->addProperty(new PropertyPainter2D("Background Painter", this->getBackgroundPainter()));
	
	cfg.addRootGroup(ellipseGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedEllipseItem::propertyChanged(const ot::Property* _property) {
	using namespace ot;

	if (this->basePropertyChanged(_property)) return;

	const ot::PropertyGroup* group = _property->getParentGroup();
	if (!group) {
		OT_LOG_EA("Data mismatch");
		return;
	}

	if (group->getName() == "Ellipse" && _property->getPropertyName() == "Radius X") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setRadiusX(actualProperty->getValue());
	}
	else if (group->getName() == "Ellipse" && _property->getPropertyName() == "Radius Y") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setRadiusY(actualProperty->getValue());
	}
	else if (group->getName() == "Ellipse" && _property->getPropertyName() == "Border Painter") {
		const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		ot::PenFCfg lineStyle(this->getOutline());
		lineStyle.setPainter(actualProperty->getPainter()->createCopy());
		this->setOutline(lineStyle);
	}
	else if (group->getName() == "Ellipse" && _property->getPropertyName() == "Border Width") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		ot::PenFCfg lineStyle(this->getOutline());
		lineStyle.setWidth(actualProperty->getValue());
		this->setOutline(lineStyle);
	}
	else if (group->getName() == "Ellipse" && _property->getPropertyName() == "Background Painter") {
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

void WrappedEllipseItem::propertyDeleteRequested(const ot::Property* _property) {
	if (this->basePropertyDeleteRequested(_property)) return;

}

QVariant WrappedEllipseItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsEllipseItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}