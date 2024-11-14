//! @file GuiAPIManager.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCore/ServiceBase.h"
#include "OTGuiAPI/OTGuiAPIAPIExport.h"

namespace ot {

	//! @class GuiAPIManager
	//! @brief The GuiAPIManager singleton manages the frontend information and provides a message sending method.
	class OT_GUIAPI_API_EXPORT GuiAPIManager {
		OT_DECL_NOCOPY(GuiAPIManager)
	public:
		//! @brief Returns global instance.
		static GuiAPIManager& instance(void);

		//! @brief Apply frontend information, enable message sending and forward frontend connected state to other GuiAPI managers.
		//! @param _serviceInformation Frontend service information to set.
		void frontendConnected(const ServiceBase& _serviceInformation);

		//! @brief Clear frontend information, disable message sending and forward frontend disconnected state to other GuiAPI managers.
		void frontendDisconnected(void);

		//! @brief Returns true if the frontend information was set
		bool isFrontendConnected(void) const { return !(m_frontend.getServiceURL().empty()); };

		//! @brief Send the provided message to the frontend.
		//! If no frontend is connected a warning log is generated and the method returns false.
		bool sendQueuedRequestToFrontend(const std::string& _message);

	private:
		ServiceBase m_frontend; //! @brief Current frontend information
		
		//! @brief Private constructor
		GuiAPIManager();

		//! @brief Private destructor
		~GuiAPIManager();
	};

}