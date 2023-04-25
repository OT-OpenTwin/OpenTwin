#pragma once
#include "EntityResultTable.h"
#include "EntityResultTableData.h"
#include "DataBase.h"
#include "Types.h"

#include <bsoncxx/builder/basic/array.hpp>

#include <OpenTwinCommunication/ActionTypes.h>


template <class T>
EntityResultTable<T>::EntityResultTable(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, factory, owner),
	tableDataStorageId(-1),
	tableDataStorageVersion(-1)
{
	className = "EntityResultTable_" + ot::TemplateTypeName<T>::getTypeName();
}

template <class T>
EntityResultTable<T>::~EntityResultTable()
{}

template <class T>
bool EntityResultTable<T>::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

template <class T>
void EntityResultTable<T>::StoreToDataBase(void)
{
	EntityBase::StoreToDataBase();
}

template <class T>
void EntityResultTable<T>::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	// Now we store the particular information about the current object
	storage.append(
		bsoncxx::builder::basic::kvp("TableDataID", tableDataStorageId),
		bsoncxx::builder::basic::kvp("TableDataVersion", tableDataStorageVersion)
	);
}


template <class T>
void EntityResultTable<T>::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	if (tableData.use_count() == 0 && tableData != nullptr)
	{
		tableData = nullptr;
	}

	// Here we can load any special information about the entity
	tableDataStorageId = doc_view["TableDataID"].get_int64();
	tableDataStorageVersion = doc_view["TableDataVersion"].get_int64();

	resetModified();
}

template <class T>
void EntityResultTable<T>::addVisualizationNodes()
{
	TreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "TableVisible";
	treeIcons.hiddenIcon = "TableHidden";

	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_AddContainerNode);
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_TREE_Name, getName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, getEntityID());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsEditable, getEditable());

	treeIcons.addToJsonDoc(&doc);

	getObserver()->sendMessageToViewer(doc);

	//addVisualizationItem(getInitiallyHidden());

	EntityBase::addVisualizationNodes();
}

/*Legacy code. Required in model service.*/
template <class T>
void EntityResultTable<T>::addVisualizationItem(bool isHidden)
{
	TreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "TableVisible";
	treeIcons.hiddenIcon = "TableHidden";

	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_AddTable);
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_CONTROL_ObjectName, getName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, getEntityID());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsEditable, getEditable());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsHidden, isHidden);
	ot::rJSON::add(doc, OT_ACTION_PARAM_PROJECT_NAME, DataBase::GetDataBase()->getProjectName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, (ot::UID)tableDataStorageId);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityVersion, (ot::UID)tableDataStorageVersion);

	treeIcons.addToJsonDoc(&doc);

	getObserver()->sendMessageToViewer(doc);
}


template <class T>
void EntityResultTable<T>::createProperties()
{
	// This item has no properties yet	
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
	EnsureTableDataLoaded();
	return tableData;
}

template <class T>
void EntityResultTable<T>::EnsureTableDataLoaded()
{
	if (tableData == nullptr)
	{
		if (tableDataStorageId != -1)
		{
			if (tableDataStorageVersion != -1)
			{
				std::map<ot::UID, EntityBase*> entitymap;
				auto tempPtr = dynamic_cast<EntityResultTableData<T>*>(readEntityFromEntityIDAndVersion(this, tableDataStorageId, tableDataStorageVersion, entitymap));
				if (tempPtr != nullptr)
				{
					tableData = std::make_shared<EntityResultTableData<T>>(*tempPtr);
				}
			}
		}

		assert(tableData != nullptr);
	}
}

template<class T>
void EntityResultTable<T>::setTableData(std::shared_ptr<EntityResultTableData<T>> data)
{
	if (data != nullptr)
	{
		setModified();
	}
	tableData = data;
	tableDataStorageId = tableData->getEntityID();
	tableDataStorageVersion = tableData->getEntityStorageVersion();
}

template <class T>
void EntityResultTable<T>::deleteTableData(void)
{
	tableData = nullptr;
	tableDataStorageId = -1;
	tableDataStorageVersion = -1;

	setModified();
}

template <class T>
void EntityResultTable<T>::releaseTableData(void)
{
	if (tableData == nullptr)
	{
		return;
	};
	tableData = nullptr;
}

