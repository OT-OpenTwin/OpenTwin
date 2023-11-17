//! @file GraphicsRectangularItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/KeyMap.h"
#include "OpenTwinCore/Logger.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTWidgets/GraphicsRectangularItem.h"
#include "OTWidgets/Painter2DFactory.h"
#include "OTWidgets/OTQtConverter.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

static ot::SimpleFactoryRegistrar<ot::GraphicsRectangularItem> rectItem(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem);
static ot::GlobalKeyMapRegistrar rectItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItem);

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
	m_brush = ot::Painter2DFactory::brushFromPainter2D(cfg->backgroundPainter());
	m_pen.setWidth(cfg->border().top()); // ToDo: Add seperate borders on all 4 sides
	m_pen.setBrush(QBrush(ot::OTQtConverter::toQt(cfg->border().color())));
	m_pen.setColor(ot::OTQtConverter::toQt(cfg->border().color()));

	// We call set rectangle size which will call set geometry to finalize the item
	this->setRectangleSize(m_size);

	return ot::CustomGraphicsItem::setupFromConfig(_cfg);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

void ot::GraphicsRectangularItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	_painter->setBrush(m_brush);
	_painter->setPen(m_pen);
	_painter->drawRoundedRect(_rect, m_cornerRadius, m_cornerRadius);
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
