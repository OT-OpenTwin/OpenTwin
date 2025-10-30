// @otlicense
// File: VtkDriverCartesianFaceScalar.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end


#include "stdafx.h"

#include "VtkDriverCartesianFaceScalar.h"
#include "DataSourceManager.h"
#include "DataSourceManagerItem.h"
#include "DataSourceCartesianVector.h"
#include "EntityVis2D3D.h"
#include "EntityCartesianVector.h"
#include "EntityMeshCartesianData.h"

#include <osg/StateSet>
#include <osg/Node>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Switch>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/LightModel>
#include <osg/LightSource>
#include <osg/MatrixTransform>
#include <osg/Matrix>

#include <osg/Image>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

#include "vtk2osg.h"

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCylinderSource.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCallbackCommand.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyData.h>
#include <vtkBandedPolyDataContourFilter.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkStructuredGridReader.h>
#include <vtkPlane.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkCutter.h>
#include <vtkArrowSource.h>
#include <vtkGlyph3D.h>
#include <vtkLookupTable.h>
#include <vtkExtractRectilinearGrid.h>
#include <vtkDoubleArray.h>
#include <vtkRectilinearGridGeometryFilter.h>
#include <vtkCellData.h>

VtkDriverCartesianFaceScalar::VtkDriverCartesianFaceScalar() :
	normalDirection(0),
	cutIndex(0)
{

}

VtkDriverCartesianFaceScalar::~VtkDriverCartesianFaceScalar()
{

}

void VtkDriverCartesianFaceScalar::setProperties(EntityVis2D3D *visEntity)
{
	assert(visEntity->getClassName() == "EntityVisCartesianFaceScalar");

	// Plane
	EntityPropertiesSelection *normalProperty  = dynamic_cast<EntityPropertiesSelection*>(visEntity->getProperties().getProperty("Normal"));
	EntityPropertiesInteger   *centerXProperty = dynamic_cast<EntityPropertiesInteger*>(visEntity->getProperties().getProperty("Plane index X"));
	EntityPropertiesInteger   *centerYProperty = dynamic_cast<EntityPropertiesInteger*>(visEntity->getProperties().getProperty("Plane index Y"));
	EntityPropertiesInteger   *centerZProperty = dynamic_cast<EntityPropertiesInteger*>(visEntity->getProperties().getProperty("Plane index Z"));

	// Scaling
	EntityPropertiesBoolean *autoscaleProperty = dynamic_cast<EntityPropertiesBoolean*>(visEntity->getProperties().getProperty("Autoscale"));
	EntityPropertiesDouble  *minProperty       = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Min"));
	EntityPropertiesDouble  *maxProperty       = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Max"));
	EntityPropertiesBoolean *logscaleProperty  = dynamic_cast<EntityPropertiesBoolean*>(visEntity->getProperties().getProperty("Logscale"));

	if (normalProperty->getValue() == "X")
	{
		normalDirection = 0;
		cutIndex        = centerXProperty->getValue();
	}
	else if (normalProperty->getValue() == "Y")
	{
		normalDirection = 1;
		cutIndex        = centerYProperty->getValue();
	}
	else if (normalProperty->getValue() == "Z")
	{
		normalDirection = 2;
		cutIndex        = centerZProperty->getValue();
	}
	else
	{
		assert(0); // Unknown normal type
	}
}

