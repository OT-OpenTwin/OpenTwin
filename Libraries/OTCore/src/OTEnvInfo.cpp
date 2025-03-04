//! @file OTEnvInfo.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/OTEnvInfo.h"

ot::OTEnvInfo::OTEnvInfo() {
	this->resetData();
}

void ot::OTEnvInfo::gatherInformation(const InformationModeFlags& _mode) {
	if (_mode & InformationModeFlag::Environment) {
		this->gatherEnvData();
	}
	if (_mode & InformationModeFlag::MongoDBConfig) {
		this->gatherEnvData();
	}
}

void ot::OTEnvInfo::logCurrentInformation(void) {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

void ot::OTEnvInfo::resetData(void) {
	m_envData.dataSet = false;
}

void ot::OTEnvInfo::gatherEnvData(void) {

}

void ot::OTEnvInfo::gatherMongoDBData(void) {

}
