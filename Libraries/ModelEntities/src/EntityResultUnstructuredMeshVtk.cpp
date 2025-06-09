#pragma once
#include "../include/EntityResultUnstructuredMeshVtk.h"
#include "DataBase.h"

EntityResultUnstructuredMeshVtk::EntityResultUnstructuredMeshVtk(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, ClassFactoryHandler* factory, const std::string & owner)
	:EntityBase(ID,parent,obs,ms,factory,owner)
{}

EntityResultUnstructuredMeshVtk::~EntityResultUnstructuredMeshVtk()
{
	clearAllBinaryData();
}

void EntityResultUnstructuredMeshVtk::clearAllBinaryData()
{
	if (_vtkData != nullptr)
	{
		delete _vtkData;
		_vtkData = nullptr;
	}
}

bool EntityResultUnstructuredMeshVtk::getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax)
{
	return false;
}

void EntityResultUnstructuredMeshVtk::setData(const std::string& quantityName, eQuantityType quantityType, EntityBinaryData*& data)
{
	clearAllBinaryData();

	_quantityName = quantityName;
	_quantityType = quantityType;

	_vtkData = data;

	_vtkDataID      = _vtkData->getEntityID();
	_vtkDataVersion = _vtkData->getEntityStorageVersion();

	data = nullptr;
}

void EntityResultUnstructuredMeshVtk::getData(std::string &quantityName, eQuantityType& quantityType, std::vector<char> &data, ClassFactoryHandler* factory)
{
	quantityName.clear();

	if (_vtkData == nullptr && _vtkDataID != -1 && _vtkDataVersion != -1)
	{
		std::map<ot::UID, EntityBase *> entityMap;
		_vtkData = dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, _vtkDataID, _vtkDataVersion, entityMap, factory));
		assert(_vtkData != nullptr);
	}

	if (_vtkData == nullptr)
	{
		return; // The data is missing for this entity.
	}

	quantityName = _quantityName;
	quantityType = _quantityType;
	data         = _vtkData->getData();
}

void EntityResultUnstructuredMeshVtk::AddStorageData(bsoncxx::builder::basic::document & storage)
{
	EntityBase::AddStorageData(storage);

	if (_vtkData != nullptr)
	{
		_vtkDataID      = _vtkData->getEntityID();
		_vtkDataVersion = _vtkData->getEntityStorageVersion();
	}

	storage.append(
		bsoncxx::builder::basic::kvp("quantityName", _quantityName),
		bsoncxx::builder::basic::kvp("quantityType", (int) _quantityType),
		bsoncxx::builder::basic::kvp("vtkDataID",       _vtkDataID),
		bsoncxx::builder::basic::kvp("vtkDataVersion",	_vtkDataVersion),
		bsoncxx::builder::basic::kvp("resultType",	    (long long)getResultType())
	);
}

void EntityResultUnstructuredMeshVtk::readSpecificDataFromDataBase(bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	_quantityName   = doc_view["quantityName"].get_string();
	_quantityType   = (eQuantityType) (int) (doc_view["quantityType"].get_int32());
	_vtkDataID      = doc_view["vtkDataID"].get_int64().value;
	_vtkDataVersion = doc_view["vtkDataVersion"].get_int64().value;

	setResultType(static_cast<tResultType>((long long)doc_view["resultType"].get_int64()));
}
