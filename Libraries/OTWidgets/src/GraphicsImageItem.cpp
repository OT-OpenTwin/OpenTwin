// @otlicense
// File: GraphicsImageItem.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
				if (!pix.loadFromData(QtFactory::toQByteArray(cfg->getImageData()), "PNG")) {
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
				if (!pix.loadFromData(QtFactory::toQByteArray(cfg->getImageData()), "JPEG")) {
					OT_LOG_EAS("Failed to load image from data");
				}
				else {
					m_painter = new PixmapImagePainter(pix);
				}
			}
			break;

			case ImageFileFormat::SVG:
			{
				QByteArray svgData = QtFactory::toQByteArray(cfg->getImageData());
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
			// Failed to create painter, default to empty
			m_painter = new PixmapImagePainter(QPixmap());
		}
		
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
		this->setBlockConfigurationNotifications(false);
		return false;
	}

	return CustomGraphicsItem::setupFromConfig(_cfg);
}

QMarginsF ot::GraphicsImageItem::getOutlineMargins() const {
	const GraphicsImageItemCfg* config = dynamic_cast<const GraphicsImageItemCfg*>(this->getConfiguration());
	if (config) {
		const double w = config->getBorderPen().getWidth() / 2.;
		return QMarginsF(w, w, w, w);
	}
	else {
		return CustomGraphicsItem::getOutlineMargins();
	}
}

QSizeF ot::GraphicsImageItem::getPreferredGraphicsItemSize(void) const {
	const GraphicsImageItemCfg* config = dynamic_cast<const GraphicsImageItemCfg*>(this->getConfiguration());
	if (!config) {
		OT_LOG_EA("Config not set");
		return QSizeF();
	}

	if (m_painter) {
		return m_painter->getDefaultImageSizeF();
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

	QSizeF adjustedSize = m_painter->getDefaultImageSizeF();
	if (config->getMaintainAspectRatio()) {
		adjustedSize.scale(_rect.size(), (config->getMaintainAspectRatio() ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio));
	}
	else {
		adjustedSize = _rect.size();
	}

	QRectF adjustedRect = ot::Positioning::calculateChildRect(_rect, adjustedSize, this->getGraphicsItemAlignment());

	// Paint the image
	m_painter->paintImage(_painter, adjustedRect, config->getMaintainAspectRatio());

	// Paint the border if needed
	PenFCfg borderPen = config->getBorderPen();
	if (this->getGraphicsItemFlags() & GraphicsItemCfg::ItemHandlesState) {
		if ((this->getGraphicsElementState() & GraphicsElement::SelectedState) && !(this->getGraphicsElementState() & GraphicsElement::HoverState)) {
			borderPen.setPainter(GraphicsItem::createSelectionBorderPainter());
		}
		else if (this->getGraphicsElementState() & GraphicsElement::HoverState) {
			borderPen.setPainter(GraphicsItem::createHoverBorderPainter());
		}
	}

	_painter->setPen(QtFactory::toQPen(borderPen));
	_painter->setBrush(Qt::NoBrush);
	_painter->drawRect(adjustedRect);
}
