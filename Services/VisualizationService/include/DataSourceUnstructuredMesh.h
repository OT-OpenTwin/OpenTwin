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

	double GetXMinCoordinate();
	double GetYMinCoordinate();
	double GetZMinCoordinate();
	double GetXMaxCoordinate();
	double GetYMaxCoordinate();
	double GetZMaxCoordinate();

private:
	bool loadMeshData(EntityBase* meshEntity);
	bool loadResultData(EntityBase* resultEntity);
	void FreeMemory();
	void buildScalarArray(size_t length, float* data, vtkNew<vtkDoubleArray>& dataArray);
	void buildVectorArray(size_t length, float* data, vtkNew<vtkDoubleArray>& dataArray);

	vtkNew<vtkUnstructuredGrid> vtkGrid;
};
