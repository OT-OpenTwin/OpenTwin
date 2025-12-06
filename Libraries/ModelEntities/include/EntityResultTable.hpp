// @otlicense
// File: EntityResultTable.hpp
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
#include "DataBase.h"
#include "EntityResultTable.h"
#include "EntityResultTableData.h"

#include <bsoncxx/builder/basic/array.hpp>

#include "OTCommunication/ActionTypes.h"
#include "OTGui/VisualisationTypes.h"

template <class T>
EntityResultTable<T>::EntityResultTable(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityBase(ID, parent, obs, ms),
	m_tableDataStorageId(-1),
	m_tableDataStorageVersion(-1)
{
	m_className = "EntityResultTable_" + ot::TemplateTypeName<T>::getTypeName();

	ot::EntityTreeItem treeItem;
	treeItem.setVisibleIcon("Default/TableVisible");
	treeItem.setHiddenIcon("Default/TableHidden");
	this->setTreeItem(treeItem, true);

	ot::VisualisationTypes visType;
	visType.addTableVisualisation();
	this->setVisualizationTypes(visType, true);
}

template <class T>
EntityResultTable<T>::~EntityResultTable()
{
}

template <class T>
bool EntityResultTable<T>::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

template <class T>
void EntityResultTable<T>::storeToDataBase(void)
{
	EntityBase::storeToDataBase();
}

template <class T>
void EntityResultTable<T>::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Now we store the particular information about the current object
	storage.append(
		bsoncxx::builder::basic::kvp("TableDataID", m_tableDataStorageId),
		bsoncxx::builder::basic::kvp("TableDataVersion", m_tableDataStorageVersion),
		bsoncxx::builder::basic::kvp("MinCol", m_minCol),
		bsoncxx::builder::basic::kvp("MaxCol", m_maxCol),
		bsoncxx::builder::basic::kvp("MinRow", m_minRow),
		bsoncxx::builder::basic::kvp("MaxRow", m_maxRow)
	);
}


template <class T>
void EntityResultTable<T>::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	if (m_tableData.use_count() == 0 && m_tableData != nullptr)
	{
		m_tableData = nullptr;
	}

	// Here we can load any special information about the entity
	m_tableDataStorageId = doc_view["TableDataID"].get_int64();
	m_tableDataStorageVersion = doc_view["TableDataVersion"].get_int64();
	m_minCol =	doc_view["MinCol"].get_int32();
	m_maxCol =	doc_view["MaxCol"].get_int32();
	m_minRow =	doc_view["MinRow"].get_int32();
	m_maxRow =	doc_view["MaxRow"].get_int32();

	resetModified();
}

template <class T>
void EntityResultTable<T>::addVisualizationNodes()
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);

	//addVisualizationItem(getInitiallyHidden());

	EntityBase::addVisualizationNodes();
}

/*Legacy code. Required in model service.*/
template <class T>
void EntityResultTable<T>::addVisualizationItem(bool isHidden)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddSceneNode, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);
}


template <class T>
void EntityResultTable<T>::createProperties()
{
	const std::string visibleTableSize = "Visible Table Dimensions";

	EntityPropertiesInteger::createProperty(visibleTableSize, "Min Row", m_minRow, "default", getProperties());
	EntityPropertiesInteger::createProperty(visibleTableSize, "Max Row", m_maxRow, "default", getProperties());
	EntityPropertiesInteger::createProperty(visibleTableSize, "Min Column", m_minCol, "default", getProperties());
	EntityPropertiesInteger::createProperty(visibleTableSize, "Max Column", m_maxCol, "default", getProperties());
	getProperties().forceResetUpdateForAllProperties();
}

template <class T>
bool EntityResultTable<T>::updateFromProperties()
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	getProperties().forceResetUpdateForAllProperties();

	return false; // Notify, whether property grid update is necessary
}

