//! @file Foundation.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date February 2021
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTCore/Logger.h"
#include "OTCommunication/CommunicationTypes.h"
#include "OTCommunication/ServiceLogNotifier.h"
#include "OTServiceFoundation/Foundation.h"
#include "OTServiceFoundation/ApplicationBase.h"
#include "OTServiceFoundation/ExternalServicesComponent.h"

// std header
#include <fstream>
#include <algorithm>

namespace ot {
	namespace intern {

		//! @brief This function is used to wrap the code that is required to forward a message to the dispatch in the external services component
		const char* dispatchActionWrapper(const std::string& _json, const std::string& _senderUrl, ot::MessageType _messageType) {
			std::string response;
			try {
				response = ot::intern::ExternalServicesComponent::instance().dispatchAction(_json, _senderUrl, _messageType);
			}
			catch (const std::exception& _e) {
				OT_LOG_EAS(_e.what());

				response = OT_ACTION_RETURN_INDICATOR_Error;
				response.append(_e.what());
			}
			catch (...) {
				OT_LOG_EAS("[FATAL] Unknown error occured. Shutting down...");
				exit(ot::AppExitCode::UnknownError);
			}

			// Copy the return value.The memory of this value will be deallocated in the deallocateData function
			char* retVal = new char[response.length() + 1];
			strcpy_s(retVal, response.length() + 1, response.c_str());

			return retVal;
		}
	}
}

const char* ot::foundation::performAction(const std::string& _json, const std::string& _senderIP)
{
	return intern::dispatchActionWrapper(_json, _senderIP, ot::EXECUTE);
};

const char* ot::foundation::performActionOneWayTLS(const std::string& _json, const std::string& _senderIP)
{
	return intern::dispatchActionWrapper(_json, _senderIP, ot::EXECUTE_ONE_WAY_TLS);
};

const char* ot::foundation::queueAction(const std::string& _json, const std::string& _senderIP)
{
	return intern::dispatchActionWrapper(_json, _senderIP, ot::QUEUE);
};

const char* ot::foundation::getServiceURL(void)
{
	try {
		std::string serviceURL = ot::intern::ExternalServicesComponent::instance().application()->getServiceURL();

		char* retVal = new char[serviceURL.length() + 1];
		strcpy_s(retVal, serviceURL.length() + 1, serviceURL.c_str());
		return retVal;
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
		exit(ot::AppExitCode::GeneralError);
	}
	char* retVal = new char[1] { 0 };
	return retVal;
}

int ot::foundation::init(const std::string& _localDirectoryServiceURL, const std::string& _ownURL, const std::string& _localSessionServiceURL, const std::string& _sessionID, ApplicationBase* _application, bool _explicitDebug) {
	try {
		// Setup logger
#if defined(_DEBUG) || defined(OT_RELEASE_DEBUG)
		bool useDebug = true;
#else
		bool useDebug = _explicitDebug;
#endif

		if (useDebug) {
			if (_application) ot::ServiceLogNotifier::initialize(_application->getServiceName(), "", true);
			else ot::ServiceLogNotifier::initialize("<NO APPLICATION>", "", true);
		}
		else {
			if (_application) ot::ServiceLogNotifier::initialize(_application->getServiceName(), "", false);
			else ot::ServiceLogNotifier::initialize("<NO APPLICATION>", "", false);
		}

		// The following code is used to make the service lauchable in debug mode in the editor when the session service is requesting the service to start
		// In addition, if an empty siteID is passed to the service, it also reads its information from the config file. This allows for debugging services
		// which are built in release mode.
		if (useDebug) {
			if (_application) {
				// Get file path
				std::string deplyomentPath = _application->deploymentPath();
				if (deplyomentPath.empty()) return -20;

				std::string data = _application->getServiceName();
				std::transform(data.begin(), data.end(), data.begin(),
					[](unsigned char c) { return std::tolower(c); });

				deplyomentPath.append(data + ".cfg");

				// Read file
				std::ifstream stream(deplyomentPath);
				char inBuffer[512];
				stream.getline(inBuffer, 512);
				std::string info(inBuffer);

				if (info.empty()) {
					OT_LOG_EA("Configuration not found");
					assert(0);
					return -21;
				}
				// Parse doc
				JsonDocument params;
				params.fromJson(info);

				OT_LOG_I("Application parameters were overwritten by configuration file: " + deplyomentPath);

				std::string actualServiceURL = ot::json::getString(params, OT_ACTION_PARAM_SERVICE_URL);
				std::string actualSessionServiceURL = ot::json::getString(params, OT_ACTION_PARAM_SESSION_SERVICE_URL);
				std::string actualLocalDirectoryServiceURL = ot::json::getString(params, OT_ACTION_PARAM_LOCALDIRECTORY_SERVICE_URL);
				std::string actualSessionID = ot::json::getString(params, OT_ACTION_PARAM_SESSION_ID);
				ot::serviceID_t actualServiceID = static_cast<ot::serviceID_t>(ot::json::getUInt(params, OT_ACTION_PARAM_SERVICE_ID));
				// Initialize the service with the parameters from the file

				int startupResult = intern::ExternalServicesComponent::instance().startup(_application, actualLocalDirectoryServiceURL, actualServiceURL);
				if (startupResult != 0) {
					return startupResult;
				}

				std::string initResult = intern::ExternalServicesComponent::instance().init(actualSessionServiceURL, actualSessionID, actualServiceID, _explicitDebug);
				if (initResult != OT_ACTION_RETURN_VALUE_OK) {
					return -22;
				}
				else {
					return 0;
				}
			}
			else {
				return -23;
			}
		}
		else {
			return intern::ExternalServicesComponent::instance().startup(_application, _localDirectoryServiceURL, _ownURL);
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
		exit(ot::AppExitCode::GeneralError);
	}
	catch (...) {
		OT_LOG_EA("Unknown error");
		exit(ot::AppExitCode::UnknownError);
	}
}
