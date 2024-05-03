//! @file GraphicsPolygonItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/KeyMap.h"
#include "OTCore/Logger.h"
#include "OTGui/GraphicsPolygonItemCfg.h"
#include "OTWidgets/GraphicsPolygonItem.h"
#include "OTWidgets/Painter2DFactory.h"
#include "OTWidgets/OTQtConverter.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

static ot::SimpleFactoryRegistrar<ot::GraphicsPolygonItem> rectItem(OT_SimpleFactoryJsonKeyValue_GraphicsPolygonItem);
static ot::GlobalKeyMapRegistrar rectItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsPolygonItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsPolygonItem);

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
	
	//_painter->drawRoundedRect(_rect, m_cornerRadius, m_cornerRadius);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter
