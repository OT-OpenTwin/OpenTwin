// @otlicense
// File: ResultCollectionMetadataAccess.h
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
#include <string>
#include <list>

#include "OTServiceFoundation/ModelComponent.h"

#include "EntityBase.h"

#include "MetadataCampaign.h"
#include "MetadataQuantity.h"
#include "MetadataParameter.h"
#include "MetadataSeries.h"

class __declspec(dllexport) ResultCollectionMetadataAccess
{
public:
	ResultCollectionMetadataAccess(const std::string& collectionName, ot::components::ModelComponent* modelComponent);
	ResultCollectionMetadataAccess(const std::string& crossCollectionName, ot::components::ModelComponent* modelComponent, const std::string& sessionServiceURL);
	ResultCollectionMetadataAccess(ResultCollectionMetadataAccess&& other) noexcept;
	ResultCollectionMetadataAccess& operator=(ResultCollectionMetadataAccess&& other) noexcept;

	virtual ~ResultCollectionMetadataAccess() {};

	const std::list<std::string> listAllSeriesNames() const;
	const std::list<std::string> listAllParameterLabels() const;
	const std::list<std::string> listAllQuantityLabels() const;

	const std::list<std::string> listAllParameterLabelsFromSeries(const std::string& _label) const;
	const std::list<std::string> listAllQuantityLabelsFromSeries(const std::string& _label) const;

	//! <returns>nullptr if not found</returns>
	const MetadataSeries* findMetadataSeries(const std::string& _label) const;
	//! <returns>nullptr if not found</returns>
	const MetadataSeries* findMetadataSeries(ot::UID _index) const ;
	//! <returns>nullptr if not found</returns>
	const MetadataParameter* findMetadataParameter(const std::string& _label) const;
	//! <returns>nullptr if not found</returns>
	const MetadataParameter* findMetadataParameter(ot::UID _index) const;
	//! <returns>nullptr if not found</returns>
	const MetadataQuantity* findMetadataQuantity(const std::string& _label) const;
	//! <returns>nullptr if not found</returns>
	const MetadataQuantity* findMetadataQuantity(ot::UID _index) const; //Needs to look through the value descriptions!

	std::list< MetadataQuantity*>findQuantityWithSameName(const std::string& _name);
	std::list< MetadataParameter*>findParameterWithSameName(const std::string& _name);

	const MetadataCampaign& getMetadataCampaign() const { return m_metadataCampaign; }

	const std::string& getCollectionName() const { return m_collectionName; }

	bool collectionHasMetadata() const { return m_metadataExistInProject; }



protected:
	ResultCollectionMetadataAccess() {};
	bool m_metadataExistInProject = false;
	std::string m_collectionName;
	ot::components::ModelComponent* m_modelComponent = nullptr;
	MetadataCampaign m_metadataCampaign;

private:
	void loadExistingCampaignData();
	std::vector<EntityBase*> findAllExistingMetadata();
};
