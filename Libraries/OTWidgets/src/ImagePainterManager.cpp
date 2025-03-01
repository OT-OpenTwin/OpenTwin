//! @file ImagePainterManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/String.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/ImagePainter.h"
#include "OTWidgets/SvgImagePainter.h"
#include "OTWidgets/PixmapImagePainter.h"
#include "OTWidgets/ImagePainterManager.h"

// Qt header
#include <QtSvg/qsvgrenderer.h>

ot::ImagePainterManager& ot::ImagePainterManager::instance(void) {
	static ImagePainterManager g_instance;
	return g_instance;
}

void ot::ImagePainterManager::paintImage(const std::string& _key, QPainter* _painter, const QRectF& _bounds) const {
	
}

ot::ImagePainter* ot::ImagePainterManager::getPainter(const std::string& _key) const {
	const auto& it = m_painter.find(_key);
	if (it == m_painter.end()) {
		OT_LOG_EAS("ImagePainter \"" + _key + "\" not found");
		return nullptr;
	}
	else return it->second;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data management

void ot::ImagePainterManager::addImagePainter(const std::string& _key, ImagePainter* _painter) {
	if (this->contains(_key)) {
		OT_LOG_W("A painter for the key  \"" + _key + "\" already exists. Ignoring..");
		return;
	}
	m_painter.insert_or_assign(_key, _painter);
}

void ot::ImagePainterManager::importFromFile(const std::string& _fileSubPath) {
	if (this->contains(_fileSubPath)) {
		return;
	}

	std::list<std::string> filePath = ot::String::split(_fileSubPath, '.', true);
	ImagePainter* newPainter = nullptr;

	if (filePath.empty()) newPainter = this->importPng(_fileSubPath);
	else if (filePath.back() == "png") newPainter = this->importPng(_fileSubPath);
	else if (filePath.back() == "svg") newPainter = this->importSvg(_fileSubPath);
	else {
		OT_LOG_E("Unknown file type. Defaulting to pixmap..");
		newPainter = this->importPng(_fileSubPath);
	}

	if (newPainter) {
		m_painter.insert_or_assign(_fileSubPath, newPainter);
	}
}

ot::ImagePainter* ot::ImagePainterManager::removeImagePainter(const std::string& _key, bool _destroyInstance) {
	ImagePainter* ptr = nullptr;

	auto it = m_painter.find(_key);
	if (it != m_painter.end()) {
		ptr = it->second;
		if (ptr && _destroyInstance) {
			delete ptr;
			ptr = nullptr;
		}

		m_painter.erase(it);
	}

	return ptr;
}

void ot::ImagePainterManager::clear(void) {
	for (const auto& it : m_painter) {
		delete it.second;
	}
	m_painter.clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

ot::ImagePainter* ot::ImagePainterManager::importPng(const std::string& _subPath) {
	return new PixmapImagePainter(IconManager::getPixmap(QString::fromStdString(_subPath)));
}

ot::ImagePainter* ot::ImagePainterManager::importSvg(const std::string& _subPath) {
	QByteArray data = IconManager::getSvgData(QString::fromStdString(_subPath));
	if (data.isEmpty()) {
		OT_LOG_EA("Empty svg data");
		return nullptr;
	}
	QSvgRenderer* newRenderer = new QSvgRenderer(data);
	if (!newRenderer->isValid()) {
		OT_LOG_EA("Invalid svg renderer");
		delete newRenderer;
		return nullptr;
	}
	else {
		return new SvgImagePainter(newRenderer);
	}
}

ot::ImagePainterManager::ImagePainterManager() {

}

ot::ImagePainterManager::~ImagePainterManager() {
	this->clear();
}