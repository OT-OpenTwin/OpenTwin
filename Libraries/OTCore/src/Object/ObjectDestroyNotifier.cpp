// @otlicense
// File: ObjectDestroyNotifier.cpp
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
#include "OTCore/Object/ObjectDestroyNotifier.h"
#include "OTCore/Object/ObjectWithDestroyNotifier.h"

ot::ObjectDestroyNotifier::ObjectDestroyNotifier()
{}

ot::ObjectDestroyNotifier::~ObjectDestroyNotifier()
{}

void ot::ObjectDestroyNotifier::addRegisteredObject(ObjectWithDestroyNotifier * _object)
{
	OTAssertNullptr(_object);
	OTAssert(std::find(m_registeredObjects.begin(), m_registeredObjects.end(), _object) == m_registeredObjects.end(), "Object already registered");
	m_registeredObjects.push_back(_object);
}

void ot::ObjectDestroyNotifier::removeRegisteredObject(ObjectWithDestroyNotifier * _object)
{
	OTAssertNullptr(_object);
	OTAssert(std::find(m_registeredObjects.begin(), m_registeredObjects.end(), _object) != m_registeredObjects.end(), "Object not registered");
	m_registeredObjects.remove(_object);
}
