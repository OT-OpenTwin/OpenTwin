//! \file dllmain.cpp
//! \author Alexander Kuester (alexk95)
//! \date January 2021
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "AppBase.h"
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/IpConverter.h"
#include "OTCommunication/ActionDispatcher.h"

// std header
#include <Windows.h>
#include <string>

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
		
		//! \brief Creates a C-String copy of the provided C++ String
		char* getCStringCopy(const std::string& _string) {
			char* retval = new char[_string.length() + 1];
			strcpy_s(retval, _string.length() + 1, _string.c_str());
			return retval;
		}

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
						exit(0);
					}
					else if (action == OT_ACTION_CMD_ServiceEmergencyShutdown) {
						exit(-100);
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
				return OT_ACTION_RETURN_INDICATOR_Error + std::string(_e.what());
			}
			catch (...) {
				return OT_ACTION_RETURN_UnknownError;
			}
		}

		char* dispatchAction(const char* _json, ot::MessageType _messageType) {
			return getCStringCopy(dispatchActionWrapper(_json, _messageType));
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
				return -1;
			}

#ifdef _DEBUG
			ot::LogDispatcher::initialize(OT_INFO_SERVICE_TYPE_LOGGER, true);
			ot::LogDispatcher::instance().setLogFlags(ot::INFORMATION_LOG | ot::DETAILED_LOG | ot::WARNING_LOG | ot::ERROR_LOG);
#else
			ot::LogDispatcher::initialize(OT_INFO_SERVICE_TYPE_LOGGER, false);
			ot::LogDispatcher::instance().setLogFlags(ot::NO_LOG);
#endif // _DEBUG
			OT_LOG_D("LoggerService starting");

			AppBase::instance().setServiceURL(_serviceURL);
		}
		catch (const std::exception & _e) {
			OutputDebugStringA("ERROR: ");
			OutputDebugStringA(_e.what());
			OutputDebugStringA("\n");
			return 1;
		}
		catch (...) {
			OutputDebugStringA("ERROR: Unknown error");
			return 2;
		}
		return 0;
	};

	_declspec(dllexport) const char *getServiceURL(void) {
		return ot::intern::getCStringCopy(AppBase::instance().getServiceURL());
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
