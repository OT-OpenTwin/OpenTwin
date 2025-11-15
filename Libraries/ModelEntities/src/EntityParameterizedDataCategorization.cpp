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

#include "EntityParameterizedDataCategorization.h"

#include "OTCommunication/ActionTypes.h"

static EntityFactoryRegistrar<EntityParameterizedDataCategorization> registrar("EntityParameterizedDataCategorization");

EntityParameterizedDataCategorization::EntityParameterizedDataCategorization(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	:EntityContainer(ID, parent, obs, ms)
{
	ot::EntityTreeItem treeItem;
	treeItem.setVisibleIcon(c_unlockedIcon);
	treeItem.setHiddenIcon(c_unlockedIcon);
	this->setTreeItem(treeItem, true);
}

void EntityParameterizedDataCategorization::addVisualizationNodes()
{
	if (!getName().empty())
	{
		ot::EntityTreeItem treeItem = this->getTreeItem();
		if (m_locked)
		{
			treeItem.setVisibleIcon(c_lockedIcon);
			treeItem.setHiddenIcon(c_lockedIcon);
		}
		else
		{
			treeItem.setVisibleIcon(c_unlockedIcon);
			treeItem.setHiddenIcon(c_unlockedIcon);
		}

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());

		doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
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
	g_dataCategorieStringMapping.find(category) == g_dataCategorieStringMapping.end() ? returnVal = "" : returnVal = g_dataCategorieStringMapping.at(category);
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
	g_dataCategorieStringMapping.find(m_selectedCategory) == g_dataCategorieStringMapping.end() ? returnVal = "" : returnVal = g_dataCategorieStringMapping.at(m_selectedCategory);
	if (returnVal != "")
	{
		EntityContainer::addStorageData(storage);
		storage.append(
			bsoncxx::builder::basic::kvp("Category", returnVal)
		);
	}
	storage.append(bsoncxx::builder::basic::kvp("Locked", m_locked));
}

void EntityParameterizedDataCategorization::readSpecificDataFromDataBase(bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityContainer::readSpecificDataFromDataBase(doc_view, entityMap);

	const std::string categorySerialized(doc_view["Category"].get_utf8().value.data());
	DataCategorie category = UNKNOWN;
	g_stringDataCategorieMapping.find(categorySerialized) == g_stringDataCategorieMapping.end() ? category = UNKNOWN: category = g_stringDataCategorieMapping.at(categorySerialized);
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

const std::string EntityParameterizedDataCategorization::c_dataCategoryGroup = "Data Category";
const std::string EntityParameterizedDataCategorization::c_defaultCategory = "DefaultCategory";
const std::string EntityParameterizedDataCategorization::c_dataCategoryRMD = "Research";
const std::string EntityParameterizedDataCategorization::c_dataCategoryMSMD = "Measurementseries";
const std::string EntityParameterizedDataCategorization::c_dataCategoryParam = "Parameter";
const std::string EntityParameterizedDataCategorization::c_dataCategoryQuant = "Quantity";

std::map<EntityParameterizedDataCategorization::DataCategorie, std::string> EntityParameterizedDataCategorization::g_dataCategorieStringMapping = { {DataCategorie::researchMetadata, c_dataCategoryRMD}, { DataCategorie::measurementSeriesMetadata, c_dataCategoryMSMD }, { DataCategorie::parameter, c_dataCategoryParam }, { DataCategorie::quantity, c_dataCategoryQuant } };
std::map<std::string, EntityParameterizedDataCategorization::DataCategorie> EntityParameterizedDataCategorization::g_stringDataCategorieMapping = { {c_dataCategoryRMD, DataCategorie::researchMetadata }, { c_dataCategoryMSMD, DataCategorie::measurementSeriesMetadata }, { c_dataCategoryParam, DataCategorie::parameter }, { c_dataCategoryQuant, DataCategorie::quantity } };

std::string EntityParameterizedDataCategorization::GetStringDataCategorization(EntityParameterizedDataCategorization::DataCategorie category)
{
	std::string returnVal;
	g_dataCategorieStringMapping.find(category) == g_dataCategorieStringMapping.end() ? returnVal = "" : returnVal = g_dataCategorieStringMapping.at(category);
	return returnVal;
}
