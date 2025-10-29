// @otlicense

#include "../include/DataSourceResult3D.h"


#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>
#include <vtkExtractRectilinearGrid.h>
#include <vtkRectilinearGridGeometryFilter.h>
#include <vtkPointData.h>

#include <memory>


DataSourceResult3D::DataSourceResult3D(){}

DataSourceResult3D::~DataSourceResult3D()
{
	FreeMemory();
}

bool DataSourceResult3D::loadData(EntityBase * resultEntity, EntityBase *meshEntity)
{
	FreeMemory();
	auto resultData = dynamic_cast<EntityResult3DData *>(resultEntity);
	assert(resultEntity != nullptr);
	entityResultData3D = resultData;
	resultData->LoadAllData();

	std::unique_ptr<EntityMeshCartesianData> meshData(resultData->GetMeshData());

	setResultType(resultData->getResultType());
	
	SetGridDimensions(meshData.get());
	
	size_t vectorLength1, vectorLength2;
	std::unique_ptr<const double[]> xComponent(resultData->GetXComponentData(vectorLength1));
	std::unique_ptr<const double[]> yComponent(resultData->GetYComponentData(vectorLength2));
	assert(vectorLength1 == vectorLength2);
	std::unique_ptr<const double[]> zComponent(resultData->GetZComponentData(vectorLength1));
	assert(vectorLength1 == vectorLength2);
	
	SetGridValues(xComponent.get(), yComponent.get(), zComponent.get(), vectorLength1);

	   
	return true;
}


void DataSourceResult3D::SetGridValues(const double * xComponent, const double * yComponent, const double * zComponent, size_t size)
{
	//Set vector values
	vtkNew<vtkDoubleArray> vectorData;
	try
	{
		vectorData->SetNumberOfComponents(3);
		vectorData->SetNumberOfTuples(size);
		for (int i = 0; i < size; i++)
		{
			vectorData->SetTuple3(i, xComponent[i], yComponent[i], zComponent[i]);
		}

		vtkGrid->GetPointData()->SetVectors(vectorData);
		vtkGrid->GetPointData()->Update();
	}
	catch (...)
	{
		if (vectorData != nullptr)
		{
			vectorData->Delete();
		}
	}
}

void DataSourceResult3D::SetGridDimensions(EntityMeshCartesianData * meshData)
{
	bool primaryMash;
	if (getResultType() == EntityResultBase::CARTESIAN_DUAL_NODE_VECTORS)
	{
		primaryMash = false;
	}
	else if (getResultType() == EntityResultBase::CARTESIAN_NODE_VECTORS)
	{
		primaryMash = true;
	}
	else
	{
		throw std::invalid_argument("Not supported result type");
	}
	size_t numberOfLinesX =meshData->getNumberLinesX();
	size_t numberOfLinesY =meshData->getNumberLinesY();
	size_t numberOfLinesZ =meshData->getNumberLinesZ();

	vtkGrid->SetDimensions((int) numberOfLinesX, (int) numberOfLinesY, (int) numberOfLinesZ);

	std::vector<double> xLines = meshData->getMeshLinesX();
	std::vector<double> yLines = meshData->getMeshLinesY();
	std::vector<double> zLines = meshData->getMeshLinesZ();

	vtkNew<vtkDoubleArray> xcoord;
	xcoord->Resize(numberOfLinesX);
	vtkNew<vtkDoubleArray> ycoord; 
	ycoord->Resize(numberOfLinesY);
	vtkNew<vtkDoubleArray> zcoord; 
	zcoord->Resize(numberOfLinesZ);


	for (double coord : xLines)
	{
		xcoord->InsertNextValue(coord);
	}
	for (double coord : yLines)
	{
		ycoord->InsertNextValue(coord);
	}
	for (double coord : zLines)
	{
		zcoord->InsertNextValue(coord);
	}

	vtkGrid->SetXCoordinates(xcoord);
	vtkGrid->SetYCoordinates(ycoord);
	vtkGrid->SetZCoordinates(zcoord);

	double xDiscretization = FindMinimumDiscretization(xLines);
	double yDiscretization = FindMinimumDiscretization(yLines);
	double zDiscretization = FindMinimumDiscretization(zLines);

	double xEps = xDiscretization / 1000;
	double yEps = yDiscretization/ 1000;
	double zEps = zDiscretization/ 1000;

	_xMinCoordinate = xLines[0] + xEps;
	_yMinCoordinate = zLines[0] + yEps;
	_zMinCoordinate = yLines[0] + zEps;
	_xMaxCoordinate = xLines.back();
	_yMaxCoordinate = yLines.back();
	_zMaxCoordinate = zLines.back();

}

const double DataSourceResult3D::FindMinimumDiscretization(std::vector<double>& coordinates)
{
	double minDistance = 0;
	for (int i = 0; i < coordinates.size() - 1; i++)
	{
		double distance = coordinates[i + 1] - coordinates[i];
		if (distance < minDistance)
		{
			minDistance = distance;
		}
	}
	return minDistance;
}

void DataSourceResult3D::FreeMemory(void)
{
	//if (vtkGrid != nullptr)
	//{
	//	vtkGrid->Delete();
	//	vtkGrid = nullptr;
	//}
	if (entityResultData3D != nullptr)
	{
		delete entityResultData3D;
		entityResultData3D = nullptr;
	}

}
