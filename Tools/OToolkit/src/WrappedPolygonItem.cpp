// @otlicense

// OToolkit header
#include "WrappedPolygonItem.h"
#include "WrappedItemFactory.h"
#include "GraphicsItemDesignerNavigation.h"

// OpenTwin header
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/GraphicsPolygonItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsScene.h"

static WrappedItemFactoryRegistrar<WrappedPolygonItem> circleRegistrar(ot::GraphicsPolygonItemCfg::className());

WrappedPolygonItem::WrappedPolygonItem() {
	this->setOutline(ot::PenFCfg(1., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)));
	this->setBackgroundPainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBackground));
}

WrappedPolygonItem::~WrappedPolygonItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Public base class methods

ot::TreeWidgetItemInfo WrappedPolygonItem::createNavigationInformation(void) {
	ot::TreeWidgetItemInfo info;
	info.setText(QString::fromStdString(this->getGraphicsItemName()));
	info.setIcon(ot::IconManager::getIcon("GraphicsEditor/Polygon.png"));

	return info;
}

void WrappedPolygonItem::makeItemTransparent(void) {
	this->setOutline(ot::PenFCfg(0., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent)));
	this->setBackgroundPainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::Transparent));
}

void WrappedPolygonItem::setupDesignerItemFromConfig(const ot::GraphicsItemCfg* _config) {
	if (!this->setupFromConfig(_config)) return;

	QList<QPointF> newControlPoints;

	for (const ot::Point2DD& pt : this->getPoints()) {
		newControlPoints.append(ot::QtFactory::toQPoint(pt) + this->pos());
	}

	this->initializeBaseData(newControlPoints, this->pos());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected base class methods

void WrappedPolygonItem::controlPointsChanged(void) {
	if (this->getControlPoints().size() < 2) return;

	ot::GraphicsScene* gscene = this->getGraphicsScene();
	if (!gscene) {
		OT_LOG_E("Graphics scene not set");
		return;
	}

	this->prepareGeometryChange();

	ot::Point2DD topLeftPoint(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	std::list<ot::Point2DD> newPoints;

	for (const QPointF& pt : this->getControlPoints()) {
		if (pt.x() < topLeftPoint.x()) topLeftPoint.setX(pt.x());
		if (pt.y() < topLeftPoint.y()) topLeftPoint.setY(pt.y());
		newPoints.push_back(ot::QtFactory::toPoint2D(pt));
	}

	if (topLeftPoint.x() == std::numeric_limits<double>::max() || topLeftPoint.y() == std::numeric_limits<double>::max()) {
		OT_LOG_E("Invalid points");
		return;
	}

	for (ot::Point2DD& pt : newPoints) {
		pt -= topLeftPoint;
	}

	this->setGraphicsItemPos(topLeftPoint);
	this->setPoints(newPoints);
}

void WrappedPolygonItem::fillPropertyGrid(void) {
	using namespace ot;

	PropertyGridCfg cfg;
	this->fillBasePropertyGrid(cfg);

	PropertyGroup* polygonGroup = new PropertyGroup("Polygon");
	polygonGroup->addProperty(new PropertyPainter2D("Border Painter", this->getOutline().getPainter()));
	polygonGroup->addProperty(new PropertyDouble("Border Width", this->getOutline().getWidth(), 0., std::numeric_limits<double>::max()));
	polygonGroup->addProperty(new PropertyPainter2D("Background Painter", this->getBackgroundPainter()));
	polygonGroup->addProperty(new PropertyBool("Fill Polygon", this->getFillPolygon()));

	cfg.addRootGroup(polygonGroup);
	this->getPropertyGrid()->setupGridFromConfig(cfg);
}

void WrappedPolygonItem::propertyChanged(const ot::Property* _property) {
	using namespace ot;

	if (this->basePropertyChanged(_property)) return;

	const ot::PropertyGroup* group = _property->getParentGroup();
	if (!group) {
		OT_LOG_EA("Data mismatch");
		return;
	}

	if (group->getName() == "Polygon" && _property->getPropertyName() == "Border Painter") {
		const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		ot::PenFCfg lineStyle(this->getOutline());
		lineStyle.setPainter(actualProperty->getPainter()->createCopy());
		this->setOutline(lineStyle);
	}
	else if (group->getName() == "Polygon" && _property->getPropertyName() == "Border Width") {
		const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		ot::PenFCfg lineStyle(this->getOutline());
		lineStyle.setWidth(actualProperty->getValue());
		this->setOutline(lineStyle);
	}
	else if (group->getName() == "Polygon" && _property->getPropertyName() == "Background Painter") {
		const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setBackgroundPainter(actualProperty->getPainter()->createCopy());
	}
	else if (group->getName() == "Polygon" && _property->getPropertyName() == "Fill Polygon") {
		const PropertyBool* actualProperty = dynamic_cast<const PropertyBool*>(_property);
		if (!actualProperty) {
			OT_LOG_E("Property cast failed { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
			return;
		}

		this->setFillPolygon(actualProperty->getValue());
	}
	else {
		OT_LOG_E("Unknown property { \"Property\": \"" + _property->getPropertyName() + "\", \"Group\": \"" + group->getName() + "\" }");
	}
}

void WrappedPolygonItem::propertyDeleteRequested(const ot::Property* _property) {
	if (this->basePropertyDeleteRequested(_property)) return;

}

QVariant WrappedPolygonItem::itemChange(QGraphicsItem::GraphicsItemChange _change, const QVariant& _constrains) {
	QVariant ret = ot::GraphicsPolygonItem::itemChange(_change, _constrains);

	if (_change == QGraphicsItem::ItemScenePositionHasChanged) {
		this->graphicsItemWasMoved(this->pos());
	}

	return ret;
}