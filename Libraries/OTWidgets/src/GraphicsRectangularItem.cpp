//! @file GraphicsRectangularItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsItemFactory.h"
#include "OTWidgets/GraphicsRectangularItem.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsRectangularItem> rectItemRegistrar(OT_FactoryKey_GraphicsRectangularItem);

ot::GraphicsRectangularItem::GraphicsRectangularItem() 
	: ot::CustomGraphicsItem(false), m_size(10, 10), m_cornerRadius(0)
{

}

ot::GraphicsRectangularItem::~GraphicsRectangularItem() {
	
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItems

bool ot::GraphicsRectangularItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	ot::GraphicsRectangularItemCfg* cfg = dynamic_cast<ot::GraphicsRectangularItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	m_size.setWidth(cfg->size().width());
	m_size.setHeight(cfg->size().height());
	m_cornerRadius = cfg->cornerRadius();
	m_brush = QtFactory::toBrush(cfg->backgroundPainter());
	m_pen = QtFactory::toPen(cfg->outline());
	
	// We call set rectangle size which will call set geometry to finalize the item
	this->setRectangleSize(m_size);

	return ot::CustomGraphicsItem::setupFromConfig(_cfg);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

void ot::GraphicsRectangularItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	QPainterPath pth;
	pth.addRoundedRect(_rect, m_cornerRadius, m_cornerRadius);
	_painter->setPen(m_pen);
	_painter->fillPath(pth, m_brush);
	_painter->drawPath(pth);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::GraphicsRectangularItem::setRectangleSize(const QSizeF& _size) {
	// Avoid resizing if the size did not change
	if (m_size == _size) return;
	m_size = _size;
	this->setGeometry(QRectF(this->pos(), m_size).toRect());
	this->raiseEvent(GraphicsItem::ItemResized);
}
