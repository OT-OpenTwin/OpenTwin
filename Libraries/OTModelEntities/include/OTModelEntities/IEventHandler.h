// @otlicense
#pragma once
#include <string>
#include <optional>
#include "OTCore/Python/PythonEventTypes.h"
class IEventHandler
{
public:
	virtual std::string getScriptName() = 0;
	virtual std::string getEnvironmentName() = 0;
	virtual std::optional<std::string>getEventHandlingFunction(PythonEventType _type, std::map<ot::UID, EntityBase*>& _entityMap) = 0;
};
