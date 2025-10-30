// @otlicense
// File: CopyInformation.h
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
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/WidgetViewBase.h"
#include "OTGui/CopyEntityInformation.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	//! @brief The CopyInformation contains all the required information about a copy/pate request.
	class OT_GUI_API_EXPORT CopyInformation : public Serializable {
	public:
		//! @brief Create CopyInformation from raw string.
		//! If the string can not be deserialized the string will be set as the create objects raw data.
		//! @param _rawString Raw string to parse and apply.
		//! @return Deserialized CopyInformation.
		static CopyInformation fromRawString(const std::string& _rawString);

		//! @brief Get JSON object member key that is used to store the copy version.
		static std::string getCopyVersionJsonKey(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Constructor

		CopyInformation();
		CopyInformation(const ConstJsonObject& _jsonObject);
		CopyInformation(const CopyInformation& _other);
		CopyInformation(CopyInformation&& _other) noexcept;
		virtual ~CopyInformation() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operator

		CopyInformation& operator = (const CopyInformation& _other);
		CopyInformation& operator = (CopyInformation&& _other) noexcept;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Helper

		//! @brief Check all members for existance and type before deserializing.
		//! This will ensure that the object is valid before deserializing.
		//! @param _object JSON object.
		//! @return False if any member is missing or has an incorrect type.
		bool tryToSetFromJson(const ot::ConstJsonObject& _object);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		int getCopyVersion(void) const { return 1; };

		void setRawData(const std::string& _raw) { m_rawData = _raw; };
		const std::string& getRawData(void) const { return m_rawData; };

		void setOriginProjectName(const std::string& _projectName) { m_originProjectName = _projectName; };
		const std::string& getOriginProjectName(void) const { return m_originProjectName; };

		void setOriginViewInfo(const WidgetViewBase& _info) { m_originViewInfo = _info; };
		const WidgetViewBase& getOriginViewInfo(void) const { return m_originViewInfo; };

		void setDestinationViewInfo(const WidgetViewBase& _info) { m_destinationViewInfo = _info; };
		const WidgetViewBase& getDestinationViewInfo(void) const { return m_destinationViewInfo; };

		void setDestinationScenePos(const Point2DD& _pos) { m_destinationScenePos = _pos; m_destinationScenePosSet = true; };
		const Point2DD& getDestinationScenePos(void) const { return m_destinationScenePos; };
		bool getDestinationScenePosSet(void) const { return m_destinationScenePosSet; };

		void addEntity(UID _uid, const std::string& _name, const std::string& _rawData = std::string());
		void addEntity(const CopyEntityInformation& _entityInfo) { m_entities.push_back(_entityInfo); };
		void setEntities(const std::list<CopyEntityInformation>& _entities) { m_entities = _entities; };
		std::list<CopyEntityInformation>& getEntities(void) { return m_entities; };
		const std::list<CopyEntityInformation>& getEntities(void) const { return m_entities; };
		void clearEntities(void) { m_entities.clear(); };

	private:
		std::string m_rawData;

		std::string m_originProjectName;
		
		WidgetViewBase m_originViewInfo;
		WidgetViewBase m_destinationViewInfo;
		
		Point2DD m_destinationScenePos;
		bool m_destinationScenePosSet;

		std::list<CopyEntityInformation> m_entities;
	};

}