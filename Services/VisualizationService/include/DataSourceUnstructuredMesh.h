#pragma once

#include <string>

#include "DataSourceManagerItem.h"

#include <vtkUnstructuredGrid.h>
#include <vtkNew.h>

class EntityResultUnstructuredMesh;
class EntityResultUnstructuredMeshData;

class DataSourceUnstructuredMesh : public DataSourceManagerItem
{
public:
	DataSourceUnstructuredMesh();
	virtual ~DataSourceUnstructuredMesh();

	virtual bool loadData(EntityBase *resultEntity, EntityBase *meshEntity) override;

	vtkUnstructuredGrid* GetVtkGrid() { return vtkGrid.GetPointer(); };

	double GetXMinCoordinate() { return _xMinCoordinate; }
	double GetYMinCoordinate() { return _yMinCoordinate; }
	double GetZMinCoordinate() { return _zMinCoordinate; }
	double GetXMaxCoordinate() { return _xMaxCoordinate; }
	double GetYMaxCoordinate() { return _yMaxCoordinate; }
	double GetZMaxCoordinate() { return _zMaxCoordinate; }

private:
	bool loadMeshData(EntityBase* meshEntity);
	bool loadResultData(EntityBase* resultEntity);
	void FreeMemory();
	void buildVectorArray(size_t length, float* data, vtkNew<vtkDoubleArray>& dataArray);

	vtkNew<vtkUnstructuredGrid> vtkGrid;

	double _xMinCoordinate;
	double _yMinCoordinate;
	double _zMinCoordinate;
	double _xMaxCoordinate;
	double _yMaxCoordinate;
	double _zMaxCoordinate;
};
