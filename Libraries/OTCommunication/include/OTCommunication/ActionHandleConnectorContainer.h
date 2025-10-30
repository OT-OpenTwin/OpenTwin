// @otlicense
// File: ActionHandleConnectorContainer.h
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
#include "OTCommunication/ActionHandleConnector.h"

// std header
#include <list>

namespace ot {

	//! @brief The ActionHandleConnectorContainer class is a simple container to store ActionHandleConnector instances and to ensure that they are removed from the dispatcher when the container is destroyed.
	class OT_COMMUNICATION_API_EXPORT ActionHandleConnectorContainer {
		OT_DECL_NOCOPY(ActionHandleConnectorContainer)
	public:
		ActionHandleConnectorContainer() = default;
		ActionHandleConnectorContainer(ActionHandleConnectorContainer&& _other) noexcept;
		~ActionHandleConnectorContainer();

		ActionHandleConnectorContainer& operator=(ActionHandleConnectorContainer&& _other) noexcept;

		//! @brief Stores the given connector in the container. The container takes ownership of the connector.
		//! @param _connector The connector to store.
		void store(std::shared_ptr<ActionHandleConnector>&& _connector);

		//! @brief Stores the given connector in the container. The container takes shared ownership of the connector.
		//! @param _connector The connector to store.
		void store(const std::shared_ptr<ActionHandleConnector>& _connector);

		//! @brief Removes all connectors from the dispatcher and clears the container.
		void clear();

	private:

		std::list<std::shared_ptr<ActionHandleConnector>> m_connectors;
	};

}
