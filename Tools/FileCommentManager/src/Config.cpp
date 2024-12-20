// Project header
#include "Logger.h"
#include "Config.h"
#include "JSONMember.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsondocument.h>

#define LOG_I(___message) LOG_INFO("Config", ___message)
#define LOG_W(___message) LOG_WARNING("Config", ___message)
#define LOG_E(___message) LOG_ERROR("Config", ___message)

bool Config::readFromFile(const QString& _filePath, const QString& _devRoot) {
	// Ini dev root
	QString devRoot = _devRoot;
	devRoot.replace('\\', '/');
	if (!devRoot.endsWith('/'));
	devRoot.append("/");

	// Open config file
	QFile file(_filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		LOG_E("Failed to open configuration file for reading. File: \"" + _filePath + "\".");
		return false;
	}

	// Read config
	QByteArray fileData = file.readAll();
	file.close();

	// Parse config
	QJsonParseError parseError;
	QJsonDocument configDoc = QJsonDocument::fromJson(fileData, &parseError);
	if (parseError.error != QJsonParseError::NoError) {
		LOG_E("Configuration file is broken. { \"File\": \"" + _filePath + "\", \"ParseError\": \"" + parseError.errorString() + "\" }");
		return false;
	}

	// Check members
	if (!configDoc.isObject()) {
		LOG_E("Configuration file is broken. { \"File\": \"" + _filePath + "\", \"ParseError\": \"Not an object\" }");
		return false;
	}
	QJsonObject docObj = std::move(configDoc.object());
	if (!docObj.contains(JSON_MEMBER_ProjectSearchPaths)) {
		LOG_E("Configuration file is broken. { \"File\": \"" + _filePath + "\", \"DataError\": \"Missing member '" JSON_MEMBER_ProjectSearchPaths "'\" }");
		return false;
	}
	if (!docObj.contains(JSON_MEMBER_FileExtensions)) {
		LOG_E("Configuration file is broken. { \"File\": \"" + _filePath + "\", \"DataError\": \"Missing member '" JSON_MEMBER_FileExtensions "'\" }");
		return false;
	}

	QJsonArray searchPathsArr = std::move(docObj[JSON_MEMBER_ProjectSearchPaths].toArray());
	for (int i = 0; i < searchPathsArr.count(); i++) {
		m_projectSearchPaths.push_back(devRoot + searchPathsArr[i].toString());
	}

	QJsonArray fileExtensionArr = std::move(docObj[JSON_MEMBER_FileExtensions].toArray());
	for (int i = 0; i < fileExtensionArr.count(); i++) {
		m_fileExtensions.push_back(fileExtensionArr[i].toString());
	}

	LOG_I("Configuration setup from file completed with:\n"
		"\t- " + QString::number(m_projectSearchPaths.count()) + " Project Search Paths\n"
		"\t- " + QString::number(m_fileExtensions.count()) + " File Extensions"
	);

	return true;
}

Config::Config() {

}