#pragma once
#include "../include/EntityResultUnstructuredMeshData.h"
#include "DataBase.h"

static EntityFactoryRegistrar<EntityResultUnstructuredMeshData> registrar("EntityResultUnstructuredMeshData");

EntityResultUnstructuredMeshData::EntityResultUnstructuredMeshData(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, const std::string & owner)
	:EntityBase(ID,parent,obs,ms,owner)
{}

EntityResultUnstructuredMeshData::~EntityResultUnstructuredMeshData()
{
	clearAllBinaryData();
}

void EntityResultUnstructuredMeshData::clearAllBinaryData()
{
	if (_pointScalar != nullptr)
	{
		delete _pointScalar;
		_pointScalar = nullptr;
	}

	if (_pointVector != nullptr)
	{
		delete _pointVector;
		_pointVector = nullptr;
	}
	
	if (_cellScalar != nullptr)
	{
		delete _cellScalar;
		_cellScalar = nullptr;
	}

	if (_cellVector != nullptr)
	{
		delete _cellVector;
		_cellVector = nullptr;
	}
}

bool EntityResultUnstructuredMeshData::getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax)
{
	return false;
}

void EntityResultUnstructuredMeshData::setData(long long numberPoints, long long numberCells, EntityBinaryData*& pointScalar, EntityBinaryData*& pointVector, EntityBinaryData*& cellScalar, EntityBinaryData*& cellVector)
{
	clearAllBinaryData();

	_numberPoints = numberPoints;
	_numberCells  = numberCells;

	_pointScalar = pointScalar;
	_pointVector = pointVector;
	_cellScalar  = cellScalar;
	_cellVector  = cellVector;

	updateIDFromObjects();

	pointScalar = pointVector = cellScalar = cellVector = nullptr;
}

void EntityResultUnstructuredMeshData::getData(size_t& lenPointScalar, float*& pointScalar, size_t& lenPointVector, float*& pointVector, size_t& lenCellScalar, float*& cellScalar, size_t& lenCellVector, float*& cellVector)
{
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIds;

	if (_pointScalar == nullptr && _pointScalarID != -1 && _pointScalarVersion != -1)
	{
		prefetchIds.push_back(std::pair<unsigned long long, unsigned long long>(_pointScalarID, _pointScalarVersion));
	}

	if (_pointVector == nullptr && _pointVectorID != -1 && _pointVectorVersion != -1)
	{
		prefetchIds.push_back(std::pair<unsigned long long, unsigned long long>(_pointVectorID, _pointVectorVersion));
	}

	if (_cellScalar == nullptr && _cellScalarID != -1 && _cellScalarVersion != -1)
	{
		prefetchIds.push_back(std::pair<unsigned long long, unsigned long long>(_cellScalarID, _cellScalarVersion));
	}

	if (_cellVector == nullptr && _cellVectorID != -1 && _cellVectorVersion != -1)
	{
		prefetchIds.push_back(std::pair<unsigned long long, unsigned long long>(_cellVectorID, _cellVectorVersion));
	}

	if (!prefetchIds.empty())
	{
		DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIds);
	}

	if (_pointScalar == nullptr && _pointScalarID != -1 && _pointScalarVersion != -1)
	{
		std::map<ot::UID, EntityBase *> entityMap;
		_pointScalar = dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, _pointScalarID, _pointScalarVersion, entityMap));
		assert(_pointScalar != nullptr);
	}

	if (_pointVector == nullptr && _pointVectorID != -1 && _pointVectorVersion != -1)
	{
		std::map<ot::UID, EntityBase*> entityMap;
		_pointVector = dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, _pointVectorID, _pointVectorVersion, entityMap));
		assert(_pointVector != nullptr);
	}

	if (_cellScalar == nullptr && _cellScalarID != -1 && _cellScalarVersion != -1)
	{
		std::map<ot::UID, EntityBase*> entityMap;
		_cellScalar = dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, _cellScalarID, _cellScalarVersion, entityMap));
		assert(_cellScalar != nullptr);
	}

	if (_cellVector == nullptr && _cellVectorID != -1 && _cellVectorVersion != -1)
	{
		std::map<ot::UID, EntityBase*> entityMap;
		_cellVector = dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, _cellVectorID, _cellVectorVersion, entityMap));
		assert(_cellVector != nullptr);
	}

	std::vector<char> pointScalarBytes, pointVectorBytes, cellScalarBytes, cellVectorBytes;
	
	readData(_pointScalar, lenPointScalar, pointScalar);
	readData(_pointVector, lenPointVector, pointVector);
	readData(_cellScalar,  lenCellScalar,  cellScalar);
	readData(_cellVector,  lenCellVector,  cellVector);
}

