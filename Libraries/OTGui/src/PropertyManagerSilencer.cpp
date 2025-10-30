// @otlicense
// File: PropertyManagerSilencer.cpp
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
#include "OTGui/PropertyManagerSilencer.h"

ot::PropertyManagerSilencer::PropertyManagerSilencer(PropertyManager* _manager) : m_manager(_manager) {
	OTAssertNullptr(m_manager);
	m_initial = m_manager->getSilenceNotifications();
	m_manager->setSilenceNotifications(true);
}

ot::PropertyManagerSilencer::PropertyManagerSilencer(PropertyManagerSilencer&& _other) noexcept :
	m_manager(std::move(_other.m_manager)), m_initial(std::move(_other.m_initial)) {
	_other.m_manager = nullptr;
}

ot::PropertyManagerSilencer::~PropertyManagerSilencer() {
	if (m_manager) {
		m_manager->setSilenceNotifications(true);
	}
}

ot::PropertyManagerSilencer& ot::PropertyManagerSilencer::operator = (PropertyManagerSilencer&& _other) noexcept {
	if (this != &_other) {
		m_manager = std::move(_other.m_manager);
		m_initial = std::move(_other.m_initial);
		_other.m_manager = nullptr;
	}

	return *this;
}
