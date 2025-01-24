//! @file ActionDispatcher.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCommunication/ActionDispatcherBase.h"

namespace ot {

	//! @class ActionDispatcher
	//! @brief The ActionDispatcher is used to dispatch inbound messages and forward them to their corresponding [\ref ActionHandleConnector "Action Handler"].
	class OT_COMMUNICATION_API_EXPORT ActionDispatcher : public ActionDispatcherBase {
		OT_DECL_NOCOPY(ActionDispatcher)
	public:
		//! @brief Returns the global singleton instance
		static ActionDispatcher& instance(void);

	private:
		ActionDispatcher() = default;
		virtual ~ActionDispatcher() = default;
	};
}