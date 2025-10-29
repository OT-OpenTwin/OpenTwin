// @otlicense

//! @file ModelAPIManager.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/CommunicationTypes.h"
#include "OTModelAPI/OTModelAPIExport.h"

// std header
#include <string>

namespace ot {

	class OT_MODELAPI_API_EXPORT ModelAPIManager {
		OT_DECL_NOCOPY(ModelAPIManager)
		OT_DECL_NOMOVE(ModelAPIManager)
	public:
		static void setModelServiceURL(const std::string& _url);
		static const std::string& getModelServiceURL(void);

		static bool sendToModel(MessageType _messageType, const std::string& _message, std::string& _response);
		static bool sendToModel(MessageType _messageType, const JsonDocument& _doc, std::string& _response);

	private:
		static ModelAPIManager& instance(void);

		ModelAPIManager();
		~ModelAPIManager();

		std::string m_serviceUrl;
	};

}