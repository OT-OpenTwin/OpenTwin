//! @file CommunicationAPI.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/BasicServiceInformation.h"
#include "OTFrontendConnectorAPI/OTFrontendConnectorAPIExport.h"

// std header
#include <string>

namespace ot {
	class OT_FRONTEND_CONNECTOR_API_EXPORT CommunicationAPI {
	public:
		static bool sendExecute(const std::string& _receiverUrl, const std::string& _message, std::string& _response);
		static bool sendQueue(const std::string& _receiverUrl, const std::string& _message);
		static bool sendExecuteToService(const BasicServiceInformation& _serviceInfo, const std::string& _message, std::string& _response);
		static bool sendQueueToService(const BasicServiceInformation& _serviceInfo, const std::string& _message);

	protected:
		virtual bool sendExecuteAPI(const std::string& _receiverUrl, const std::string& _message, std::string& _response) = 0;
		virtual bool sendQueueAPI(const std::string& _receiverUrl, const std::string& _message) = 0;
		virtual bool sendExecuteToServiceAPI(const BasicServiceInformation& _serviceInfo, const std::string& _message, std::string& _response) = 0;
		virtual bool sendQueueToServiceAPI(const BasicServiceInformation& _serviceInfo, const std::string& _message) = 0;

		CommunicationAPI();
		virtual ~CommunicationAPI();

	private:
		static CommunicationAPI* instance();
		static CommunicationAPI*& getInstanceReference();
	};
}