std::string VtkDriverCartesianFaceScalar::buildSceneNode(DataSourceManagerItem *dataItem)
{
	std::time_t timer = time(nullptr);
	reportTime("VTK image creation started", timer);

	osg::Node *node = new osg::Switch;

	DataSourceCartesianVector *dataSource = dynamic_cast<DataSourceCartesianVector*>(dataItem);

	if (dataSource != nullptr)
	{
		// Get the values from the vector
		int nx = dataSource->getMeshData()->getNumberLinesX();
		int ny = dataSource->getMeshData()->getNumberLinesY();
		int nz = dataSource->getMeshData()->getNumberLinesZ();

		std::vector<double> values;
		dataSource->getResultData()->getValues(values);
		assert(values.size() == 3 * nx * ny * nz);

		bool primaryMesh = true;

		if (dataSource->getResultData()->getResultType() == EntityResultBase::CARTESIAN_FACES_3D)
		{
			primaryMesh = true;
		}
		else if (dataSource->getResultData()->getResultType() == EntityResultBase::CARTESIAN_DUAL_FACES_3D)
		{
			primaryMesh = false;
		}
		else
		{
			assert(0); // Unknown result type
		}

		// Determine the minimum and maximum values in the vector
		double minValue = values[0];
		double maxValue = values[0];

		for (double v : values)
		{
			minValue = std::min(minValue, v);
			maxValue = std::max(maxValue, v);
		}

		// Determine the transformation 
		size_t mx = 1;
		size_t my = nx;
		size_t mz = nx * ny;

		size_t np = nx * ny * nz;

		// Ensure that the cut-index is in a valid range
		if (cutIndex < 0) cutIndex = 1;

		// Determine the plane visualization in a local coordinate system
		switch (normalDirection)
		{
		case 0:
			// Add transformation to the node
			{
				osg::MatrixTransform* transform = new osg::MatrixTransform;
				transform->addChild(node);
				osg::Matrix permutation;
				permutation.set(0, 1, 0, 0,   0, 0, 1, 0,   1, 0, 0, 0,   0, 0, 0, 1);
				transform->setMatrix(permutation);

				// Now build the plane for x normal direction
				if (cutIndex >= nx) cutIndex = nx - 1;
				buildPlane(primaryMesh, cutIndex, ny, nz, nx, my, mz, mx, dataSource->getMeshData()->getMeshLinesY(), dataSource->getMeshData()->getMeshLinesZ(), dataSource->getMeshData()->getMeshLinesX(), 
						   values, 0, minValue, maxValue, node);

				node = transform;
			}
			break;
		case 1:
			{
				// Add transformation to the node
				osg::MatrixTransform* transform = new osg::MatrixTransform;
				transform->addChild(node);
				osg::Matrix permutation;
				permutation.set(0, 0, 1, 0,   1, 0, 0, 0,   0, 1, 0, 0,   0, 0, 0, 1);
				transform->setMatrix(permutation);

				// Now build the plane for y normal direction
				if (cutIndex >= ny) cutIndex = ny - 1;
				buildPlane(primaryMesh, cutIndex, nz, nx, ny, mz, mx, my, dataSource->getMeshData()->getMeshLinesZ(), dataSource->getMeshData()->getMeshLinesX(), dataSource->getMeshData()->getMeshLinesY(), 
						   values, np, minValue, maxValue, node);

				node = transform;
			}
			break;
		case 2:
			{
				// Build the plane for the z normal direction
				if (cutIndex >= nz) cutIndex = nz - 1;
				buildPlane(primaryMesh, cutIndex, nx, ny, nz, mx, my, mz, dataSource->getMeshData()->getMeshLinesX(), dataSource->getMeshData()->getMeshLinesY(), dataSource->getMeshData()->getMeshLinesZ(), 
						   values, 2 * np, minValue, maxValue, node);
			}
			break;
		default:
			assert(0); // Unknown direction
		}
	}

	// Now we serialize the node information and return it as a string
	std::stringstream dataOut;

	osgDB::ReaderWriter* readerWriter = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");

	if (readerWriter != nullptr)
	{
		readerWriter->writeNode(*node, dataOut, new osgDB::Options("Compressor=zlib"));
	}

	reportTime("VTK data serialized", timer);

	return dataOut.str();
}

