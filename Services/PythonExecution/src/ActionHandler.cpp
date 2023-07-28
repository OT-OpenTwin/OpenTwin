#include "ActionHandler.h"
#include "OpenTwinCommunication/ActionTypes.h"


#pragma once

ActionHandler::ActionHandler(std::string urlMasterService)
	:_urlMasterService(urlMasterService)
{
	_handlingFunction[OT_ACTION_CMD_PYTHON_Initialization] = std::bind(&ActionHandler::Initialize, this, std::placeholders::_1);
	_handlingFunction[OT_ACTION_CMD_ShutdownRequestedByService] = std::bind(&ActionHandler::ShutdownProcess, this, std::placeholders::_1);
}

const char* ActionHandler::Handle(const char* json, const char* senderIP)
{
	std::lock_guard<std::mutex> lock(_mutex);
	
	char* returnValue = new char{0};
	OT_rJSON_doc doc = ot::rJSON::fromJSON(json);
	std::string senderURL(senderIP);
	
	if (senderURL != _urlMasterService)
	{
		SendAccessDeniedMessage(senderURL);
	}
	else
	{
		std::string action = ot::rJSON::getString(doc, OT_ACTION_MEMBER);
		if (_handlingFunction.find(action) != _handlingFunction.end())
		{
			auto function = _handlingFunction[action];
			try
			{
				function(doc);
			}
			catch (std::exception& e)
			{
				//Exit? vorher noch ne message zurück?
			}
		}
		else
		{
			assert(0); // Action not supported
		}
	}
		
	


    return returnValue;
}

void ActionHandler::SendAccessDeniedMessage(const std::string& senderURL)
{

}

void ActionHandler::Initialize(OT_rJSON_doc& doc)
{
		
}

void ActionHandler::ShutdownProcess(OT_rJSON_doc& doc)
{
	exit(1);
}