void EntityResultUnstructuredMeshData::readData(EntityBinaryData* data, size_t& lenValues, float*& values)
{
	if (data == nullptr)
	{
		lenValues = 0;
		values = nullptr;
	}
	else
	{
		std::vector<char> bytes = data->getData();

		lenValues = bytes.size() / sizeof(float);

		values = new float[lenValues];

		for (size_t index = 0; index < lenValues; index++)
		{
			values[index] = ((float*)bytes.data())[index];
		}
	}
}

void EntityResultUnstructuredMeshData::updateIDFromObjects(void)
{
	if (_pointScalar != nullptr)
	{
		_pointScalarID = _pointScalar->getEntityID();
		_pointScalarVersion = _pointScalar->getEntityStorageVersion();
	}

	if (_pointVector != nullptr)
	{
		_pointVectorID = _pointVector->getEntityID();
		_pointVectorVersion = _pointVector->getEntityStorageVersion();
	}

	if (_cellScalar != nullptr)
	{
		_cellScalarID = _cellScalar->getEntityID();
		_cellScalarVersion = _cellScalar->getEntityStorageVersion();
	}

	if (_cellVector != nullptr)
	{
		_cellVectorID = _cellVector->getEntityID();
		_cellVectorVersion = _cellVector->getEntityStorageVersion();
	}
}

void EntityResultUnstructuredMeshData::AddStorageData(bsoncxx::builder::basic::document & storage)
{
	EntityBase::AddStorageData(storage);

	updateIDFromObjects();

	storage.append(
		bsoncxx::builder::basic::kvp("numberPoints",		_numberPoints),
		bsoncxx::builder::basic::kvp("numberCells",			_numberCells),
		bsoncxx::builder::basic::kvp("pointScalarID",		_pointScalarID),
		bsoncxx::builder::basic::kvp("pointScalarVersion",	_pointScalarVersion),
		bsoncxx::builder::basic::kvp("pointVectorID",		_pointVectorID),
		bsoncxx::builder::basic::kvp("pointVectorVersion",	_pointVectorVersion),
		bsoncxx::builder::basic::kvp("cellScalarID",		_cellScalarID),
		bsoncxx::builder::basic::kvp("cellScalarVersion",	_cellScalarVersion),
		bsoncxx::builder::basic::kvp("cellVectorID",		_cellVectorID),
		bsoncxx::builder::basic::kvp("cellVectorVersion",	_cellVectorVersion),
		bsoncxx::builder::basic::kvp("resultType",			(long long)getResultType())
	);
}

void EntityResultUnstructuredMeshData::readSpecificDataFromDataBase(bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	_numberPoints		= doc_view["numberPoints"].get_int64().value;
	_numberCells		= doc_view["numberCells"].get_int64().value;
	_pointScalarID		= doc_view["pointScalarID"].get_int64().value;
	_pointScalarVersion = doc_view["pointScalarVersion"].get_int64().value;
	_pointVectorID		= doc_view["pointVectorID"].get_int64().value;
	_pointVectorVersion = doc_view["pointVectorVersion"].get_int64().value;
	_cellScalarID		= doc_view["cellScalarID"].get_int64().value;
	_cellScalarVersion	= doc_view["cellScalarVersion"].get_int64().value;
	_cellVectorID		= doc_view["cellVectorID"].get_int64().value;
	_cellVectorVersion	= doc_view["cellVectorVersion"].get_int64().value;

	setResultType(static_cast<tResultType>((long long)doc_view["resultType"].get_int64()));
}
