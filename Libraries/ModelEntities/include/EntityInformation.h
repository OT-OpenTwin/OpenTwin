// @otlicense
// File: EntityInformation.h
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
#include "OTCore/BasicEntityInformation.h"

// std header
#include <string>

class EntityBase;

namespace ot {

	class __declspec(dllexport) EntityInformation : public BasicEntityInformation {
	public:
		EntityInformation();
		EntityInformation(EntityBase* _entity);
		EntityInformation(const EntityInformation&) = default;
		virtual ~EntityInformation();

		EntityInformation& operator = (const EntityInformation&) = default;

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setEntityType(std::string _type) { m_type = _type; };
		const std::string& getEntityType(void) const { return m_type; };

		bool operator==(const EntityInformation& _other) const 
		{
			return BasicEntityInformation::operator==(_other) && _other.m_type == this->m_type;
		}
	private:
		std::string m_type;
	};

}