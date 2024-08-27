//! @file GraphicsPixmapItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/Positioning.h"
#include "OTWidgets/ImagePainter.h"
#include "OTWidgets/GraphicsPixmapItem.h"
#include "OTWidgets/ImagePainterManager.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsPixmapItem> pixmItemRegistrar(OT_FactoryKey_GraphicsImageItem);

ot::GraphicsPixmapItem::GraphicsPixmapItem()
	: ot::CustomGraphicsItem(new GraphicsImageItemCfg)
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsPixmapItem::~GraphicsPixmapItem() {

}

bool ot::GraphicsPixmapItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	const GraphicsImageItemCfg* cfg = dynamic_cast<const GraphicsImageItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->setBlockConfigurationNotifications(true);
	this->prepareGeometryChange();

	try {
		ImagePainterManager::instance().importFromFile(cfg->imagePath());
		this->setBlockConfigurationNotifications(false);
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
		this->setBlockConfigurationNotifications(false);
		return false;
	}
	catch (...) {
		OT_LOG_EA("[FATAL] Unknown error");
		this->setBlockConfigurationNotifications(false);
		return false;
	}
	return ot::CustomGraphicsItem::setupFromConfig(_cfg);
}

QSizeF ot::GraphicsPixmapItem::getPreferredGraphicsItemSize(void) const {
	const GraphicsImageItemCfg* config = dynamic_cast<const GraphicsImageItemCfg*>(this->getConfiguration());
	if (!config) {
		OT_LOG_EA("Config not set");
		return QSizeF();
	}
	ImagePainter* painter = ImagePainterManager::instance().getPainter(config->imagePath());
	if (painter) return painter->getDefaultImageSize();
	else return QSizeF();
}

void ot::GraphicsPixmapItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	const GraphicsImageItemCfg* config = dynamic_cast<const GraphicsImageItemCfg*>(this->getConfiguration());
	if (!config) {
		OT_LOG_EA("Config not set");
		return;
	}
	ImagePainter* painter = ImagePainterManager::instance().getPainter(config->imagePath());
	if (!painter) {
		return;
	}

	QSizeF adjustedSize = painter->getDefaultImageSize();
	if (config->isMaintainAspectRatio()) {
		adjustedSize.scale(_rect.size(), Qt::KeepAspectRatio);
	}
	else {
		adjustedSize = _rect.size();
	}

	QRectF adjustedRect = ot::calculateChildRect(_rect, adjustedSize, this->getGraphicsItemAlignment());
	painter->paintImage(_painter, adjustedRect);

	/*if (m_maintainAspectRatio) {
		QPixmap scaled = m_pixmap.scaled(_rect.size().toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		

		// Check if a color mask is set
		if (m_colorMask.isValid()) {
			QPixmap mask(scaled.size());
			mask.fill(QtFactory::toQColor(m_colorMask));
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
			mask.fill(QtFactory::toQColor(m_colorMask));
			_painter->setCompositionMode(QPainter::CompositionMode_SourceIn);

			_painter->drawPixmap(_rect.topLeft().x(), _rect.topLeft().y(), _rect.width(), _rect.height(), m_pixmap);
			_painter->drawPixmap(_rect.topLeft().x(), _rect.topLeft().y(), _rect.width(), _rect.height(), mask);
		}
		else {
			_painter->drawPixmap(_rect.topLeft().x(), _rect.topLeft().y(), _rect.width(), _rect.height(), m_pixmap);
		}
	}
	*/
}