template <class T>
std::shared_ptr<EntityResultTableData<T>> EntityResultTable<T>::getTableData()
{
	ensureTableDataLoaded();
	return m_tableData;
}

template <class T>
void EntityResultTable<T>::ensureTableDataLoaded()
{
	if (m_tableData == nullptr)
	{
		if (m_tableDataStorageId != -1)
		{
			if (m_tableDataStorageVersion != -1)
			{
				std::map<ot::UID, EntityBase*> entitymap;
				auto tempPtr = dynamic_cast<EntityResultTableData<T>*>(readEntityFromEntityIDAndVersion(this, m_tableDataStorageId, m_tableDataStorageVersion, entitymap));
				if (tempPtr != nullptr)
				{
					m_tableData = std::shared_ptr <EntityResultTableData<T>>(tempPtr);
				}
				else
				{
					throw std::exception("Failed to load the Table Data.");
				}
			}
		}

		assert(m_tableData != nullptr);
	}
}

template<class T>
void EntityResultTable<T>::setTableData(std::shared_ptr<EntityResultTableData<T>> data)
{
	if (data != nullptr)
	{
		setModified();
	}
	m_tableData = data;
	m_tableDataStorageId = m_tableData->getEntityID();
	m_tableDataStorageVersion = m_tableData->getEntityStorageVersion();
}


template<class T>
inline int EntityResultTable<T>::getMinColumn()
{
	auto propBase = getProperties().getProperty("Min Column");
	auto intProp =	dynamic_cast<EntityPropertiesInteger*>(propBase);
	return intProp->getValue();
}

template<class T>
inline int EntityResultTable<T>::getMinRow()
{
	auto propBase = getProperties().getProperty("Min Row");
	auto intProp = dynamic_cast<EntityPropertiesInteger*>(propBase);
	return intProp->getValue();
}

template<class T>
inline int EntityResultTable<T>::getMaxColumn()
{
	auto propBase = getProperties().getProperty("Max Column");
	auto intProp = dynamic_cast<EntityPropertiesInteger*>(propBase);
	return intProp->getValue();
}

template<class T>
inline int EntityResultTable<T>::getMaxRow()
{
	auto propBase = getProperties().getProperty("Max Row");
	auto intProp = dynamic_cast<EntityPropertiesInteger*>(propBase);
	return intProp->getValue();
}

template<class T>
inline void EntityResultTable<T>::setMinColumn(int minCol)
{
	auto propBase = getProperties().getProperty("Min Column");
	auto intProp = dynamic_cast<EntityPropertiesInteger*>(propBase);
	intProp->setValue(minCol);
	intProp->setNeedsUpdate();
}

template<class T>
inline void EntityResultTable<T>::setMinRow(int minRow)
{
	auto propBase = getProperties().getProperty("Min Row");
	auto intProp = dynamic_cast<EntityPropertiesInteger*>(propBase);
	intProp->setValue(minRow);
	intProp->setNeedsUpdate();
}

template<class T>
inline void EntityResultTable<T>::setMaxColumn(int maxCol)
{
	auto propBase = getProperties().getProperty("Max Column");
	auto intProp = dynamic_cast<EntityPropertiesInteger*>(propBase);
	intProp->setValue(maxCol);
	intProp->setNeedsUpdate();
}

template<class T>
inline void EntityResultTable<T>::setMaxRow(int maxRow)
{
	auto propBase = getProperties().getProperty("Max Row");
	auto intProp = dynamic_cast<EntityPropertiesInteger*>(propBase);
	intProp->setValue(maxRow);
	intProp->setNeedsUpdate();
}

template <class T>
void EntityResultTable<T>::deleteTableData(void)
{
	m_tableData = nullptr;
	m_tableDataStorageId = -1;
	m_tableDataStorageVersion = -1;

	setModified();
}

template <class T>
void EntityResultTable<T>::releaseTableData(void)
{
	if (m_tableData == nullptr)
	{
		return;
	};
	m_tableData = nullptr;
}

