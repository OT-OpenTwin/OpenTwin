// @otlicense
// File: EntityParameterizedDataCategorization.cpp
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

#include "OTModelEntities/EntityParameterizedDataCategorization.h"

#include "OTCommunication/ActionTypes.h"

static EntityFactoryRegistrar<EntityParameterizedDataCategorization> registrar("EntityParameterizedDataCategorization");

EntityParameterizedDataCategorization::EntityParameterizedDataCategorization(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	:EntityContainer(ID, parent, obs, ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon(m_unlockedIcon);
	treeItem.setHiddenIcon(m_unlockedIcon);
	this->setDefaultTreeItem(treeItem);
}

void EntityParameterizedDataCategorization::addVisualizationNodes()
{
	if (!getName().empty())
	{
		ot::EntityTreeItem treeItem = this->getTreeItem();
		if (m_locked)
		{
			treeItem.setVisibleIcon(m_lockedIcon);
			treeItem.setHiddenIcon(m_lockedIcon);
		}
		else
		{
			treeItem.setVisibleIcon(m_unlockedIcon);
			treeItem.setHiddenIcon(m_unlockedIcon);
		}
		
		if (GetSelectedDataCategorie() == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata )
		{
			treeItem.setSelectChilds(false);
		}
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());

		doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(treeItem, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

		getObserver()->sendMessageToViewer(doc);
	}

	for (auto child : getChildrenList())
	{
		child->addVisualizationNodes();
	}

	EntityBase::addVisualizationNodes();
}

void EntityParameterizedDataCategorization::CreateProperties(DataCategorie category)
{
	std::string returnVal;
	m_dataCategorieStringMapping.find(category) == m_dataCategorieStringMapping.end() ? returnVal = "" : returnVal = m_dataCategorieStringMapping.at(category);
	if (returnVal != "")
	{
		m_selectedCategory = category;
	}
}

void EntityParameterizedDataCategorization::addStorageData(bsoncxx::builder::basic::document & storage)
{
	if (m_selectedCategory == DataCategorie::UNKNOWN)
	{
		throw std::runtime_error("Data category in EntityParameterizedDataCategorization was not set.");
	}
	std::string returnVal;
	m_dataCategorieStringMapping.find(m_selectedCategory) == m_dataCategorieStringMapping.end() ? returnVal = "" : returnVal = m_dataCategorieStringMapping.at(m_selectedCategory);
	if (returnVal != "")
	{
		EntityContainer::addStorageData(storage);
		storage.append(
			bsoncxx::builder::basic::kvp("Category", returnVal)
		);
	}
	storage.append(bsoncxx::builder::basic::kvp("Locked", m_locked));
}

void EntityParameterizedDataCategorization::readSpecificDataFromDataBase(const bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityContainer::readSpecificDataFromDataBase(doc_view, entityMap);

	const std::string categorySerialized(doc_view["Category"].get_utf8().value.data());
	DataCategorie category = UNKNOWN;
	m_stringDataCategorieMapping.find(categorySerialized) == m_stringDataCategorieMapping.end() ? category = UNKNOWN: category = m_stringDataCategorieMapping.at(categorySerialized);
	if (category != UNKNOWN)
	{
		m_selectedCategory = category;
	}
	else
	{
		throw std::runtime_error("Data category in EntityParameterizedDataCategorization was not set.");
	}
	m_locked = doc_view["Locked"].get_bool();
}

const std::string EntityParameterizedDataCategorization::m_dataCategoryGroup = "Data Category";
const std::string EntityParameterizedDataCategorization::m_defaultCategory = "DefaultCategory";
const std::string EntityParameterizedDataCategorization::m_dataCategoryRMD = "Research";
const std::string EntityParameterizedDataCategorization::m_dataCategoryMSMD = "Measurementseries";
const std::string EntityParameterizedDataCategorization::m_dataCategoryParam = "Parameter";
const std::string EntityParameterizedDataCategorization::m_dataCategoryQuant = "Quantity";

std::map<EntityParameterizedDataCategorization::DataCategorie, std::string> EntityParameterizedDataCategorization::m_dataCategorieStringMapping = { {DataCategorie::researchMetadata, m_dataCategoryRMD}, { DataCategorie::measurementSeriesMetadata, m_dataCategoryMSMD }, { DataCategorie::parameter, m_dataCategoryParam }, { DataCategorie::quantity, m_dataCategoryQuant } };
std::map<std::string, EntityParameterizedDataCategorization::DataCategorie> EntityParameterizedDataCategorization::m_stringDataCategorieMapping = { {m_dataCategoryRMD, DataCategorie::researchMetadata }, { m_dataCategoryMSMD, DataCategorie::measurementSeriesMetadata }, { m_dataCategoryParam, DataCategorie::parameter }, { m_dataCategoryQuant, DataCategorie::quantity } };

std::string EntityParameterizedDataCategorization::GetStringDataCategorization(EntityParameterizedDataCategorization::DataCategorie category)
{
	std::string returnVal;
	m_dataCategorieStringMapping.find(category) == m_dataCategorieStringMapping.end() ? returnVal = "" : returnVal = m_dataCategorieStringMapping.at(category);
	return returnVal;
}
