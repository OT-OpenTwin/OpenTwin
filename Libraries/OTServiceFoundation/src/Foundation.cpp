// @otlicense
// File: Foundation.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTSystem/FileSystem/FileSystem.h"
#include "OTCore/Logging/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/CommunicationTypes.h"
#include "OTCommunication/ServiceLogNotifier.h"
#include "OTCommunication/Dispatch/ActionDispatcher.h"
#include "OTServiceFoundation/Foundation.h"
#include "OTServiceFoundation/ApplicationBase.h"
#include "OTServiceFoundation/ExternalServicesComponent.h"

// std header
#include <fstream>
#include <algorithm>

namespace ot
{
	namespace intern
	{

		//! @brief This function is used to wrap the code that is required to forward a message to the dispatch in the external services component
		const char* dispatchActionWrapper(const std::string& _json, const std::string& _senderUrl, ot::MessageType _messageType)
		{
			std::string response;
			try
			{
				response = ot::intern::ExternalServicesComponent::instance().dispatchAction(_json, _senderUrl, _messageType);
			}
			catch (const std::exception& _e)
			{
				OT_LOG_E(_e.what());

				response = ot::ReturnMessage::toJson(ot::ReturnMessage::Failed, _e.what());
			}
			catch (...)
			{
				OT_LOG_E("[FATAL] Unknown error occured. Shutting down...");
				exit(ot::AppExitCode::UnknownError);
			}

			// Copy the return value.The memory of this value will be deallocated in the deallocateData function
			char* retVal = new char[response.length() + 1];
			strcpy_s(retVal, response.length() + 1, response.c_str());

			return retVal;
		}

		void debugInitializeWorker(ot::ServiceInitData&& _data, bool _explicitDebug)
		{
			try
			{
				ot::JsonDocument iniDoc;
				iniDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Init, iniDoc.GetAllocator()), iniDoc.GetAllocator());
				iniDoc.AddMember(OT_ACTION_PARAM_IniData, JsonObject(_data, iniDoc.GetAllocator()), iniDoc.GetAllocator());
				ot::ActionDispatcher::instance().dispatch(OT_ACTION_CMD_Init, iniDoc, ot::EXECUTE);
			}
			catch (const std::exception& _e)
			{
				OT_LOG_E(_e.what());
				exit(ot::AppExitCode::GeneralError);
			}
			catch (...)
			{
				OT_LOG_E("Unknown error");
				exit(ot::AppExitCode::UnknownError);
			}
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

const char* ot::foundation::getServiceURL()
{
	try
	{
		std::string serviceURL = ot::intern::ExternalServicesComponent::instance().application()->getServiceURL();

		char* retVal = new char[serviceURL.length() + 1];
		strcpy_s(retVal, serviceURL.length() + 1, serviceURL.c_str());
		return retVal;
	}
	catch (const std::exception& _e)
	{
		OT_LOG_E(_e.what());
		exit(ot::AppExitCode::GeneralError);
	}
	char* retVal = new char[1] { 0 };
	return retVal;
}

int ot::foundation::init(const std::string& _ownURL, const std::string& _ownName,
	const std::string& _unused1, const std::string& _unused2, ApplicationBase* _application, bool _explicitDebug)
{
	try
	{
		if (!_application)
		{
			OT_LOG_E("Application pointer is null");
			exit(ot::AppExitCode::ArgumentMissingError);
		}

		if (_ownName.empty())
		{
			OT_LOG_E("Service name is empty");
			exit(ot::AppExitCode::ArgumentMissingError);
		}

		// Setup logger
#if defined(_DEBUG) || defined(OT_RELEASE_DEBUG)
		bool useDebug = true;
#else
		bool useDebug = _explicitDebug;
#endif

		ot::ServiceLogNotifier::initialize(_application->getServiceName(), "", useDebug);

		// The following code is used to make the service lauchable in debug mode in the editor when the session service is requesting the service to start
		// In addition, if an empty siteID is passed to the service, it also reads its information from the config file. This allows for debugging services
		// which are built in release mode.
		if (useDebug)
		{
			// Determine deployment path
			const std::string deplyomentPath = _application->deploymentPath();
			if (deplyomentPath.empty())
			{
				OT_LOG_E("Deployment path not found");
				exit(ot::AppExitCode::FileNotFound);
			}

			const std::string configFilePath = deplyomentPath + ot::String::toLower(_application->getServiceName()) + ".cfg";

			// Read file

			std::string configFile;
			if (!ot::FileSystem::readFile(configFilePath, configFile))
			{
				OT_LOG_E("Failed to read configuration file: " + configFilePath);
				exit(ot::AppExitCode::FileNotFound);
			}

			if (configFile.empty())
			{
				OT_LOG_E("Configuration not found");
				exit(ot::AppExitCode::FileNotFound);
			}
			// Parse doc
			JsonDocument configDoc;
			if (!configDoc.fromJson(configFile))
			{
				OT_LOG_E("Configuration not valid JSON");
				exit(ot::AppExitCode::InvalidArgumentError);
			}

			OT_LOG_I("Application parameters were overwritten by configuration file: " + configFilePath);

			const std::string actualServiceURL = ot::json::getString(configDoc, OT_ACTION_PARAM_SERVICE_URL);

			ot::ServiceInitData iniData;
			iniData.setFromJsonObject(ot::json::getObject(configDoc, OT_ACTION_PARAM_IniData));
			
			if (iniData.getServiceName() != _application->getServiceName())
			{
				OT_LOG_D("Updating service name to " + iniData.getServiceName());
				_application->setServiceName(iniData.getServiceName());
				ot::LogDispatcher::instance().setServiceName(_application->getServiceName());
			}

			// Initialize the service with the parameters from the file

			int startupResult = intern::ExternalServicesComponent::instance().startup(_application, actualServiceURL);
			if (startupResult != ot::AppExitCode::Success)
			{
				return startupResult;
			}

			// Use a thread to initialize the service since the init call might take some time and we want to return
			std::thread iniWorker(intern::debugInitializeWorker, std::move(iniData), _explicitDebug);
			iniWorker.detach();

			return ot::AppExitCode::Success;
		}
		else
		{
			if (_application->getServiceName() != _ownName)
			{
				OT_LOG_D("Updating service name to " + _ownName);
				_application->setServiceName(_ownName);
				ot::LogDispatcher::instance().setServiceName(_application->getServiceName());
			}
			return intern::ExternalServicesComponent::instance().startup(_application, _ownURL);
		}
	}
	catch (const std::exception& _e)
	{
		OT_LOG_E(_e.what());
		exit(ot::AppExitCode::GeneralError);
	}
	catch (...)
	{
		OT_LOG_E("Unknown error");
		exit(ot::AppExitCode::UnknownError);
	}
}
