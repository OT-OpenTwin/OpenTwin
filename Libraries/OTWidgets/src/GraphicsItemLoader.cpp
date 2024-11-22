//! \file GraphicsItemLoader.cpp
//! \author Alexander Kuester (alexk95)
//! \date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTWidgets/GraphicsItemLoader.h"

// Qt header
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>

ot::GraphicsItemLoader& ot::GraphicsItemLoader::instance(void) {
	static GraphicsItemLoader g_instance;
	return g_instance;
}

// ###########################################################################################################################################################################################################################################################################################################################

bool ot::GraphicsItemLoader::addSearchPath(const QString& _path) {
	QString path = _path;
	path.replace('\\', '/');
	if (!path.endsWith('/')) {
		path.append('/');
	}

	auto it = std::find(m_searchPaths.begin(), m_searchPaths.end(), path);
	if (it != m_searchPaths.end()) {
		OT_LOG_W("Search path already set. Ignoring...");
		return false;
	}

	QDir dir(path);
	if (!dir.exists()) {
		OT_LOG_W("Search directory does not exist: \"" + _path.toStdString() + "\"");
		return false;
	}

	m_searchPaths.push_back(path);
	return true;
}

ot::GraphicsItemCfg* ot::GraphicsItemLoader::createConfiguration(const QString& _subPath) {
	auto it = m_configurations.find(_subPath);
	if (it != m_configurations.end()) {
		return it->second;
	}

	GraphicsItemCfg* newConfig = nullptr;

	QString fileName = this->findFullItemPath(_subPath);
	if (fileName.isEmpty()) {
		OT_LOG_E("Configuration file not found \"" + _subPath.toStdString() + "\"");
		return newConfig;
	}

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		OT_LOG_E("Failed to open file for reading: \"" + fileName.toStdString()+ "\"");
		return newConfig;
	}

	QByteArray configData = file.readAll();
	file.close();

	if (configData.isEmpty()) {
		OT_LOG_E("Configuration file is broken: \"" + fileName.toStdString() + "\"");
		return newConfig;
	}

	std::string configString = configData.toStdString();
	JsonDocument configDoc;
	if (!configDoc.fromJson(configString.c_str())) {
		OT_LOG_E("Configuration file is broke: Failed to parse: \"" + fileName.toStdString() + "\"");
		return newConfig;
	}

	newConfig = GraphicsItemCfgFactory::create(configDoc.GetConstObject());

	if (newConfig) {
		m_configurations.insert_or_assign(_subPath, newConfig);
	}

	return newConfig;
}

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsItemLoader::GraphicsItemLoader()
	
{

}

ot::GraphicsItemLoader::~GraphicsItemLoader() {

}

QString ot::GraphicsItemLoader::findFullItemPath(const QString& _subPath) const {
	for (const QString& searchPath : m_searchPaths) {
		QFile file(searchPath + _subPath);
		if (file.exists()) {
			return searchPath + _subPath;
		}
	}
	return QString();
}