//! @file GraphicsPolygonItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
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
	: ot::CustomGraphicsItem(false)
{

}

ot::GraphicsPolygonItem::~GraphicsPolygonItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItems

bool ot::GraphicsPolygonItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	ot::GraphicsPolygonItemCfg* cfg = dynamic_cast<ot::GraphicsPolygonItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	// We call set rectangle size which will call set geometry to finalize the item
	m_polygon.clear();
	QList<QPointF> pts;
	for (const Point2DD& pt : cfg->points()) {
		pts.append(QtFactory::toPoint(pt));
	}
	m_polygon.append(pts);

	return ot::CustomGraphicsItem::setupFromConfig(_cfg);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

QSizeF ot::GraphicsPolygonItem::getPreferredGraphicsItemSize(void) const {
	return QSizeF();
}

void ot::GraphicsPolygonItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	_painter->setBrush(m_brush);
	_painter->setPen(m_pen);

	//! @todo Check for fill polygon
	_painter->drawPolygon(m_polygon);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::GraphicsPolygonItem::setPolygon(const QList<QPointF>& _pts) {
	m_polygon.clear();
	m_polygon.append(_pts);
}
