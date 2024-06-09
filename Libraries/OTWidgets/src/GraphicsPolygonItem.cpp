//! @file GraphicsPolygonItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsPolygonItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsPolygonItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsPolygonItem> polyItemRegistrar(OT_FactoryKey_GraphicsPolygonItem);

ot::GraphicsPolygonItem::GraphicsPolygonItem()
	: ot::CustomGraphicsItem(new GraphicsPolygonItemCfg)
{

}

ot::GraphicsPolygonItem::~GraphicsPolygonItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItems

bool ot::GraphicsPolygonItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);

	if (!ot::CustomGraphicsItem::setupFromConfig(_cfg)) return false;

	this->updateItemGeometry();

	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

QSizeF ot::GraphicsPolygonItem::getPreferredGraphicsItemSize(void) const {
	const GraphicsPolygonItemCfg* cfg = this->getItemConfiguration<GraphicsPolygonItemCfg>();
	QPointF topLeftPoint(DBL_MAX, DBL_MAX);
	QPointF bottomRightPoint(-DBL_MAX, -DBL_MAX);

	for (const Point2DD& pt : cfg->getPoints()) {
		if (pt.x() < topLeftPoint.x()) topLeftPoint.setX(pt.x());
		if (pt.y() < topLeftPoint.y()) topLeftPoint.setY(pt.y());
		if (pt.x() > bottomRightPoint.x()) bottomRightPoint.setX(pt.x());
		if (pt.y() > bottomRightPoint.y()) bottomRightPoint.setY(pt.y());
	}

	if (topLeftPoint.x() < 0.) {
		bottomRightPoint.setX(bottomRightPoint.x() - topLeftPoint.x());
		topLeftPoint.setX(0.);
	}
	if (topLeftPoint.y() < 0.) {
		bottomRightPoint.setY(bottomRightPoint.y() - topLeftPoint.y());
		topLeftPoint.setY(0.);
	}

	return QRectF(topLeftPoint, bottomRightPoint).size();
}

void ot::GraphicsPolygonItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	const GraphicsPolygonItemCfg* cfg = this->getItemConfiguration<GraphicsPolygonItemCfg>();

	QPen borderPen = QtFactory::toQPen(cfg->getOutline());

	if (this->getGraphicsItemFlags() & GraphicsItemCfg::ItemHandlesState && !this->getBlockStateNotifications()) {
		if ((this->getGraphicsItemState() & GraphicsItemState::SelectedState) && !(this->getGraphicsItemState() & GraphicsItemState::HoverState)) {
			Painter2D* newPainter = GraphicsItem::createSelectionBorderPainter();
			borderPen.setBrush(QtFactory::toQBrush(newPainter));
			delete newPainter;
		}
		else if (this->getGraphicsItemState() & GraphicsItemState::HoverState) {
			Painter2D* newPainter = GraphicsItem::createHoverBorderPainter();
			borderPen.setBrush(QtFactory::toQBrush(newPainter));
			delete newPainter;
		}
	}

	_painter->setPen(borderPen);

	QPolygonF poly;
	for (const Point2DD& pt : cfg->getPoints()) {
		poly.append(QtFactory::toQPoint(pt));
	}

	if (cfg->getFillPolygon()) {
		QPainterPath painterPath;
		painterPath.addPolygon(poly);
		_painter->fillPath(painterPath, QtFactory::toQBrush(cfg->getBackgroundPainter()));
		_painter->drawPath(painterPath);
	}
	else {
		_painter->drawPolygon(poly);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::GraphicsPolygonItem::addPoint(const Point2DD& _pt) {
	this->getItemConfiguration<GraphicsPolygonItemCfg>()->addPoint(_pt);
	this->updateItemGeometry();
}

void ot::GraphicsPolygonItem::addPoint(const QPointF& _pt) {
	this->addPoint(QtFactory::toPoint2D(_pt));
}

void ot::GraphicsPolygonItem::setPoints(const std::list<Point2DD>& _points) {
	this->getItemConfiguration<GraphicsPolygonItemCfg>()->setPoints(_points);
	this->updateItemGeometry();
}

const std::list<ot::Point2DD>& ot::GraphicsPolygonItem::getPoints(void) const {
	return this->getItemConfiguration<GraphicsPolygonItemCfg>()->getPoints();
}

void ot::GraphicsPolygonItem::setBackgroundPainter(Painter2D* _painter) {
	this->getItemConfiguration<GraphicsPolygonItemCfg>()->setBackgroundPainter(_painter);
	this->update();
}

const ot::Painter2D* ot::GraphicsPolygonItem::getBackgroundPainter(void) const {
	return this->getItemConfiguration<GraphicsPolygonItemCfg>()->getBackgroundPainter();
}

void ot::GraphicsPolygonItem::setOutline(const OutlineF& _outline) {
	this->getItemConfiguration<GraphicsPolygonItemCfg>()->setOutline(_outline);
	this->update();
}

const ot::OutlineF& ot::GraphicsPolygonItem::getOutline(void) const {
	return this->getItemConfiguration<GraphicsPolygonItemCfg>()->getOutline();
}

void ot::GraphicsPolygonItem::setFillPolygon(bool _fill) {
	this->getItemConfiguration<GraphicsPolygonItemCfg>()->setFillPolygon(_fill);
	this->update();
}

bool ot::GraphicsPolygonItem::getFillPolygon(void) const {
	return this->getItemConfiguration<GraphicsPolygonItemCfg>()->getFillPolygon();
}
