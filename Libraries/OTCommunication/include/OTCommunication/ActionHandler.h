//! @file ActionHandler.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCommunication/CommunicationAPIExport.h"

namespace ot {

	class OT_COMMUNICATION_API_EXPORT ActionHandler {
	public:
		ActionHandler();
		ActionHandler(const ActionHandler& _other) = default;
		ActionHandler(ActionHandler&& _other) = default;
		virtual ~ActionHandler();
		ActionHandler& operator = (const ActionHandler& _other) = default;
		ActionHandler& operator = (ActionHandler&& _other) = default;
	};

}