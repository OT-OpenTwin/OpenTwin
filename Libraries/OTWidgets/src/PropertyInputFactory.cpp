// @otlicense
// File: PropertyInputFactory.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTGui/Property.h"
#include "OTWidgets/PropertyInput.h"
#include "OTWidgets/PropertyInputFactory.h"

ot::PropertyInputFactory& ot::PropertyInputFactory::instance(void) {
	static PropertyInputFactory g_instance;
	return g_instance;
}

ot::PropertyInput* ot::PropertyInputFactory::createInput(const std::string& _key) {
	PropertyInputFactory& factory = PropertyInputFactory::instance();
	auto it = factory.m_constructors.find(_key);
	if (it == factory.m_constructors.end()) {
		OT_LOG_E("PropertyInput \"" + _key + "\" not registered");
		return nullptr;
	}
	return it->second();
}

ot::PropertyInput* ot::PropertyInputFactory::createInput(const Property* _config) {
	OTAssertNullptr(_config);
	PropertyInput* inp = PropertyInputFactory::createInput(_config->getPropertyType());
	if (inp) {
		inp->setupFromConfiguration(_config);
	}
	return inp;
}

bool ot::PropertyInputFactory::registerPropertyInput(const std::string& _key, const PropertyInputConstructor& _constructor) {
	PropertyInputFactory& factory = PropertyInputFactory::instance();
	if (factory.m_constructors.find(_key) != factory.m_constructors.end()) {
		OT_LOG_E("PropertyInput \"" + _key + "\" already registered");
		return false;
	}
	factory.m_constructors.insert_or_assign(_key, _constructor);
	return true;
}