//! @file ProjectInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "ProjectInformation.h"

// OpenTwin header
#include "OTCommunication/ActionTypes.h"

ProjectInformation::ProjectInformation() {

}

ProjectInformation::ProjectInformation(const ot::ConstJsonObject& _jsonObject) {
	m_name = ot::json::getString(_jsonObject, OT_PARAM_AUTH_NAME);
	m_type = ot::json::getString(_jsonObject, OT_PARAM_AUTH_PROJECT_TYPE);
	m_user = ot::json::getString(_jsonObject, OT_PARAM_AUTH_OWNER);
	m_lastAccessTime = QDateTime::fromMSecsSinceEpoch(ot::json::getInt64(_jsonObject, OT_PARAM_AUTH_PROJECT_LASTACCESS));
	m_groups = ot::json::getStringList(_jsonObject, "groups");
}

void ProjectInformation::addGroup(const std::string& _group) {
	m_groups.push_back(_group);
}
