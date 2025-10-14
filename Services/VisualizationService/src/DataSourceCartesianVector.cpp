
#include "stdafx.h"

#include "DataSourceCartesianVector.h"
#include "EntityBase.h"
#include "EntityCartesianVector.h"
#include "EntityMeshCartesianData.h"

#include <vtkNew.h>
#include <vtkStructuredGrid.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGridReader.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkCell.h>
#include <vtkCellData.h>


DataSourceCartesianVector::DataSourceCartesianVector() : 
	resultData(nullptr),
	meshData(nullptr)
{

}

DataSourceCartesianVector::~DataSourceCartesianVector()
{
	if (resultData != nullptr)
	{
		delete resultData;
		resultData = nullptr;
	}

	if (meshData != nullptr)
	{
		delete meshData;
		meshData = nullptr;
	}
}

bool DataSourceCartesianVector::loadData(EntityBase *resultEntity, EntityBase *meshEntity)
{
	assert(resultData == nullptr);
	assert(meshData == nullptr);

	// First, load the data from the data base
	resultData = dynamic_cast<EntityCartesianVector *>(resultEntity);
	if (resultData == nullptr)
	{
		assert(0); // Wrong data type
		return false;
	}

	meshData = dynamic_cast<EntityMeshCartesianData*>(meshEntity);

	if (meshData == nullptr)
	{
		delete resultData;
		resultData = nullptr;

		assert(0); // Unable to read the mesh data
		return false;
	}

	return true; // successful loading
}

