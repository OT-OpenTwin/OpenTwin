#include "DataBaseInfo.h"

DataBaseInfo::DataBaseInfo(const DataBaseInfo& _other) {
	*this = _other;
}

DataBaseInfo& DataBaseInfo::operator=(const DataBaseInfo& _other) {
	if (this != &_other) {
		m_dbUrl = _other.m_dbUrl;
		m_siteId = _other.m_siteId;
		m_collectionName = _other.m_collectionName;

		m_userName = _other.m_userName;
		m_userPassword = _other.m_userPassword;
	}

	return *this;
}

bool DataBaseInfo::hasInfoSet(void) const {
	return !m_dbUrl.empty() && !m_siteId.empty() && !m_collectionName.empty() && !m_userName.empty() && !m_userPassword.empty();
}
