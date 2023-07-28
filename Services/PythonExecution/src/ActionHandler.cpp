#include "ActionHandler.h"
#include "OpenTwinCommunication/ActionTypes.h"
#pragma warning(disable : 4996)

#pragma once

ActionHandler::ActionHandler(const std::string& urlMasterService)
	:_urlMasterService(urlMasterService)
{
	_handlingFunction[OT_ACTION_CMD_ServiceShutdown] = std::bind(&ActionHandler::ShutdownProcess, this, std::placeholders::_1);
}

const char* ActionHandler::Handle(const char* json, const char* senderIP)
{
	std::lock_guard<std::mutex> lock(_mutex);
	std::string returnMessage;
	OT_rJSON_doc doc = ot::rJSON::fromJSON(json);
	if (ot::rJSON::memberExists(doc, OT_ACTION_PARAM_SENDER_URL))
	{
		std::string senderURL = ot::rJSON::getString(doc, OT_ACTION_PARAM_SENDER_URL);
		
		if (senderURL != _urlMasterService)
		{
			returnMessage = "Access denied.";
		}
		else
		{
			std::string action = ot::rJSON::getString(doc, OT_ACTION_MEMBER);
			if (_handlingFunction.find(action) != _handlingFunction.end())
			{
				auto function = _handlingFunction[action];
				try
				{
					returnMessage = function(doc);
				}
				catch (std::exception& e)
				{
					returnMessage = "Exception: " + std::string(e.what());
					//Exit? vorher noch ne message zurück?
				}
			}
			else
			{
				returnMessage = "Action " + action + " not supported.";
				assert(0); // Action not supported
			}
		}
	}
	
	char* returnValue = new char[returnMessage.size() + 1];
	std::strcpy(returnValue, returnMessage.c_str());

    return returnValue;
}

std::string ActionHandler::ShutdownProcess(OT_rJSON_doc& doc)
{
	exit(1);
}
