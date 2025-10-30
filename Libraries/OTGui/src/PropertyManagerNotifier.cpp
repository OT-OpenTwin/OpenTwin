// @otlicense
// File: PropertyManagerNotifier.cpp
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
#include "OTGui/PropertyManager.h"
#include "OTGui/PropertyManagerNotifier.h"

ot::PropertyManagerNotifier::PropertyManagerNotifier() : m_manager(nullptr) {

}

ot::PropertyManagerNotifier::PropertyManagerNotifier(const PropertyManagerNotifier& _other) : m_manager(_other.m_manager) {
	if (m_manager) {
		m_manager->addNotifier(this);
	}
}

ot::PropertyManagerNotifier::PropertyManagerNotifier(PropertyManagerNotifier&& _other) noexcept : m_manager(_other.m_manager) {
	if (m_manager) {
		m_manager->removeNotifier(&_other, false);
		m_manager->addNotifier(this);
	}
}

ot::PropertyManagerNotifier::~PropertyManagerNotifier() {
	if (m_manager) {
		m_manager->removeNotifier(this, false);
	}
}

ot::PropertyManagerNotifier& ot::PropertyManagerNotifier::operator=(const PropertyManagerNotifier& _other) {
	if (this != &_other) {
		if (m_manager) {
			m_manager->removeNotifier(this, false);
		}
		m_manager = _other.m_manager;
		if (m_manager) {
			m_manager->addNotifier(this);
		}
	}

	return *this;
}

ot::PropertyManagerNotifier& ot::PropertyManagerNotifier::operator=(PropertyManagerNotifier&& _other) noexcept {
	if (this != &_other) {
		if (m_manager) {
			m_manager->removeNotifier(this, false);
		}
		m_manager = std::move(_other.m_manager);
		if (m_manager) {
			m_manager->removeNotifier(&_other, false);
			m_manager->addNotifier(this);
		}
	}

	return *this;
}
