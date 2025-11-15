// @otlicense
// File: BasicEntityInformation.h
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
#include "OTCore/Serializable.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_CORE_API_EXPORT BasicEntityInformation : public Serializable {
		OT_DECL_DEFCOPY(BasicEntityInformation)
		OT_DECL_DEFMOVE(BasicEntityInformation)
	public:
		BasicEntityInformation();
		explicit BasicEntityInformation(UID _entityId, UID _entityVersion);
		explicit BasicEntityInformation(const std::string& _entityName, UID _entityId, UID _entityVersion);
		virtual ~BasicEntityInformation() = default;

		bool operator == (const BasicEntityInformation& _other) const;
		bool operator != (const BasicEntityInformation& _other) const;

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setEntityName(const std::string& _entityName) { m_entityName = _entityName; };
		void setEntityName(std::string&& _entityName) { m_entityName = std::move(_entityName); };
		const std::string& getEntityName(void) const { return m_entityName; };

		void setEntityID(UID _entityId) { m_entityId = _entityId; };
		UID getEntityID(void) const { return m_entityId; };

		void setEntityVersion(UID _entityVersion) { m_entityVersion = _entityVersion; };
		UID getEntityVersion(void) const { return m_entityVersion; };

	private:
		std::string m_entityName; //! @brief Name of entity that is displayed in the widget view.
		UID m_entityId; //! @brief ID of the entity that is displayed in the widget view.
		UID m_entityVersion; //! @brief Version of the entity that is displayed in the widget view.
	};

}