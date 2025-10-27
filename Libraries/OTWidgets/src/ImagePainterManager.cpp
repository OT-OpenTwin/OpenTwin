//! @file ImagePainterManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
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

const ot::ImagePainter* ot::ImagePainterManager::getPainter(const std::string& _key) const {
	const auto& it = m_painter.find(_key);
	if (it == m_painter.end()) {
		OT_LOG_EAS("ImagePainter \"" + _key + "\" not found");
		return nullptr;
	}
	else return it->second;
}

ot::ImagePainter* ot::ImagePainterManager::createFromRawData(const std::vector<char>& _data, ImageFileFormat _format) {
	switch (_format) {
	case ot::ImageFileFormat::PNG:
	{
		QPixmap pixmap;
		if (!pixmap.loadFromData(QByteArray::fromRawData(_data.data(), static_cast<int>(_data.size())), "png")) {
			OT_LOG_E("Failed to load png file data");
			return nullptr;
		}
		return new PixmapImagePainter(pixmap);
	}
		break;
	case ot::ImageFileFormat::JPEG:
	{
		QPixmap pixmap;
		if (!pixmap.loadFromData(QByteArray::fromRawData(_data.data(), static_cast<int>(_data.size())), "jpeg")) {
			OT_LOG_E("Failed to load jpeg file data");
			return nullptr;
		}
		return new PixmapImagePainter(pixmap);
	}
		break;
	case ot::ImageFileFormat::SVG:
	{
		QByteArray svgData = QByteArray::fromRawData(_data.data(), static_cast<int>(_data.size()));
		SvgImagePainter* newPainter = new SvgImagePainter(svgData);
		if (!newPainter->isValid()) {
			OT_LOG_EA("Invalid svg data");
			delete newPainter;
			return nullptr;
		}
		else {
			return newPainter;
		}
	}
		break;
	default:
		OT_LOG_E("Unsupported image file format (" + std::to_string(static_cast<int>(_format)) + ")");
		return nullptr;
	}
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
	if (_fileSubPath.empty()) {
		OT_LOG_E("Empty file path");
		return;
	}

	if (this->contains(_fileSubPath)) {
		return;
	}

	std::string actualPath = _fileSubPath;
	std::list<std::string> filePath = ot::String::split(_fileSubPath, '.', true);
	std::string actualExtension;

	if (filePath.size() > 1) {
		// File extension provided
		actualExtension = filePath.back();
	}
	else if (filePath.size() == 1) {
		// No file extension find best
		std::list<std::string> testExtensions = { "png", "svg" };
		while (!testExtensions.empty()) {
			std::string extension = testExtensions.back();
			testExtensions.pop_back();
			std::string testPath = _fileSubPath + "." + extension;
			if (IconManager::fileExists(QString::fromStdString(testPath))) {
				actualPath = std::move(testPath);
				actualExtension = std::move(extension);
				break;
			}
		}
	}
	else {
		OT_LOG_E("Invalid file path \"" + _fileSubPath + "\"");
		return;
	}


	ImagePainter* newPainter = nullptr;
	
	if (actualExtension.empty()) {
		OT_LOG_E("Failed to determine file extension. Ignoring import request for \"" + _fileSubPath + "\"");
		return;
	}

	if (actualExtension == "png") {
		newPainter = this->importPng(actualPath);
	}
	else if (actualExtension == "svg") {
		newPainter = this->importSvg(actualPath);
	}
	else {
		OT_LOG_W("Unknown file type \"" + actualExtension + "\". Defaulting to pixmap..");
		newPainter = this->importPng(actualPath);
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
	SvgImagePainter* newPainter = new SvgImagePainter(data);
	if (!newPainter->isValid()) {
		OT_LOG_EA("Invalid svg data");
		delete newPainter;
		return nullptr;
	}
	else {
		return newPainter;
	}
}

ot::ImagePainterManager::ImagePainterManager() {

}

ot::ImagePainterManager::~ImagePainterManager() {
	this->clear();
}