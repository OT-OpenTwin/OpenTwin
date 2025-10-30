// @otlicense

// OpenTwin header
#include "OTCommunication/ActionHandleConnectorContainer.h"

ot::ActionHandleConnectorContainer::ActionHandleConnectorContainer(ActionHandleConnectorContainer&& _other) noexcept {
	this->operator=(std::move(_other));
}

ot::ActionHandleConnectorContainer::~ActionHandleConnectorContainer() {
	this->clear();
}

ot::ActionHandleConnectorContainer& ot::ActionHandleConnectorContainer::operator=(ActionHandleConnectorContainer&& _other) noexcept {
	if (this != &_other) {
		this->clear();

		m_connectors = std::move(_other.m_connectors);
		_other.m_connectors.clear();
	}

	return *this;
}

void ot::ActionHandleConnectorContainer::store(std::shared_ptr<ActionHandleConnector>&& _connector) {
	m_connectors.push_back(std::move(_connector));
}

void ot::ActionHandleConnectorContainer::store(const std::shared_ptr<ActionHandleConnector>& _connector) {
	m_connectors.push_back(_connector);
}

void ot::ActionHandleConnectorContainer::clear() {
	for (auto& connector : m_connectors) {
		connector->removeFromDispatcher();
	}
	m_connectors.clear();
}