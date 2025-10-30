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
#include "PropertyBundle.h"
#include "OTCore/ValueComparisionDefinition.h"

class __declspec(dllexport) PropertyBundleQuerySettings : public PropertyBundle
{
public:
	virtual void setProperties(EntityBase* _thisObject) override;
	virtual bool updatePropertyVisibility(EntityBase* _thisObject) override;
	void setQueryDefinitions(const std::list<std::string>& _queryOptions);
	void reload(EntityBase* _thisObject);

	std::list<ValueComparisionDefinition> getValueComparisionDefinitions(EntityBase* _thisObject);
	
	//! @brief Checks if parameter that determine the query have changed.
	bool requiresUpdate(EntityBase* _thisObject);

	EntityPropertiesInteger* getNumberOfQueriesProperty(EntityBase* _thisObject);

	void updateQuerySettings(EntityBase* _thisObject,const std::list<std::string>& _queryOptions);

private:
	const std::string m_groupQuerySettings = "Query settings";
	const std::string m_propertyNbOfComparisions = "Number of queries";

	const std::string m_groupQueryDefinition = "Query definition";
	const std::string m_propertyName = "Name";
	const std::string m_propertyComparator = "Comparator";
	const std::string m_propertyValue = "Value";

	std::string m_quantityName = "";
	uint32_t m_maxNumberOfQueryDefinitions = 0;
	std::list<std::string> m_selectionOptions;
};
