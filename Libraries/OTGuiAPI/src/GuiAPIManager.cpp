// @otlicense

//! @file GraphicsFlowItem.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/ThisService.h"
#include "OTCommunication/Msg.h"
#include "OTGuiAPI/GuiAPIManager.h"

ot::GuiAPIManager& ot::GuiAPIManager::instance(void) {
	static GuiAPIManager g_instance;
	return g_instance;
}

void ot::GuiAPIManager::frontendConnected(const ServiceBase& _serviceInformation) {
	m_frontend = _serviceInformation;
	if (m_frontend.getServiceURL().empty()) {
		OT_LOG_W("Provided frontend infromation does not contain a url");
	}
	else {
		OT_LOG_D("User frontend url set to \"" + m_frontend.getServiceURL() + "\"");
	}
}

void ot::GuiAPIManager::frontendDisconnected(void) {
	m_frontend.setServiceURL("");
}

bool ot::GuiAPIManager::sendQueuedRequestToFrontend(const std::string& _message) {
	if (!this->isFrontendConnected()) {
		OT_LOG_W("User frontend is not connected");
		return false;
	}

	std::string tmp;
	return msg::send(ThisService::instance().getServiceURL(), m_frontend.getServiceURL(), ot::QUEUE, _message, tmp);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private functions

ot::GuiAPIManager::GuiAPIManager() {
	m_frontend.setServiceURL("");
}

ot::GuiAPIManager::~GuiAPIManager() {

}