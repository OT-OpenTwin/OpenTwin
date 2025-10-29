// @otlicense

//! @file ServiceDataUi.cpp
//!
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "ServiceDataUi.h"

ServiceDataUi::ServiceDataUi() 
	: m_uiInitCompleted(false)
{

}

ServiceDataUi::ServiceDataUi(const std::string& _name, const std::string& _type, const std::string& _url, ot::serviceID_t _id) 
	: ot::ServiceBase(_name, _type, _url, _id), m_uiInitCompleted(false)
{

}

ServiceDataUi::~ServiceDataUi() {

}