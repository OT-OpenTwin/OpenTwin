//! @file GraphicsPixmapItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/Positioning.h"
#include "OTWidgets/ImagePainter.h"
#include "OTWidgets/SvgImagePainter.h"
#include "OTWidgets/GraphicsImageItem.h"
#include "OTWidgets/PixmapImagePainter.h"
#include "OTWidgets/ImagePainterManager.h"
#include "OTWidgets/GraphicsItemFactory.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtSvg/qsvgrenderer.h>

static ot::GraphicsItemFactoryRegistrar<ot::GraphicsImageItem> imgItemRegistrar(ot::GraphicsImageItemCfg::className());

ot::GraphicsImageItem::GraphicsImageItem()
	: CustomGraphicsItem(new GraphicsImageItemCfg()), m_painter(nullptr)
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred));
	this->setGraphicsItem(this);
	this->setFlags(this->flags() | QGraphicsItem::ItemSendsScenePositionChanges);
}

ot::GraphicsImageItem::~GraphicsImageItem() {
	if (m_painter) {
		delete m_painter;
		m_painter = nullptr;
	}
}

bool ot::GraphicsImageItem::setupFromConfig(const GraphicsItemCfg* _cfg) {
	const GraphicsImageItemCfg* cfg = dynamic_cast<const GraphicsImageItemCfg*>(_cfg);
	if (cfg == nullptr) {
		OT_LOG_EA("Invalid configuration provided: Cast failed");
		return false;
	}

	this->setBlockConfigurationNotifications(true);
	this->prepareGeometryChange();

	if (m_painter) {
		delete m_painter;
		m_painter = nullptr;
	}

	try {
		if (!cfg->getImageData().empty()) {
			switch (cfg->getImageFileFormat()) {
			case ImageFileFormat::PNG:
			{
				QPixmap pix;
				if (!pix.loadFromData(QByteArray::fromStdString(cfg->getImageData()), "PNG")) {
					OT_LOG_EAS("Failed to load image from data");
				}
				else {
					m_painter = new PixmapImagePainter(pix);
				}
			}
			break;

			case ImageFileFormat::JPEG:
			{
				QPixmap pix;
				if (!pix.loadFromData(QByteArray::fromStdString(cfg->getImageData()), "JPEG")) {
					OT_LOG_EAS("Failed to load image from data");
				}
				else {
					m_painter = new PixmapImagePainter(pix);
				}
			}
			break;

			case ImageFileFormat::SVG:
			{
				QByteArray svgData = QByteArray::fromStdString(cfg->getImageData());
				SvgImagePainter* newPainter = new SvgImagePainter(svgData);
				if (!newPainter->isValid()) {
					OT_LOG_EAS("Failed to load svg image from data");
					delete newPainter;
				}
				else {
					m_painter = newPainter;
				}
			}
			break;

			default:
				OT_LOG_EAS("Unsupported image format \"" + ot::toString(cfg->getImageFileFormat()) + "\"");
			}
		}
		else if (!cfg->getImagePath().empty()) {
			ImagePainterManager::instance().importFromFile(cfg->getImagePath());
			const ImagePainter* newPainter = ImagePainterManager::instance().getPainter(cfg->getImagePath());
			if (newPainter) {
				m_painter = newPainter->createCopy();
			}
		}
		else {
			OT_LOG_EA("No image data or path provided");
		}

		this->setBlockConfigurationNotifications(false);

		if (!m_painter) {
			return false;
		}
		
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
		this->setBlockConfigurationNotifications(false);
		return false;
	}

	return CustomGraphicsItem::setupFromConfig(_cfg);
}

QSizeF ot::GraphicsImageItem::getPreferredGraphicsItemSize(void) const {
	const GraphicsImageItemCfg* config = dynamic_cast<const GraphicsImageItemCfg*>(this->getConfiguration());
	if (!config) {
		OT_LOG_EA("Config not set");
		return QSizeF();
	}

	if (m_painter) {
		return m_painter->getDefaultImageSize();
	}
	else {
		return QSizeF();
	}
}

void ot::GraphicsImageItem::setImagePath(const std::string& _imagePath) {
	GraphicsImageItemCfg* config = dynamic_cast<GraphicsImageItemCfg*>(this->getConfiguration());
	if (!config) {
		OT_LOG_EA("Config not set");
		return;
	}

	config->setImagePath(_imagePath);
}

void ot::GraphicsImageItem::paintCustomItem(QPainter* _painter, const QStyleOptionGraphicsItem* _opt, QWidget* _widget, const QRectF& _rect) {
	const GraphicsImageItemCfg* config = dynamic_cast<const GraphicsImageItemCfg*>(this->getConfiguration());
	if (!config) {
		OT_LOG_EA("Config not set");
		return;
	}

	if (m_painter == nullptr) {
		return;
	}

	QSizeF adjustedSize = m_painter->getDefaultImageSize();
	if (config->getMaintainAspectRatio()) {
		adjustedSize.scale(_rect.size(), Qt::KeepAspectRatio);
	}
	else {
		adjustedSize = _rect.size();
	}

	QRectF adjustedRect = ot::Positioning::calculateChildRect(_rect, adjustedSize, this->getGraphicsItemAlignment());
	m_painter->paintImage(_painter, adjustedRect);
}
