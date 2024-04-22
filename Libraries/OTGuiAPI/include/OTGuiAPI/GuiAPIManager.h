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

	class OT_GUIAPI_API_EXPORT GuiAPIManager {
		OT_DECL_NOCOPY(GuiAPIManager)
	public:
		static GuiAPIManager& instance(void);

		void frontendConnected(const ServiceBase& _serviceInformation);
		void frontendDisconnected(void);
		bool isFrontendConnected(void) const { return !(m_frontend.serviceURL().empty()); };

		bool sendQueuedRequestToFrontend(const std::string& _receiverUrl, const std::string& _message);

	private:
		ServiceBase m_frontend;

		GuiAPIManager();
		~GuiAPIManager();
	};

}