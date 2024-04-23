//! @file FrontendAPI.cpp
//!
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGuiAPI/FrontendAPI.h"
#include "OTGuiAPI/GuiAPIManager.h"

bool ot::FrontendAPI::setControlEnabled(const std::string& _controlName, bool _isEnabled, bool _sendRequest) {
	// Ensure the control was actually created
	if (!this->controlExists(_controlName)) {
		OT_LOG_W("A control with the name \"" + _controlName + "\" does not exist");
		return false;
	}

	// Avoid further processing if the enabled state did not change
	if (m_controlsEnabledState[_controlName] == _isEnabled) return true;

	m_controlsEnabledState[_controlName] = _isEnabled;

	if (_sendRequest) {
		m_controlsEnabledStateInFrontend[_controlName] = _isEnabled;

		std::list<std::string> elst;
		std::list<std::string> dlst;

		if (_isEnabled) {
			elst.push_back(_controlName);
		}
		else {
			dlst.push_back(_controlName);
		}

		JsonDocument cmdDoc;
		cmdDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_EnableDisableControls, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_EnabledControlsList, JsonArray(elst, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_UI_DisabledControlsList, JsonArray(dlst, cmdDoc.GetAllocator()), cmdDoc.GetAllocator());
		//cmdDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_application->serviceID(), cmdDoc.GetAllocator());

		return GuiAPIManager::instance().sendQueuedRequestToFrontend(cmdDoc.toJson());
	}
	else {
		return true;
	}
}

bool ot::FrontendAPI::setControlEnabled(const std::string& _toolBarPageName, const std::string& _toolBarGroupName, const std::string& _controlName, bool _isEnabled, bool _sendRequest) {
	return this->setControlEnabled(this->toolBarControlName(_toolBarPageName, _toolBarGroupName, _controlName), _isEnabled, _sendRequest);
}

bool ot::FrontendAPI::setControlEnabled(const std::string& _toolBarPageName, const std::string& _toolBarGroupName, const std::string& _toolBarSubGroupName, const std::string& _controlName, bool _isEnabled, bool _sendRequest) {
	return this->setControlEnabled(this->toolBarControlName(_toolBarPageName, _toolBarGroupName, _toolBarSubGroupName, _controlName), _isEnabled, _sendRequest);
}

bool ot::FrontendAPI::sendCurrentControlEnabledStates(void) {
	return false;
}

