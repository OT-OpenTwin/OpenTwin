// @otlicense
// File: ActionDispatcher.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
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