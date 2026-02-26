// @otlicense
// File: EntityMetadataSeries.cpp
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

#include "OTModelEntities/EntityMetadataSeries.h"

#include "OTCommunication/ActionTypes.h"
#include "bsoncxx/json.hpp"

static EntityFactoryRegistrar<EntityMetadataSeries> registrar("EntityMetadataSeries");

EntityMetadataSeries::EntityMetadataSeries(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms)
	: EntityWithDynamicFields(ID, parent, mdl, ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/MSMD");
	treeItem.setHiddenIcon("Default/MSMD");
	this->setDefaultTreeItem(treeItem);

	CreatePlainDocument(_parameterDocument);
	CreatePlainDocument(_quantityDocument);
}

bool EntityMetadataSeries::getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax)
{
	return false;
}

std::vector<std::string> EntityMetadataSeries::getAllParameterDocumentNames()
{
	return getDocumentsNames(_parameterDocument);
}

std::vector<std::string> EntityMetadataSeries::getAllQuantityDocumentNames()
{
	return getDocumentsNames(_quantityDocument);
}

void EntityMetadataSeries::InsertToParameterField(std::string fieldName, std::list<ot::Variable>&& values, std::string documentName)
{
	std::string fullDocumentPath = _parameterDocument;
	if (documentName != "")
	{
		fullDocumentPath += "/" + documentName;
	}
	InsertInField(fieldName, std::move(values), fullDocumentPath);
}

void EntityMetadataSeries::InsertToQuantityField(std::string fieldName, std::list<ot::Variable>&& values, std::string documentName)
{
	std::string fullDocumentPath = _quantityDocument;
	if (documentName != "")
	{
		fullDocumentPath += "/" + documentName;
	}
	InsertInField(fieldName, std::move(values), fullDocumentPath);
}

void EntityMetadataSeries::setMetadata(const ot::JsonDocument& _metadata)
{
	m_metadata.CopyFrom(_metadata, m_metadata.GetAllocator());
}


void EntityMetadataSeries::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityWithDynamicFields::addStorageData(_storage);
	try
	{
		const std::string metadata = ot::json::toJson(m_metadata);
		bsoncxx::document::value doc = bsoncxx::from_json(metadata);
		_storage.append(bsoncxx::builder::basic::kvp("Metadata", doc));
	}
	catch (std::exception& _e)
	{
		const std::string message = "Failed to store metadata of series: " + this->getName() + ". Exception: " + std::string(_e.what());
		OT_LOG_E(message);
	}
}

void EntityMetadataSeries::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityWithDynamicFields::readSpecificDataFromDataBase(doc_view, entityMap);
	if (doc_view.find("Metadata") != doc_view.end())
	{
		try
		{
			auto metadataView = doc_view["Metadata"];
			const std::string metadataString = bsoncxx::to_json(metadataView.get_document());
			m_metadata.fromJson(metadataString);
		}
		catch (std::exception& _e)
		{
			const std::string message = "Failed to load metadata of series: " + this->getName() + ". Exception: " + std::string(_e.what());
			OT_LOG_E(message);
		}
	}
}
