// @otlicense
// File: OTObject.cpp
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
#include "OTCore/Debugging/CoreDebug.h"
#include "OTCore/Object/OTObject.h"

ot::OTObject::OTObject(OTObject* _parentObject)
	: m_parentObject(_parentObject), m_destroyByParent(true)
{
	OT_CORE_OTOBJECT_DBG_PTR(this, "Creating OTObject");
}

ot::OTObject::~OTObject()
{
	OT_CORE_OTOBJECT_DBG_PTR(this, "Destroying OTObject");

	// Parent
	if (m_parentObject)
	{
		m_parentObject->forgetChildObject(this);
		m_parentObject = nullptr;
	}

	// Children
	std::list<OTObject*> children = std::move(m_childObjects);
	m_childObjects.clear();
	for (OTObject* child : children)
	{
		child->setParentOTObject(nullptr);
		if (child->getDestroyByParent())
		{
			OT_CORE_OTOBJECT_DBG_PTR(this, "Destroying child object: " << LogMsgPtr(child));
			delete child;
		}
	}

	OT_CORE_OTOBJECT_DBG_PTR(this, "OTObject destroyed");
}

void ot::OTObject::setParentOTObject(OTObject* _newParent)
{
	if (_newParent == m_parentObject)
	{
		return;
	}

	if (m_parentObject)
	{
		m_parentObject->forgetChildObject(this);
	}

	m_parentObject = _newParent;

	if (m_parentObject)
	{
		m_parentObject->registerChildObject(this);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Child management

void ot::OTObject::registerChildObject(OTObject* _object)
{
	if (std::find(m_childObjects.begin(), m_childObjects.end(), _object) == m_childObjects.end())
	{
		OT_CORE_OTOBJECT_DBG_PTR(this, "Registering child object: " << LogMsgPtr(_object));
		m_childObjects.push_back(_object);
	}
	else
	{
		OT_LOG_E("Child object already registered");
		OTAssert(false, "Child object already registered");
	}
}

void ot::OTObject::forgetChildObject(OTObject* _object)
{
	auto childIt = std::find(m_childObjects.begin(), m_childObjects.end(), _object);
	if (childIt != m_childObjects.end())
	{
		OT_CORE_OTOBJECT_DBG_PTR(this, "Forgetting child object: " << LogMsgPtr(_object));
		m_childObjects.erase(childIt);
	}
}