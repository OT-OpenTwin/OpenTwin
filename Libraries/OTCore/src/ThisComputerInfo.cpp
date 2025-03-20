//! @file ThisComputerInfo.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/SystemProcess.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCore/String.h"
#include "OTCore/Logger.h"
#include "OTCore/ThisComputerInfo.h"

// std header
#include <fstream>

#define OT_LCL_LOG_INFO(___text) ot::LogDispatcher::instance().dispatch(___text, "Environment Information", ot::INFORMATION_LOG)

void ot::ThisComputerInfo::logInformation(const InformationModeFlags& _mode) {
	ThisComputerInfo(_mode).logCurrentInformation();
}

std::string ot::ThisComputerInfo::getEnvEntry(EnvironemntEntry entry) {
	using os = OperatingSystem;

	// Otherwise, retrieve from the environment

	switch (entry) {
	case EnvAdminPort:             return os::getEnvironmentVariableString("OPEN_TWIN_ADMIN_PORT");
	case EnvAuthPort:              return os::getEnvironmentVariableString("OPEN_TWIN_AUTH_PORT");
	case EnvCertPath:              return os::getEnvironmentVariableString("OPEN_TWIN_CERTS_PATH");
	case EnvDownloadPort:          return os::getEnvironmentVariableString("OPEN_TWIN_DOWNLOAD_PORT");
	case EnvGssPort:               return os::getEnvironmentVariableString("OPEN_TWIN_GSS_PORT");
	case EnvLssPort:               return os::getEnvironmentVariableString("OPEN_TWIN_LSS_PORT");
	case EnvGdsPort:               return os::getEnvironmentVariableString("OPEN_TWIN_GDS_PORT");
	case EnvLdsPort:               return os::getEnvironmentVariableString("OPEN_TWIN_LDS_PORT");
	case EnvMessageTimeout:        return os::getEnvironmentVariableString("OPEN_TWIN_GLOBAL_TIMEOUT");
	case EnvMongoDBAddress:        return os::getEnvironmentVariableString("OPEN_TWIN_MONGODB_ADDRESS");
	case EnvMongoServicesAddress:  return os::getEnvironmentVariableString("OPEN_TWIN_SERVICES_ADDRESS");
	default:                        return ""; // Handle unexpected values gracefully
	}
}

ot::ThisComputerInfo::ThisComputerInfo(const InformationModeFlags& _mode) {
	m_envData.dataSet = false;
	m_mongoData.dataSet = false;


	this->gatherInformation(_mode);
}

ot::ThisComputerInfo& ot::ThisComputerInfo::gatherInformation(const InformationModeFlags& _mode) {
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

	return *this;
}

ot::ThisComputerInfo& ot::ThisComputerInfo::logCurrentInformation(void) {
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
		OT_LCL_LOG_INFO("MongoDB Address:               " + m_envData.mongoDBAddress);
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

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

void ot::ThisComputerInfo::gatherEnvData(void) {
	m_envData.adminPort            = this->getEnvEntry(EnvAdminPort);
	m_envData.authorizationPort    = this->getEnvEntry(EnvAuthPort);
	m_envData.certificatePath      = this->getEnvEntry(EnvCertPath);
	m_envData.downloadPort         = this->getEnvEntry(EnvDownloadPort);
	m_envData.gssPort              = this->getEnvEntry(EnvGssPort);
	m_envData.lssPort              = this->getEnvEntry(EnvLssPort);
	m_envData.gdsPort              = this->getEnvEntry(EnvGdsPort);
	m_envData.ldsPort              = this->getEnvEntry(EnvLdsPort);
	m_envData.globalMessageTimeout = this->getEnvEntry(EnvMessageTimeout);
	m_envData.mongoDBAddress       = this->getEnvEntry(EnvMongoDBAddress);
	m_envData.mongoServicesAddress = this->getEnvEntry(EnvMongoServicesAddress);

	m_envData.dataSet = true;
}

bool ot::ThisComputerInfo::gatherMongoDBData(void) {
	if (m_mongoData.info.loadSystemServiceConfig("MongoDB")) {
		m_mongoData.dataSet = true;
		return true;
	}
	else {
		return false;
	}
}

void ot::ThisComputerInfo::gatherMongoDBConfigFileContent(void) {
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
