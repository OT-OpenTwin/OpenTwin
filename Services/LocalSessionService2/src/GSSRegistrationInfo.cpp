//! @file GSSRegistrationInfo.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2025
// ###########################################################################################################################################################################################################################################################################################################################

// LSS header
#include "GSSRegistrationInfo.h"

GSSRegistrationInfo::GSSRegistrationInfo() :
	m_serviceID(0), m_logFlagsSet(false)
{
	this->reset();
}

void GSSRegistrationInfo::reset(void) {
	m_serviceID = ot::invalidServiceID;
	m_databaseURL.clear();
	m_authURL.clear();
	m_gdsURL.clear();
	m_logFlagsSet = false;
}
