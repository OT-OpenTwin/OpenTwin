// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

// std header
#include <string>

namespace ot
{
	namespace Gateway
	{

		enum class GatewayActionType
		{
			Unknown        = 0,
			Ping           = 1,
			Shutdown       = 2,
			SendToService  = 3,
			QueueToService = 4
		};
		std::string toString(GatewayActionType _action);
		GatewayActionType stringToGatewayActionType(const std::string& _action);

	}
}