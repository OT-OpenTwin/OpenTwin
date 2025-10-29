// @otlicense

#pragma once
#include "DataSourceManagerItem.h"
#include "EntityCompressedVector.h"
#include "EntityMeshCartesianData.h"
#include "EntityResultBase.h"
#include "EntityResult3DData.h"

#include <vector>
#include <vtkRectilinearGrid.h>
#include <vtkNew.h>

class DataSourceResult3D : public DataSourceManagerItem, public EntityResultBase
{
public:
	DataSourceResult3D();
	virtual ~DataSourceResult3D();

	virtual bool loadData(EntityBase *resultEntity, EntityBase *meshEntity) override;

	vtkRectilinearGrid * GetVtkGrid() { return vtkGrid.GetPointer(); };
		
	double GetXMinCoordinate() { return _xMinCoordinate; }
	double GetYMinCoordinate() { return _yMinCoordinate; }
	double GetZMinCoordinate() { return _zMinCoordinate; }
	double GetXMaxCoordinate() { return _xMaxCoordinate; }
	double GetYMaxCoordinate() { return _yMaxCoordinate; }
	double GetZMaxCoordinate() { return _zMaxCoordinate; }

private:
	EntityResult3DData * entityResultData3D = nullptr;
	vtkNew<vtkRectilinearGrid> vtkGrid;
	
	void SetGridValues(const double * xComponent, const double * yComponent, const double * zComponent, size_t size);
	void SetGridDimensions(EntityMeshCartesianData * meshData);
	const double FindMinimumDiscretization(std::vector<double> & coordinates);
	void FreeMemory(void);

	double _xMinCoordinate;
	double _yMinCoordinate;
	double _zMinCoordinate;
	double _xMaxCoordinate;
	double _yMaxCoordinate;
	double _zMaxCoordinate;
		
};