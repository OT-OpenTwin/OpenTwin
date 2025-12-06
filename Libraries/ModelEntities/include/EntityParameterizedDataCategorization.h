// @otlicense
// File: EntityParameterizedDataCategorization.h
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
#include "EntityContainer.h"
#include <map>
#include <string>

class __declspec(dllexport) EntityParameterizedDataCategorization: public EntityContainer
{
public:
	enum DataCategorie { researchMetadata, measurementSeriesMetadata, quantity, parameter, UNKNOWN};

	EntityParameterizedDataCategorization() : EntityParameterizedDataCategorization(0, nullptr, nullptr, nullptr) {};
	EntityParameterizedDataCategorization(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);

	virtual std::string getClassName(void) const override { return "EntityParameterizedDataCategorization"; };
	virtual void addVisualizationNodes() override;
	
	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	void CreateProperties(DataCategorie categorie);
	DataCategorie GetSelectedDataCategorie() { return m_selectedCategory; };

	static std::string GetStringDataCategorization(EntityParameterizedDataCategorization::DataCategorie category);

	void setLock(bool _lock) {
		m_locked = _lock;
		setModified();
	}

	bool getIsLocked() const { return m_locked; };

private:
	static const std::string c_dataCategoryGroup;
	static const std::string c_defaultCategory;
	static const std::string c_dataCategoryRMD;
	static const std::string c_dataCategoryMSMD;
	static const std::string c_dataCategoryParam;
	static const std::string c_dataCategoryQuant;

	const std::string c_unlockedIcon = "Default/Categorization";
	const std::string c_lockedIcon = "Default/CategorizationLocked";
	bool m_locked = false;

	DataCategorie m_selectedCategory = UNKNOWN;
	
	virtual void addStorageData(bsoncxx::builder::basic::document& storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	virtual int getSchemaVersion() { return 1; };

	static std::map<std::string, DataCategorie> g_stringDataCategorieMapping;
	static std::map<DataCategorie, std::string> g_dataCategorieStringMapping;
};
