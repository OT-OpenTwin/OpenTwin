//! @file StartArgumentParser.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "StartArgumentParser.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"

// Qt header
#include <qtcore/qfile.h>
#include <QtCore/qstandardpaths.h>

bool StartArgumentParser::importConfig() {
	// Reset data
	m_logIn = false;
	m_openProject = false;

	m_loginData = LoginData();
	m_projectInfo = ot::ProjectInformation();

	// Read data
	const QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
	const QString filePath = tempPath + "/OpenTwinFrontendStartupData.otdat";

	QFile file(filePath);
	if (!file.exists()) {
		return true;
	}

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		OT_LOG_E("Failed to open temporary file for reading: \"" + filePath.toStdString() + "\"");
		return false;
	}

	QByteArray fileContent = file.readAll();
	file.close();

	// Check for empty file
	if (fileContent.isEmpty()) {
		return true;
	}

	// Parse data
	ot::JsonDocument doc;
	if (!doc.fromJson(fileContent.toStdString())) {
		OT_LOG_E("Failed to parse temporary file content: \"" + filePath.toStdString() + "\"");
		return false;
	}

	m_logIn = ot::json::getBool(doc, "LogIn", false);
	if (m_logIn) {
		m_loginData.setFromRequiredDataJson(ot::json::getObject(doc, "LoginData"));
	}
	m_openProject = ot::json::getBool(doc, "OpenProject", false);
	if (m_openProject) {
		m_projectInfo.setFromJsonObject(ot::json::getObject(doc, "ProjectData"));
	}
	
	// Remove file
	if (!QFile::remove(filePath)) {
		OT_LOG_W("Failed to remove temporary file: \"" + filePath.toStdString() + "\"");
	}

    return true;
}

bool StartArgumentParser::exportConfig() const {
	// Create config
	ot::JsonDocument doc;

	doc.AddMember("LogIn", m_logIn, doc.GetAllocator());
	ot::JsonObject logInObj;
	m_loginData.addRequiredDataToJson(logInObj, doc.GetAllocator());
	doc.AddMember("LoginData", logInObj, doc.GetAllocator());

	doc.AddMember("OpenProject", m_openProject, doc.GetAllocator());
	doc.AddMember("ProjectData", ot::JsonObject(m_projectInfo, doc.GetAllocator()), doc.GetAllocator());

	// Write data
	const QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
	const QString filePath = tempPath + "/OpenTwinFrontendStartupData.otdat";

	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
		OT_LOG_E("Failed to open temporary file for writing: \"" + filePath.toStdString() + "\"");
		return false;
	}

	file.write(QByteArray::fromStdString(doc.toJson()));
	file.close();
	return true;
}

StartArgumentParser::StartArgumentParser() : 
	m_logIn(false), m_openProject(false) 
{}