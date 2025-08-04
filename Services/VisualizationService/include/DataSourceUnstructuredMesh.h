#pragma once

#include <string>

#include "DataSourceManagerItem.h"

#include <vtkUnstructuredGrid.h>
#include <vtkNew.h>

class EntityResultUnstructuredMesh;
class EntityResultUnstructuredMeshData;
class EntityResultUnstructuredMeshVtk;

class DataSourceUnstructuredMesh : public DataSourceManagerItem
{
public:
	DataSourceUnstructuredMesh();
	virtual ~DataSourceUnstructuredMesh();

	virtual bool loadData(EntityBase *resultEntity, EntityBase *meshEntity, ClassFactory* classFactory) override;

	vtkUnstructuredGrid* GetVtkGrid() { return vtkGrid; };

	double GetXMinCoordinate();
	double GetYMinCoordinate();
	double GetZMinCoordinate();
	double GetXMaxCoordinate();
	double GetYMaxCoordinate();
	double GetZMaxCoordinate();

	bool GetHasPointScalar() { return hasPointScalar; }
	bool GetHasPointVector() { return hasPointVector; }
	bool GetHasCellScalar() { return hasCellScalar; }
	bool GetHasCellVector() { return hasCellVector; }

private:
	bool loadData(EntityResultUnstructuredMeshData* resultEntity, EntityBase* meshEntity, ClassFactory* classFactory);
	bool loadData(EntityResultUnstructuredMeshVtk* resultData, ClassFactory* classFactory);

	bool loadMeshData(EntityBase* meshEntity, ClassFactory* classFactory);
	bool loadResultData(EntityBase* resultEntity, ClassFactory* classFactory);
	void FreeMemory();
	void buildScalarArray(size_t length, float* data, vtkNew<vtkDoubleArray>& dataArray);
	void buildVectorArray(size_t length, float* data, vtkNew<vtkDoubleArray>& dataArray);

	vtkUnstructuredGrid *vtkGrid;

	bool hasPointScalar = false;
	bool hasCellScalar = false;
	bool hasPointVector = false;
	bool hasCellVector = false;
};
