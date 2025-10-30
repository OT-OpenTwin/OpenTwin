// @otlicense

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
