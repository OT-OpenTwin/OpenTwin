#pragma once
#include <mutex>
#include <map>
#include <functional>
#include "OpenTwinCore/rJSON.h"

class ActionHandler
{
public:
	ActionHandler(std::string urlMasterService);
	const char* Handle(const char* json, const char* senderIP);

private:
	using handlerMethods = std::function<void(OT_rJSON_doc& _doc)>;

	std::mutex _mutex;
	const std::string _urlMasterService;
	std::map<std::string, handlerMethods> _handlingFunction;

	void SendAccessDeniedMessage(const std::string& senderURL);

	void Initialize(OT_rJSON_doc& doc);
};
