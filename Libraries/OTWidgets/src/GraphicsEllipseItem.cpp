//! @file GraphicsEllipseItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsEllipseItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsEllipseItem> elliItemRegistrar(OT_FactoryKey_GraphicsEllipseItem);

ot::GraphicsEllipseItem::GraphicsEllipseItem()
	: ot::CustomGraphicsItem(new GraphicsEllipseItemCfg), m_radiusX(5.), m_radiusY(5.)
{}

ot::GraphicsEllipseItem::~GraphicsEllipseItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItems

bool ot::GraphicsEllipseItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	const GraphicsEllipseItemCfg* cfg = dynamic_cast<const GraphicsEllipseItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	m_radiusX = cfg->getRadiusX();
	m_radiusY = cfg->getRadiusY();
	m_brush = QtFactory::toBrush(cfg->getBackgroundPainter());
	m_pen = QtFactory::toPen(cfg->getOutline());

	return ot::CustomGraphicsItem::setupFromConfig(_cfg);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

QSizeF ot::GraphicsEllipseItem::getPreferredGraphicsItemSize(void) const {
	return QSizeF(m_radiusX * 2., m_radiusY * 2.);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

void ot::GraphicsEllipseItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	_painter->setBrush(m_brush);
	_painter->setPen(m_pen);
	_painter->drawEllipse(_rect.center(), _rect.width() / 2., _rect.height() / 2.);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::GraphicsEllipseItem::setRadius(double _x, double _y) {
	// Avoid resizing if the size did not change
	if (_x == m_radiusX && _y == m_radiusY) { return; }
	this->prepareGeometryChange();
	m_radiusX = _x;
	m_radiusY = _y;
	this->setGeometry(QRectF(this->pos(), QSizeF(m_radiusX * 2., m_radiusY * 2.)));
	this->raiseEvent(GraphicsItem::ItemResized);
}
