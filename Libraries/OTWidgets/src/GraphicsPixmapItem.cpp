//! @file GraphicsPixmapItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OpenTwinCore/KeyMap.h"
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GraphicsPixmapItem.h"

// Qt header
#include <QtGui/qpainter.h>

static ot::SimpleFactoryRegistrar<ot::GraphicsPixmapItem> pixmapItem(OT_SimpleFactoryJsonKeyValue_GraphicsPixmapItem);
static ot::GlobalKeyMapRegistrar pixmapItemKey(OT_SimpleFactoryJsonKeyValue_GraphicsImageItemCfg, OT_SimpleFactoryJsonKeyValue_GraphicsPixmapItem);

ot::GraphicsPixmapItem::GraphicsPixmapItem()
	: ot::CustomGraphicsItem(false)
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsPixmapItem::~GraphicsPixmapItem() {

}

bool ot::GraphicsPixmapItem::setupFromConfig(ot::GraphicsItemCfg* _cfg) {
	ot::GraphicsImageItemCfg* cfg = dynamic_cast<ot::GraphicsImageItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->prepareGeometryChange();

	try {
		m_pixmap = ot::IconManager::instance().getPixmap(QString::fromStdString(cfg->imagePath()));
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
		return false;
	}
	catch (...) {
		OT_LOG_EA("[FATAL] Unknown error");
		return false;
	}
	return ot::GraphicsItem::setupFromConfig(_cfg);
}

QSizeF ot::GraphicsPixmapItem::getPreferredGraphicsItemSize(void) const {
	return m_pixmap.size();
}

void ot::GraphicsPixmapItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	_painter->drawPixmap(_rect.topLeft().x(), _rect.topLeft().y(), _rect.width(), _rect.height(), m_pixmap);
}
