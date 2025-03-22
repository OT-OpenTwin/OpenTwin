//! @file ThisComputerInfo.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/SystemProcess.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCore/Logger.h"
#include "OTCore/String.h"
#include "OTCore/ThisComputerInfo.h"

// std header
#include <fstream>

std::string ot::ThisComputerInfo::toInfoString(const InformationModeFlags& _mode) {
	return ThisComputerInfo(_mode).toInfoString();
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

std::string ot::ThisComputerInfo::toInfoString(void) {
	std::stringstream ss;
	bool empty = true;
	const std::string delimiterLine = "---------------------------------------------------------------------------";

	// Environment
	if (m_envData.dataSet) {
		if (!empty) {
			ss << std::endl;
		}
		empty = false;

		ss << delimiterLine << std::endl;
		ss << "         ENVIRONMENT" << std::endl;
		ss << "" << std::endl;
		ss << "Admin Port:                    " + m_envData.adminPort << std::endl;
		ss << "Authorization Port:            " + m_envData.authorizationPort << std::endl;
		ss << "Certificate Path:              " + m_envData.certificatePath << std::endl;
		ss << "Download Port:                 " + m_envData.downloadPort << std::endl;
		ss << "Global Session Service Port:   " + m_envData.gssPort << std::endl;
		ss << "Local Session Service Port:    " + m_envData.lssPort << std::endl;
		ss << "Global Directory Service Port: " + m_envData.gdsPort << std::endl;
		ss << "Local Directory Service Port:  " + m_envData.ldsPort << std::endl;
		ss << "MongoDB Address:               " + m_envData.mongoDBAddress << std::endl;
		ss << "MongoDB Services Address:      " + m_envData.mongoServicesAddress << std::endl;
		ss << "";
	}

	// MongoDB
	if (m_mongoData.dataSet) {
		if (!empty) {
			ss << std::endl;
		}
		empty = false;

		ss << delimiterLine << std::endl;
		ss << "         MONGODB CONFIG" << std::endl;
		ss << "" << std::endl;
		ss << "Service Name:                  " + m_mongoData.info.getServiceName() << std::endl;
		ss << "Display Name:                  " + m_mongoData.info.getDisplayName() << std::endl;
		ss << "Binary Path:                   " + m_mongoData.info.getBinaryPath() << std::endl;
		ss << "Config Path:                   " + m_mongoData.info.getConfigPath() << std::endl;
		ss << "Service Path:                  " + m_mongoData.info.getServicePath() << std::endl;
		ss << "Start Type:                    " + m_mongoData.info.getStartType() << std::endl;
		ss << "" << std::endl;

		// MongoDB Config file
		ss << delimiterLine << std::endl;
		ss << "         MONGODB CONFIG FILE CONTENT" << std::endl;
		ss << "" << std::endl;

		if (!m_mongoData.configFileContent.empty()) {
			ss << "File (" << m_mongoData.info.getConfigPath() << "):" << std::endl;
			ss << std::endl;
			ss << m_mongoData.configFileContent << std::endl;
		}
		else {
			ss << "" << std::endl;
			ss << "<Mongo config file is empty>" << std::endl;
		}

		ss << "";
	}
		
	return ss.str();
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
