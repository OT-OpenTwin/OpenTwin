#pragma once
#include "EntityResultTable.h"
#include "EntityResultTableData.h"
#include "DataBase.h"
#include "Types.h"

#include <bsoncxx/builder/basic/array.hpp>

#include "OTCommunication/ActionTypes.h"


template <class T>
EntityResultTable<T>::EntityResultTable(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, factory, owner),
	tableDataStorageId(-1),
	tableDataStorageVersion(-1)
{
	className = "EntityResultTable_" + ot::TemplateTypeName<T>::getTypeName();
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
		bsoncxx::builder::basic::kvp("TableDataVersion", tableDataStorageVersion),
		bsoncxx::builder::basic::kvp("MinCol", _minCol),
		bsoncxx::builder::basic::kvp("MaxCol", _maxCol),
		bsoncxx::builder::basic::kvp("MinRow", _minRow),
		bsoncxx::builder::basic::kvp("MaxRow", _maxRow)
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
	_minCol =	doc_view["MinCol"].get_int32();
	_maxCol =	doc_view["MaxCol"].get_int32();
	_minRow =	doc_view["MinRow"].get_int32();
	_maxRow =	doc_view["MaxRow"].get_int32();

	resetModified();
}

template <class T>
void EntityResultTable<T>::addVisualizationNodes()
{
	TreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "TableVisible";
	treeIcons.hiddenIcon = "TableHidden";

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

	treeIcons.addToJsonDoc(doc);

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

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddTable, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectName, ot::JsonString(getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, getEditable(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsHidden, isHidden, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(DataBase::GetDataBase()->getProjectName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, (ot::UID)tableDataStorageId, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion, (ot::UID)tableDataStorageVersion, doc.GetAllocator());

	treeIcons.addToJsonDoc(&doc);

	getObserver()->sendMessageToViewer(doc);
}


template <class T>
void EntityResultTable<T>::createProperties()
{
	const std::string visibleTableSize = "Visible Table Dimensions";

	EntityPropertiesInteger::createProperty(visibleTableSize, "Min Row", _minRow, "default", getProperties());
	EntityPropertiesInteger::createProperty(visibleTableSize, "Max Row", _maxRow, "default", getProperties());
	EntityPropertiesInteger::createProperty(visibleTableSize, "Min Column", _minCol, "default", getProperties());
	EntityPropertiesInteger::createProperty(visibleTableSize, "Max Column", _maxCol, "default", getProperties());
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
					tableData = std::shared_ptr <EntityResultTableData<T>>(tempPtr);
				}
				else
				{
					throw std::exception("Failed to load the Table Data.");
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

