// @otlicense
// File: EntityMetadataCampaign.h
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
#include "OTCore/MetadataHandle/MetadataCampaign.h"
#include "EntityBase.h"

#include <string>

class __declspec(dllexport) EntityMetadataCampaign : public EntityBase
{
public:
	EntityMetadataCampaign() : EntityMetadataCampaign(0, nullptr, nullptr, nullptr) {};
	EntityMetadataCampaign(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms);

	static std::string className() { return "EntityMetadataCampaign"; };
	std::string getClassName() const override { return EntityMetadataCampaign::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override;

	virtual void addVisualizationNodes() override;

	void setCampaign(const MetadataCampaign& _campaign) 
	{ 
		m_campaign = _campaign; 
		setModified();
	}
	const MetadataCampaign& getCampaign() { return m_campaign; }
private:
	MetadataCampaign m_campaign;
	
	virtual void addStorageData(bsoncxx::builder::basic::document& storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};