// @otlicense
// File: ActionHandleConnectorContainer.cpp
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