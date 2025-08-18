//! \file dllmain.cpp
//! \author Alexander Kuester (alexk95)
//! \date January 2021
// ###########################################################################################################################################################################################################################################################################################################################

// Service header
#include "AppBase.h"

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCore/String.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/IpConverter.h"
#include "OTCommunication/ActionDispatcher.h"

// std header
#include <string>
#include <Windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

namespace ot {
	namespace intern {
		
		std::string dispatchActionWrapper(const char* _json, ot::MessageType _messageType) {
			try {
				ot::JsonDocument actionDoc;
				actionDoc.fromJson(_json);
				if (actionDoc.IsObject()) {

					std::string action = ot::json::getString(actionDoc, OT_ACTION_MEMBER);

					// Messages that can be processed directly
					if (action == OT_ACTION_CMD_Ping) {
						return OT_ACTION_CMD_Ping;
					}
					else if (action == OT_ACTION_CMD_ServiceShutdown) {
						OT_LOG_D("Shutting down logger service");
						exit(ot::AppExitCode::Success);
					}
					else if (action == OT_ACTION_CMD_ServiceEmergencyShutdown) {
						OT_LOG_D("Logger service emergency shutdown");
						exit(ot::AppExitCode::EmergencyShutdown);
					}
					else {
						bool handlerFound = false;
						std::string result = ot::ActionDispatcher::instance().dispatch(action, actionDoc, handlerFound, _messageType);
						if (handlerFound) {
							return result;
						}
						else {
							return OT_ACTION_RETURN_UnknownAction;
						}
					}
					
				}
				else {
					return OT_ACTION_RETURN_INDICATOR_Error "Ivalid action format";
				}
			}
			catch (const std::exception& _e) {
				OT_LOG_E(_e.what());
				return OT_ACTION_RETURN_INDICATOR_Error + std::string(_e.what());
			}
			catch (...) {
				OT_LOG_E("Unknown error");
				return OT_ACTION_RETURN_UnknownError;
			}
		}

		char* dispatchAction(const char* _json, ot::MessageType _messageType) {
			return ot::String::getCStringCopy(dispatchActionWrapper(_json, _messageType));
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Microservice Calls

extern "C"
{
	_declspec(dllexport) int init(const char * _unused1, const char * _serviceURL, const char * _unused2, const char * _unused3)
	{
		try {
			if (_serviceURL == nullptr) {
				OTAssert(0, "Service URL not set");
				exit(ot::AppExitCode::ServiceUrlMissing);
			}

			// Initialize log dispatcher
#ifdef _DEBUG
			ot::LogDispatcher::initialize(OT_INFO_SERVICE_TYPE_LOGGER, false);
#else
			ot::LogDispatcher::initialize(OT_INFO_SERVICE_TYPE_LOGGER, false);
#endif

			AppBase::instance().setServiceURL(_serviceURL);

			// Initialize log message cache
			AppBase::instance().updateBufferSizeFromLogFlags(ot::LogDispatcher::instance().getLogFlags());

			// Set log flags for the logger service
#ifdef _DEBUG
			ot::LogDispatcher::instance().setLogFlags(ot::INFORMATION_LOG | ot::DETAILED_LOG | ot::WARNING_LOG | ot::ERROR_LOG);
			ot::LogDispatcher::instance().addReceiver(&AppBase::instance());
#else
			ot::LogDispatcher::instance().setLogFlags(ot::NO_LOG);
#endif // _DEBUG

			OT_LOG_D("LoggerService starting");
		}
		catch (const std::exception & _e) {
			OutputDebugStringA("ERROR: ");
			OutputDebugStringA(_e.what());
			OutputDebugStringA("\n");
			exit(ot::AppExitCode::GeneralError);
		}
		catch (...) {
			OutputDebugStringA("ERROR: Unknown error");
			exit(ot::AppExitCode::UnknownError);
		}
		return ot::AppExitCode::Success;
	};

	_declspec(dllexport) const char *getServiceURL(void) {
		return ot::String::getCStringCopy(AppBase::instance().getServiceURL());
	};

	_declspec(dllexport) const char *performAction(const char * _json, const char * _senderIP) {
		return ot::intern::dispatchAction(_json, ot::EXECUTE);
	};

	_declspec(dllexport) const char *performActionOneWayTLS(const char * _json, const char * _senderIP) {
		return ot::intern::dispatchAction(_json, ot::EXECUTE_ONE_WAY_TLS);
	};

	// The logger service is not queueing actions
	_declspec(dllexport) const char *queueAction(const char * _json, const char * _senderIP) {
		return ot::intern::dispatchAction(_json, ot::QUEUE);
	};

	_declspec(dllexport) void deallocateData(const char * _data) {
		// Destroy data (e.g. return value form performAction)
		if (_data != nullptr) {
			delete[] _data;
		}
	};
}
