//! @file GraphicsShapeItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsShapeItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/GraphicsShapeItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsShapeItem> polyItemRegistrar(OT_FactoryKey_GraphicsShapeItem);

ot::GraphicsShapeItem::GraphicsShapeItem()
	: ot::CustomGraphicsItem(false)
{

}

ot::GraphicsShapeItem::~GraphicsShapeItem() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::GraphicsItems

bool ot::GraphicsShapeItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	OTAssertNullptr(_cfg);
	ot::GraphicsShapeItemCfg* cfg = dynamic_cast<ot::GraphicsShapeItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	// We call set rectangle size which will call set geometry to finalize the item
	m_path = QtFactory::toPainterPath(cfg->outlinePath());

	return ot::CustomGraphicsItem::setupFromConfig(_cfg);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions: ot::CustomGraphicsItem

QSizeF ot::GraphicsShapeItem::getPreferredGraphicsItemSize(void) const {
	return QSizeF();
}

void ot::GraphicsShapeItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	_painter->setPen(m_pen);
	_painter->fillPath(m_path, m_brush);
	_painter->drawPath(m_path);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::GraphicsShapeItem::setPath(const Path2DF& _path) {
	m_path = QtFactory::toPainterPath(_path);
}
