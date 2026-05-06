// @otlicense
// File: EntityMetadataSeries.h
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
#include "OTModelEntities/EntityBase.h"
#include "OTCore/JSON/JSON.h"

class __declspec(dllexport) EntityMetadataSeries : public EntityBase
{
public:
	EntityMetadataSeries() : EntityMetadataSeries(0, nullptr, nullptr, nullptr) {};
	EntityMetadataSeries(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms);

	static std::string className() { return "EntityMetadataSeries"; };
	std::string getClassName() const override { return EntityMetadataSeries::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override;
	
	virtual void addVisualizationNodes() override;

	void setSeries(const MetadataSeries& _series) 
	{ 
		m_series = _series; 
		setModified();
	};
	MetadataSeries& getSeries() { return m_series; };
	
private:
	MetadataSeries m_series;
	
	virtual void addStorageData(bsoncxx::builder::basic::document& storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};
