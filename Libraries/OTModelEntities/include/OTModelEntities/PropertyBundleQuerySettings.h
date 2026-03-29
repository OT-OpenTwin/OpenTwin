// @otlicense
// File: PropertyBundleQuerySettings.h
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
#include "OTCore/QueryDescription/ValueComparisonDescription.h"
#include "OTModelEntities/PropertyBundle.h"

namespace ot {

	class OT_MODELENTITIES_API_EXPORT PropertyBundleQuerySettings : public PropertyBundle
	{
	public:
		virtual void setProperties(EntityBase* _entity) override;
		virtual bool updatePropertyVisibility(EntityBase* _entity) override;
		void setQueryDefinitions(const std::list<std::string>& _queryOptions);
		void reload(EntityBase* _entity);

		std::list<ot::ValueComparisonDescription> getValueComparisonDefinitions(const EntityBase* _entity) const;
		ValueComparisonDescription getValueComparisonDefinition(const EntityBase* _entity, uint32_t _queryIndex) const { return getValueComparisonDefinition(_entity, getGroupName(_queryIndex)); };

		//! @brief Checks if parameter that determine the query have changed.
		bool needsUpdate(const EntityBase* _entity) const;

		int32_t getNumberOfQueries(const EntityBase* _entity) const;

		void updateQuerySettings(EntityBase* _entity, const std::list<std::string>& _queryOptions);

		const std::string& getGroupQuerySettingsName() const { return m_groupQuerySettings; };

	private:
		const std::string m_groupQuerySettings = "Query settings";
		const std::string m_propertyNbOfComparisions = "Number of queries";

		const std::string m_groupQueryDefinition = "Query definition";
		const std::string m_propertyName = "Name";
		const std::string m_propertyComparator = "Comparator";
		const std::string m_propertyValue = "Value";

		inline std::string getGroupName(uint32_t _index) const { return m_groupQueryDefinition + "_" + std::to_string(_index); };

		void createPropertiesForIndex(EntityBase* _entity, uint32_t _queryIndex);
		void deletePropertiesForIndex(EntityBase* _entity, uint32_t _queryIndex);
		void setPropertiesVisible(EntityBase* _entity, uint32_t _queryIndex, bool _visible);
		bool needsUpdate(const EntityBase* _entity, uint32_t _queryIndex) const;
		ValueComparisonDescription getValueComparisonDefinition(const EntityBase* _entity, const std::string& _groupName) const;
		void resetNameOptions(EntityBase* _entity);

		std::string m_quantityName = "";
		uint32_t m_maxNumberOfQueryDefinitions = 0;
		std::list<std::string> m_nameSelectionOptions;
	};

}