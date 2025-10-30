// @otlicense
// File: PropertyReadCallbackNotifier.cpp
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

// OT header
#include "OTSystem/OTAssert.h"
#include "OTGui/PropertyManager.h"
#include "OTGui/PropertyReadCallbackNotifier.h"

ot::PropertyReadCallbackNotifier::PropertyReadCallbackNotifier(ManagerGetType _getManager, const std::string& _propertyPath, std::optional<CallbackType> _method) :
	m_method(_method), m_propertyPath(_propertyPath)
{
	m_manager = _getManager();
	OTAssertNullptr(m_manager);
	m_manager->addReadCallbackNotifier(this);
}

ot::PropertyReadCallbackNotifier::~PropertyReadCallbackNotifier() {
	if (m_manager) {
		m_manager->removeReadCallbackNotifier(this);
	}
}

void ot::PropertyReadCallbackNotifier::call(const std::string& _propertyGroupName, const std::string& _propertyName) {
	if (m_method.has_value()) {
		m_method.value()(_propertyGroupName, _propertyName);
	}
}
