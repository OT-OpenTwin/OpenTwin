// @otlicense
// File: NamedOTObject.h
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

#pragma once

// OpenTwin header
#include "OTCore/Object/OTObject.h"

namespace ot
{

	//! @brief The NamedOTObject class is a subclass of OTObject that adds a name property to the object.
	class OT_CORE_API_EXPORT NamedOTObject : public OTObject
	{
		OT_DECL_NOCOPY(NamedOTObject)
		OT_DECL_NOMOVE(NamedOTObject)
	public:
		explicit NamedOTObject(OTObject* _parentObject = (OTObject*)nullptr);
		explicit NamedOTObject(const std::string& _name, OTObject* _parentObject = (OTObject*)nullptr);
		virtual ~NamedOTObject() = default;

		void setOTObjectName(const std::string& _name);
		const std::string& getOTObjectName() const { return m_name; };

		NamedOTObject* findNamedChildObject(const std::string& _objectName, bool _recursive = true);
		const NamedOTObject* findNamedChildObject(const std::string& _objectName, bool _recursive = true) const;

	private:
		std::string m_name;

	};

}