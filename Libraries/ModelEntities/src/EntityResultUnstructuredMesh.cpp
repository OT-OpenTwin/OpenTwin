#pragma once
#include "../include/EntityResultUnstructuredMesh.h"
#include "DataBase.h"

static EntityFactoryRegistrar<EntityResultUnstructuredMesh> registrar("EntityResultUnstructuredMesh");

EntityResultUnstructuredMesh::EntityResultUnstructuredMesh(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, const std::string & owner)
	:EntityBase(ID,parent,obs,ms,owner)
{}

EntityResultUnstructuredMesh::~EntityResultUnstructuredMesh()
{
	clearAllBinaryData();
}

void EntityResultUnstructuredMesh::clearAllBinaryData()
{
	if (_xcoord != nullptr)
	{
		delete _xcoord;
		_xcoord = nullptr;
	}

	if (_ycoord != nullptr)
	{
		delete _ycoord;
		_ycoord = nullptr;
	}
	
	if (_zcoord != nullptr)
	{
		delete _zcoord;
		_zcoord = nullptr;
	}

	if (_cellData != nullptr)
	{
		delete _cellData;
		_cellData = nullptr;
	}
}

bool EntityResultUnstructuredMesh::getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax)
{
	return false;
}

void EntityResultUnstructuredMesh::setMeshData(size_t numberPoints, size_t numberCells, size_t sizeCellData, EntityBinaryData *&xcoord, EntityBinaryData *&ycoord, EntityBinaryData *&zcoord, EntityBinaryData *&cellData)
{
	clearAllBinaryData();

	_numberPoints = numberPoints;
	_numberCells  = numberCells;
	_sizeCellData = sizeCellData;

	_xcoord = xcoord;
	_ycoord = ycoord;
	_zcoord = zcoord;

	_cellData = cellData;

	updateIDFromObjects();

	xcoord = ycoord = zcoord = cellData = nullptr;
}

void EntityResultUnstructuredMesh::GetPointCoordData(size_t & numberPoints, double *&x, double *&y, double *&z)
{
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIds;

	if (_xcoord == nullptr)
	{
		if (_xCoordDataID == -1 || _xCoordDataVersion == -1)
		{
			throw std::invalid_argument("Binary data xCoord not existing.");
		}
		prefetchIds.push_back(std::pair<unsigned long long, unsigned long long>(_xCoordDataID, _xCoordDataVersion));
	}

	if (_ycoord == nullptr)
	{
		if (_yCoordDataID == -1 || _yCoordDataVersion == -1)
		{
			throw std::invalid_argument("Binary data yCoord not existing.");
		}
		prefetchIds.push_back(std::pair<unsigned long long, unsigned long long>(_yCoordDataID, _yCoordDataVersion));
	}

	if (_zcoord == nullptr)
	{
		if (_zCoordDataID == -1 || _zCoordDataVersion == -1)
		{
			throw std::invalid_argument("Binary data zCoord not existing.");
		}
		prefetchIds.push_back(std::pair<unsigned long long, unsigned long long>(_zCoordDataID, _zCoordDataVersion));
	}

	if (!prefetchIds.empty())
	{
		DataBase::instance().prefetchDocumentsFromStorage(prefetchIds);
	}

	if (_xcoord == nullptr)
	{
		std::map<ot::UID, EntityBase *> entityMap;
		_xcoord = dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, _xCoordDataID, _xCoordDataVersion, entityMap));
		assert(_xcoord != nullptr);
	}

	if (_ycoord == nullptr)
	{
		std::map<ot::UID, EntityBase*> entityMap;
		_ycoord = dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, _yCoordDataID, _yCoordDataVersion, entityMap));
		assert(_ycoord != nullptr);
	}

	if (_zcoord == nullptr)
	{
		std::map<ot::UID, EntityBase*> entityMap;
		_zcoord = dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, _zCoordDataID, _zCoordDataVersion, entityMap));
		assert(_zcoord != nullptr);
	}

	std::vector<char> xBytes = _xcoord->getData();
	std::vector<char> yBytes = _ycoord->getData();
	std::vector<char> zBytes = _zcoord->getData();

	assert(xBytes.size() == yBytes.size());
	assert(xBytes.size() == zBytes.size());

	numberPoints = yBytes.size() / sizeof(double);
	assert(numberPoints == _numberPoints);

	x = new double[numberPoints];
	y = new double[numberPoints];
	z = new double[numberPoints];

	for (size_t index = 0; index < numberPoints; index++)
	{
		x[index] = ((double*)xBytes.data())[index];
		y[index] = ((double*)yBytes.data())[index];
		z[index] = ((double*)zBytes.data())[index];
	}
}

