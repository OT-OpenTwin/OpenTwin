//! @file GraphicsPixmapItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GraphicsPixmapItem.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsPixmapItem> pixmItemRegistrar(OT_FactoryKey_GraphicsImageItem);

ot::GraphicsPixmapItem::GraphicsPixmapItem()
	: ot::CustomGraphicsItem(false), m_maintainAspectRatio(false), m_colorMask(-1.f, -1.f, -1.f, -1.f)
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
		m_colorMask = cfg->colorMask();
		m_maintainAspectRatio = cfg->isMaintainAspectRatio();
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
	return ot::CustomGraphicsItem::setupFromConfig(_cfg);
}

QSizeF ot::GraphicsPixmapItem::getPreferredGraphicsItemSize(void) const {
	return m_pixmap.size();
}

void ot::GraphicsPixmapItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	if (m_maintainAspectRatio) {
		QPixmap scaled = m_pixmap.scaled(_rect.size().toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QRectF adjustedRect = this->calculateInnerRect(_rect, scaled.size(), this->graphicsItemAlignment());

		// Check if a color mask is set
		if (m_colorMask.isValid()) {
			QPixmap mask(scaled.size());
			mask.fill(QtFactory::toColor(m_colorMask));
			_painter->setCompositionMode(QPainter::CompositionMode_SourceIn);

			_painter->drawPixmap(adjustedRect.topLeft(), scaled);
			_painter->drawPixmap(adjustedRect.topLeft(), mask);
		}
		else {
			_painter->drawPixmap(adjustedRect.topLeft(), scaled);
		}
	}
	else {

		// Check if a color mask is set
		if (m_colorMask.isValid()) {
			QPixmap mask(_rect.size().toSize());
			mask.fill(QtFactory::toColor(m_colorMask));
			_painter->setCompositionMode(QPainter::CompositionMode_SourceIn);

			_painter->drawPixmap(_rect.topLeft().x(), _rect.topLeft().y(), _rect.width(), _rect.height(), m_pixmap);
			_painter->drawPixmap(_rect.topLeft().x(), _rect.topLeft().y(), _rect.width(), _rect.height(), mask);
		}
		else {
			_painter->drawPixmap(_rect.topLeft().x(), _rect.topLeft().y(), _rect.width(), _rect.height(), m_pixmap);
		}
	}
}
