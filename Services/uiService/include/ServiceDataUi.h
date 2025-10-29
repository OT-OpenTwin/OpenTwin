// @otlicense

//! @file ServiceDataUi.h
//!
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/ServiceBase.h"

class ServiceDataUi : public ot::ServiceBase {
public:
	ServiceDataUi();
	ServiceDataUi(const std::string& _name, const std::string& _type, const std::string& _url, ot::serviceID_t _id);
	virtual ~ServiceDataUi();

	void setUiInitializationCompleted(bool _completed = true) { m_uiInitCompleted = _completed; };
	bool isUiInitializationCompleted(void) const { return m_uiInitCompleted; };

private:
	bool m_uiInitCompleted;
};
