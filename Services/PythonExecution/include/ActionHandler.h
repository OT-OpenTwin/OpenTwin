#pragma once
#include <mutex>
#include <map>
#include <functional>
#include "OpenTwinCore/rJSON.h"

class ActionHandler
{
public:
	ActionHandler(const std::string& urlMasterService);
	const char* Handle(const char* json, const char* senderIP);

private:
	using handlerMethods = std::function<std::string(OT_rJSON_doc& _doc)>;

	std::mutex _mutex;
	const std::string _urlMasterService;
	std::map<std::string, handlerMethods> _handlingFunction;

	std::string ShutdownProcess(OT_rJSON_doc& doc);
};
