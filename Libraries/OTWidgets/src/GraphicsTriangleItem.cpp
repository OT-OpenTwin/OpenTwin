//! @file GraphicsTriangleItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/KeyMap.h"
#include "OTCore/Logger.h"
#include "OTWidgets/GraphicsTriangleItem.h"
#include "OTWidgets/Painter2DFactory.h"
#include "OTWidgets/OTQtConverter.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtGui/qpainterpath.h>

static ot::SimpleFactoryRegistrar<ot::GraphicsTriangleItem> triaItem(OT_SimpleFactoryJsonKeyValue_GraphicsTriangleItem);
static ot::GlobalKeyMapRegistrar triaItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsTriangleItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsTriangleItem);

ot::GraphicsTriangleItem::GraphicsTriangleItem()
	: ot::CustomGraphicsItem(false), m_size(10, 10), m_direction(ot::GraphicsTriangleItemCfg::Right)
{

}

ot::GraphicsTriangleItem::~GraphicsTriangleItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItems

bool ot::GraphicsTriangleItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	ot::GraphicsTriangleItemCfg* cfg = dynamic_cast<ot::GraphicsTriangleItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	m_size.setWidth(cfg->size().width());
	m_size.setHeight(cfg->size().height());
	m_direction = cfg->triangleDirection();
	m_brush = ot::Painter2DFactory::brushFromPainter2D(cfg->backgroundPainter());
	m_pen.setWidth(cfg->outline().top()); // ToDo: Add seperate borders on all 4 sides
	m_pen.setBrush(QBrush(ot::OTQtConverter::toQt(cfg->outline().color())));
	m_pen.setColor(ot::OTQtConverter::toQt(cfg->outline().color()));

	// We call set rectangle size which will call set geometry to finalize the item
	this->setTriangleSize(m_size);

	return ot::CustomGraphicsItem::setupFromConfig(_cfg);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

void ot::GraphicsTriangleItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	this->paintGeneralGraphics(_painter, _opt, _widget);
	
	_painter->setBrush(m_brush);
	_painter->setPen(m_pen);

	// Set path depending on the direction
	QPainterPath path;

	switch (m_direction)
	{
	case ot::GraphicsTriangleItemCfg::Left:
		path.moveTo(QPointF(_rect.topLeft().x(), _rect.center().y()));
		path.lineTo(_rect.topRight());
		path.lineTo(_rect.bottomRight());
		path.lineTo(QPointF(_rect.topLeft().x(), _rect.center().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Up:
		path.moveTo(QPointF(_rect.center().x(), _rect.topLeft().y()));
		path.lineTo(_rect.bottomRight());
		path.lineTo(_rect.bottomLeft());
		path.lineTo(QPointF(_rect.center().x(), _rect.topLeft().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Right:
		path.moveTo(QPointF(_rect.bottomRight().x(), _rect.center().y()));
		path.lineTo(_rect.bottomLeft());
		path.lineTo(_rect.topLeft());
		path.lineTo(QPointF(_rect.bottomRight().x(), _rect.center().y()));
		break;
	case ot::GraphicsTriangleItemCfg::Down:
		path.moveTo(QPointF(_rect.center().x(), _rect.bottomRight().y()));
		path.lineTo(_rect.topLeft());
		path.lineTo(_rect.topRight());
		path.lineTo(QPointF(_rect.center().x(), _rect.bottomRight().y()));
		break;
	default:
		OT_LOG_EA("Unknown triangle direction");
		break;
	}

	_painter->fillPath(path, m_brush);
	_painter->drawPath(path);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::GraphicsTriangleItem::setTriangleSize(const QSizeF& _size) {
	// Avoid resizing if the size did not change
	if (m_size == _size) return;
	m_size = _size;
	this->setGeometry(QRectF(this->pos(), m_size).toRect());
	this->raiseEvent(GraphicsItem::ItemResized);
}

