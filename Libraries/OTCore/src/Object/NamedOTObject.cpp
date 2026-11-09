// @otlicense
// File: NamedOTObject.cpp
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
#include "OTCore/Object/NamedOTObject.h"

ot::NamedOTObject::NamedOTObject(OTObject* _parentObject)
	: OTObject(_parentObject)
{}

ot::NamedOTObject::NamedOTObject(const std::string& _name, OTObject* _parentObject)
	: OTObject(_parentObject), m_name(_name)
{}

void ot::NamedOTObject::setOTObjectName(const std::string& _name)
{
	OT_CORE_OTOBJECT_DBG_PTR(this, "Setting object name to: " + _name);
}

ot::NamedOTObject* ot::NamedOTObject::findNamedChildObject(const std::string& _objectName, bool _recursive)
{
	return const_cast<NamedOTObject*>(
		static_cast<const NamedOTObject*>(this)->findNamedChildObject(_objectName, _recursive)
		);
}

const ot::NamedOTObject* ot::NamedOTObject::findNamedChildObject(const std::string& _objectName, bool _recursive) const
{
	const NamedOTObject* result = nullptr;
	for (OTObject* obj : this->getChildOTObjects())
	{
		const NamedOTObject* namedObj = dynamic_cast<const NamedOTObject*>(obj);
		if (namedObj == nullptr)
		{
			continue;
		}

		if (namedObj->getOTObjectName() == _objectName)
		{
			result = namedObj;
			break;
		}
		else if (_recursive)
		{
			result = namedObj->findNamedChildObject(_objectName, _recursive);
			if (result)
			{
				break;
			}
		}
	}

	return result;
}
