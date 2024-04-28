//! @file FrontendAPI.h
//!
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGuiAPI/OTGuiAPIAPIExport.h"

// std header
#include <map>
#include <list>
#include <string>

namespace ot {

	//! @class FrontendAPI
	//! @brief Frontend request layer.
	//! The FrontendAPI prvoides a set methods that may be used to send requests to the frontend.
	class FrontendAPI {
		OT_DECL_NOCOPY(FrontendAPI)
	public:
		// ###########################################################################################################################################################################################################################################################################################################################

		// General

		//! @brief Sets the control enabled state.
		//! @param _controlName Name of the control.
		//! @param _isEnabled If true the control should be enabled.
		//! @param _sendRequest If true the enabled state request will be send to the frontend, otherwise the state will be buffered to be send later.
		bool setControlEnabled(const std::string& _controlName, bool _isEnabled = true, bool _sendRequest = true);

		//! @brief Sets the control enabled state.
		//! @param _toolBarPageName Name of the ToolBar page where the controls group is located at.
		//! @param _toolBarGroupName Name of the ToolBar group where the control is located at.
		//! @param _controlName Name of the control.
		//! @param _isEnabled If true the control should be enabled.
		//! @param _sendRequest If true the enabled state request will be send to the frontend, otherwise the state will be buffered to be send later.
		bool setControlEnabled(const std::string& _toolBarPageName, const std::string& _toolBarGroupName, const std::string& _controlName, bool _isEnabled = true, bool _sendRequest = true);

		//! @brief Sets the control enabled state.
		//! @param _toolBarPageName Name of the ToolBar page where the controls group is located at.
		//! @param _toolBarGroupName Name of the ToolBar group where the control is located at.
		//! @param _controlName Name of the control.
		//! @param _isEnabled If true the control should be enabled.
		//! @param _sendRequest If true the enabled state request will be send to the frontend, otherwise the state will be buffered to be send later.
		bool setControlEnabled(const std::string& _toolBarPageName, const std::string& _toolBarGroupName, const std::string& _toolBarSubGroupName, const std::string& _controlName, bool _isEnabled = true, bool _sendRequest = true);

		//! @brief Will send the currently buffered control enabled states to the frontend.
		bool sendCurrentControlEnabledStates(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// ToolBar

		

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private

	private:
		//! @brief Returns the control name according to the provided data.
		//! The string will have the following format: "_pageName + ':' + _groupName + ':' + _controlName".
		//! @param _pageName The ToolBar page name where the group is located at.
		//! @param _groupName The ToolBar group inside the page where the control is located at.
		//! @param _controlName The name of the control.
		inline std::string toolBarControlName(const std::string& _pageName, const std::string& _groupName, const std::string& _controlName) const { return _pageName + ":" + _groupName + ":" + _controlName; };
		//! @brief Returns the control name according to the provided data.
		//! The string will have the following format: "_pageName + ':' + _groupName + ':' + _subgroupName + ':' + _controlName".
		//! @param _pageName The ToolBar page name where the group is located at.
		//! @param _groupName The ToolBar group inside the page where the subgroup is located at.
		//! @param _sugroupName The ToolBar subgroup inside the group where the control is located at.
		//! @param _controlName The name of the control.
		inline std::string toolBarControlName(const std::string& _pageName, const std::string& _groupName, const std::string& _subgroupName, const std::string& _controlName) const { return _pageName + ":" + _groupName + ":" + _subgroupName + ":" + _controlName; };
		//! @brief Returns true if a control under the given name exists.
		inline bool controlExists(const std::string& _controlName) const { return m_controlsEnabledState.find(_controlName) != m_controlsEnabledState.end(); };

		std::map<std::string, bool> m_controlsEnabledState; //! @brief Contains the currently requested enabled state
		std::map<std::string, bool> m_controlsEnabledStateInFrontend; //! @brief Contains the current enabled state in the frontend

		FrontendAPI() {};
		~FrontendAPI() {};
	};

}