void EntityResultUnstructuredMesh::GetCellData(size_t& numberCells, size_t& sizeCellData, int*& cells)
{
	if (_cellData == nullptr)
	{
		if (_cellDataID == -1 || _cellDataVersion == -1)
		{
			throw std::invalid_argument("Binary data cellData not existing.");
		}
		std::map<ot::UID, EntityBase*> entityMap;
		_cellData = dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, _cellDataID, _cellDataVersion, entityMap));
		assert(_cellData != nullptr);
	}

	std::vector<char> cellBytes = _cellData->getData();

	sizeCellData = cellBytes.size() / sizeof(int);
	assert(sizeCellData == _sizeCellData);

	numberCells = _numberCells;

	cells = new int[sizeCellData];

	for (size_t index = 0; index < sizeCellData; index++)
	{
		cells[index] = ((int*)cellBytes.data())[index];
	}
}

void EntityResultUnstructuredMesh::updateIDFromObjects(void)
{
	if (_xcoord != nullptr)
	{
		_xCoordDataID = _xcoord->getEntityID();
		_xCoordDataVersion = _xcoord->getEntityStorageVersion();
	}

	if (_ycoord != nullptr)
	{
		_yCoordDataID = _ycoord->getEntityID();
		_yCoordDataVersion = _ycoord->getEntityStorageVersion();
	}

	if (_zcoord != nullptr)
	{
		_zCoordDataID = _zcoord->getEntityID();
		_zCoordDataVersion = _zcoord->getEntityStorageVersion();
	}

	if (_cellData != nullptr)
	{
		_cellDataID = _cellData->getEntityID();
		_cellDataVersion = _cellData->getEntityStorageVersion();
	}
}

void EntityResultUnstructuredMesh::addStorageData(bsoncxx::builder::basic::document & storage)
{
	EntityBase::addStorageData(storage);

	updateIDFromObjects();

	assert(_xCoordDataID != -1 && _xCoordDataVersion != -1);
	assert(_yCoordDataID != -1 && _yCoordDataVersion != -1);
	assert(_zCoordDataID != -1 && _zCoordDataVersion != -1);
	assert(_cellDataID   != -1 && _cellDataVersion   != -1);

	storage.append(
		bsoncxx::builder::basic::kvp("numberPoints",		_numberPoints),
		bsoncxx::builder::basic::kvp("numberCells",			_numberCells),
		bsoncxx::builder::basic::kvp("sizeCellData",		_sizeCellData),
		bsoncxx::builder::basic::kvp("xCoordDataID",		_xCoordDataID),
		bsoncxx::builder::basic::kvp("xCoordDataVersion",	_xCoordDataVersion),
		bsoncxx::builder::basic::kvp("yCoordDataID",		_yCoordDataID),
		bsoncxx::builder::basic::kvp("yCoordDataVersion",	_yCoordDataVersion),
		bsoncxx::builder::basic::kvp("zCoordDataID",		_zCoordDataID),
		bsoncxx::builder::basic::kvp("zCoordDataVersion",	_zCoordDataVersion), 
		bsoncxx::builder::basic::kvp("cellDataID",			_cellDataID),
		bsoncxx::builder::basic::kvp("cellDataVersion",		_cellDataVersion)
	);
}

void EntityResultUnstructuredMesh::readSpecificDataFromDataBase(bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	_numberPoints		= doc_view["numberPoints"].get_int64().value;
	_numberCells		= doc_view["numberCells"].get_int64().value;
	_sizeCellData		= doc_view["sizeCellData"].get_int64().value;

	_xCoordDataID		= doc_view["xCoordDataID"].get_int64().value;
	_xCoordDataVersion	= doc_view["xCoordDataVersion"].get_int64().value;
	_yCoordDataID		= doc_view["yCoordDataID"].get_int64().value;
	_yCoordDataVersion	= doc_view["yCoordDataVersion"].get_int64().value;
	_zCoordDataID		= doc_view["zCoordDataID"].get_int64().value;
	_zCoordDataVersion	= doc_view["zCoordDataVersion"].get_int64().value;

	_cellDataID			= doc_view["cellDataID"].get_int64().value;
	_cellDataVersion	= doc_view["cellDataVersion"].get_int64().value;
}
