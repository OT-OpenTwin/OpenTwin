// @otlicense

//! @file DebugHelper.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Open Twin header
#include "OTCore/ServiceDebugInformation.h"

namespace ot {

	//! @class DebugHelper
	//! @brief This class provides helper methods for debugging purposes.
	class OT_CORE_API_EXPORT DebugHelper {
		OT_DECL_STATICONLY(DebugHelper)
	public:
		static std::string getSetupCompletedMessagePrefix() { return "Service Setup Completed: "; };

		//! @brief This method is used to log the service initialization completed message.
		//! @param _serviceInfo The service information of the service that has completed its initialization.
		static void serviceSetupCompleted(const ot::ServiceDebugInformation& _serviceInfo);
	};
}