void VtkDriverCartesianFaceScalar::buildPlane(bool primaryMesh, int iw, int nu, int nv, int nw, size_t mu, size_t mv, size_t mw, std::vector<double> &uc, std::vector<double> &vc, std::vector<double> &wc, 
											  std::vector<double> &values, size_t offset, double minValue, double maxValue, osg::Node *parent)
{
	vtkRectilinearGrid *grid = vtkRectilinearGrid::New();

	if (primaryMesh)
	{
		// Set up a primary grid
		grid->SetDimensions(nu, nv, 1);
		grid->SetExtent(0, nu - 1, 0, nv - 1, 0, 0);

		vtkDoubleArray *ucoord = vtkDoubleArray::New();
		vtkDoubleArray *vcoord = vtkDoubleArray::New();
		vtkDoubleArray *wcoord = vtkDoubleArray::New();

		for (double coord : uc) ucoord->InsertNextValue(coord);
		for (double coord : vc) vcoord->InsertNextValue(coord);
		wcoord->InsertNextValue(wc[iw]);

		grid->SetXCoordinates(ucoord);
		grid->SetYCoordinates(vcoord);
		grid->SetZCoordinates(wcoord);

		ucoord->Delete();
		vcoord->Delete();
		wcoord->Delete();

		vtkFloatArray *scalarData = vtkFloatArray::New();

		for (int iu = 0; iu < nu-1; iu++)
		{
			for (int iv = 0; iv < nv - 1; iv++)
			{
				size_t index2D = iu + iv * (nu-1);
				size_t index3D = iu * mu + iv * mv + iw * mw;

				scalarData->InsertTuple1(index2D, values[index3D + offset]);
			}
		}

		grid->GetCellData()->SetScalars(scalarData);
		grid->GetCellData()->Update();
		scalarData->Delete();
	}
	else
	{
		// Set up a dual grid
		grid->SetDimensions(nu+1, nv+1, 1);
		grid->SetExtent(0, nu, 0, nv, 0, 0);

		vtkDoubleArray *ucoord = vtkDoubleArray::New();
		vtkDoubleArray *vcoord = vtkDoubleArray::New();
		vtkDoubleArray *wcoord = vtkDoubleArray::New();

		ucoord->InsertNextValue(uc[0]);
		for (int iu = 1; iu < nu; iu++)
		{
			ucoord->InsertNextValue(0.5 * (uc[iu] + uc[iu - 1]));
		}
		ucoord->InsertNextValue(uc[nu-1]);

		vcoord->InsertNextValue(vc[0]);
		for (int iv = 1; iv < nv; iv++)
		{
			vcoord->InsertNextValue(0.5 * (vc[iv] + vc[iv - 1]));
		}
		vcoord->InsertNextValue(vc[nv-1]);

		if (iw == 0)
		{
			wcoord->InsertNextValue(wc[0]);
		}
		else if (iw == nw - 1)
		{
			wcoord->InsertNextValue(wc[nw-1]);
		}
		else
		{
			wcoord->InsertNextValue(0.5 * (wc[iw] + wc[iw-1]));
		}

		grid->SetXCoordinates(ucoord);
		grid->SetYCoordinates(vcoord);
		grid->SetZCoordinates(wcoord);

		ucoord->Delete();
		vcoord->Delete();
		wcoord->Delete();

		vtkFloatArray *scalarData = vtkFloatArray::New();

		for (int iu = 0; iu < nu; iu++)
		{
			for (int iv = 0; iv < nv; iv++)
			{
				size_t index2D = iu + iv * nu;
				size_t index3D = iu * mu + iv * mv + iw * mw;

				scalarData->InsertTuple1(index2D, values[index3D + offset]);
			}
		}

		grid->GetCellData()->SetScalars(scalarData);
		grid->GetCellData()->Update();
		scalarData->Delete();
	}

	vtkNew <vtkRectilinearGridGeometryFilter> plane;
	plane->SetInputData(grid);
	//plane->SetExtent(0, nu-1, 0, nv-1, 0, 0);

	//vtkNew<vtkLookupTable> lut;
	//lut->SetNumberOfTableValues( 100 ); 
	//lut->SetHueRange(0.0, 1.0);
	//lut->SetAlphaRange(1.0, 1.0);
	//lut->SetRange(0, 100);
	//lut->Build();

	vtkNew<vtkPolyDataMapper> mapperContour;
	mapperContour->SetInputConnection(plane->GetOutputPort());

	mapperContour->SetScalarRange(minValue, maxValue);
	mapperContour->SetScalarVisibility(true);
	mapperContour->SetScalarModeToUseCellData();
	mapperContour->CreateDefaultLookupTable();
	//mapper->SetLookupTable(lut);
	//mapper->UseLookupTableScalarRangeOn();

	vtkNew<vtkPolyDataMapper> mapperGrid;
	mapperGrid->SetInputData(plane->GetOutput());
	mapperGrid->SetScalarVisibility(false);

	vtkNew<vtkActor> actorContour;
	actorContour->SetMapper(mapperContour);

	osg::Node *contourNode = VTKActorToOSG(actorContour);

	dynamic_cast<osg::Switch *>(parent)->addChild(contourNode);

	vtkNew<vtkActor> actorGrid;
	actorGrid->SetMapper(mapperGrid);
	actorGrid->GetProperty()->SetRepresentationToWireframe();
	actorGrid->GetProperty()->SetEdgeVisibility(true);
	actorGrid->GetProperty()->SetEdgeColor(1.0, 1.0, 1.0);
	actorGrid->GetProperty()->SetLineWidth(1.5);

	osg::Node *gridNode = VTKActorToOSG(actorGrid);

	dynamic_cast<osg::Switch *>(parent)->addChild(gridNode);
}
