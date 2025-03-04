//! @file OTEnvInfo.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/SystemProcess.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCore/String.h"
#include "OTCore/Logger.h"
#include "OTCore/OTEnvInfo.h"

// std header
#include <fstream>

#define OT_LCL_LOG_INFO(___text) ot::LogDispatcher::instance().dispatch(___text, "Environment Information", ot::INFORMATION_LOG)

ot::OTEnvInfo::OTEnvInfo() {
	this->resetData();
}

void ot::OTEnvInfo::gatherInformation(const InformationModeFlags& _mode) {
	if (_mode & InformationModeFlag::Environment) {
		this->gatherEnvData();
	}
	if (_mode & InformationModeFlag::MongoDBConfig) {
		if (this->gatherMongoDBData()) {
			if (_mode & InformationModeFlag::MongoDBConfigFileContent) {
				this->gatherMongoDBConfigFileContent();
			}
		}
	}
}

void ot::OTEnvInfo::logCurrentInformation(void) {
	const std::string delimiterLine = "---------------------------------------------------------------------------";

	// Environment
	if (m_envData.dataSet) {
		OT_LCL_LOG_INFO(delimiterLine);
		OT_LCL_LOG_INFO("         ENVIRONMENT");
		OT_LCL_LOG_INFO("");
		OT_LCL_LOG_INFO("Admin Port:                    " + m_envData.adminPort);
		OT_LCL_LOG_INFO("Authorization Port:            " + m_envData.authorizationPort);
		OT_LCL_LOG_INFO("Certificate Path:              " + m_envData.certificatePath);
		OT_LCL_LOG_INFO("Download Port:                 " + m_envData.downloadPort);
		OT_LCL_LOG_INFO("Global Session Service Port:   " + m_envData.gssPort);
		OT_LCL_LOG_INFO("Local Session Service Port:    " + m_envData.lssPort);
		OT_LCL_LOG_INFO("Global Directory Service Port: " + m_envData.gdsPort);
		OT_LCL_LOG_INFO("Local Directory Service Port:  " + m_envData.ldsPort);
		OT_LCL_LOG_INFO("MongoDB Adress:                " + m_envData.mongoDBAdress);
		OT_LCL_LOG_INFO("MongoDB Services Address:      " + m_envData.mongoServicesAddress);
		OT_LCL_LOG_INFO("");
	}

	// MongoDB
	if (m_mongoData.dataSet) {
		OT_LCL_LOG_INFO(delimiterLine);
		OT_LCL_LOG_INFO("         MONGODB CONFIG");
		OT_LCL_LOG_INFO("");
		OT_LCL_LOG_INFO("Service Name:                  " + m_mongoData.info.getServiceName());
		OT_LCL_LOG_INFO("Display Name:                  " + m_mongoData.info.getDisplayName());
		OT_LCL_LOG_INFO("Binary Path:                   " + m_mongoData.info.getBinaryPath());
		OT_LCL_LOG_INFO("Config Path:                   " + m_mongoData.info.getConfigPath());
		OT_LCL_LOG_INFO("Service Path:                  " + m_mongoData.info.getServicePath());
		OT_LCL_LOG_INFO("Start Type:                    " + m_mongoData.info.getStartType());
		OT_LCL_LOG_INFO("");

		// MongoDB Config file
		OT_LCL_LOG_INFO(delimiterLine);
		OT_LCL_LOG_INFO("         MONGODB CONFIG FILE CONTENT");

		if (!m_mongoData.configFileContent.empty()) {
			OT_LCL_LOG_INFO("\nFile (" + m_mongoData.info.getConfigPath() + "):\n\n" + m_mongoData.configFileContent);
		}
		else {
			OT_LCL_LOG_INFO("");
			OT_LCL_LOG_INFO("<Mongo config file is empty>");
		}
		OT_LCL_LOG_INFO("");
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

void ot::OTEnvInfo::resetData(void) {
	m_envData.dataSet = false;
	m_mongoData.dataSet = false;
}

void ot::OTEnvInfo::gatherEnvData(void) {
	using os = OperatingSystem;
	m_envData.adminPort = os::getEnvironmentVariableString("OPEN_TWIN_ADMIN_PORT");
	m_envData.authorizationPort = os::getEnvironmentVariableString("OPEN_TWIN_AUTH_PORT");
	m_envData.certificatePath = os::getEnvironmentVariableString("OPEN_TWIN_CERTS_PATH");
	m_envData.downloadPort = os::getEnvironmentVariableString("OPEN_TWIN_DOWNLOAD_PORT");
	m_envData.gssPort = os::getEnvironmentVariableString("OPEN_TWIN_GSS_PORT");
	m_envData.lssPort = os::getEnvironmentVariableString("OPEN_TWIN_LSS_PORT");
	m_envData.gdsPort = os::getEnvironmentVariableString("OPEN_TWIN_GDS_PORT");
	m_envData.ldsPort = os::getEnvironmentVariableString("OPEN_TWIN_LDS_PORT");
	m_envData.mongoDBAdress = os::getEnvironmentVariableString("OPEN_TWIN_MONGODB_ADDRESS");
	m_envData.mongoServicesAddress = os::getEnvironmentVariableString("OPEN_TWIN_SERVICES_ADDRESS");

	m_envData.dataSet = true;
}

bool ot::OTEnvInfo::gatherMongoDBData(void) {
	if (m_mongoData.info.loadSystemServiceConfig("MongoDB")) {
		m_mongoData.dataSet = true;
		return true;
	}
	else {
		return false;
	}
}

void ot::OTEnvInfo::gatherMongoDBConfigFileContent(void) {
	if (m_mongoData.info.getConfigPath().empty()) {
		OT_LOG_E("Configuration file path is empty");
		return;
	}

	std::ifstream file(m_mongoData.info.getConfigPath());
	if (!file) {
		OT_LOG_E("Failed to open file: \"" + m_mongoData.info.getConfigPath() + "\"");
	}
	else {
		std::ostringstream ss;
		ss << file.rdbuf();
		m_mongoData.configFileContent = ss.str();
		
		file.close();
	}
}
