// @otlicense
// File: VtkDriverFarfield.cpp
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

#include "VtkDriverFarfield.h"
#include "DataSourceManager.h"
#include "DataSourceManagerItem.h"
#include "DataSourceVtkFarfield.h"
#include "OTModelEntities/EntityVis2D3D.h"
#include "OTModelEntities/EntityResultFarfield.h"

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
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkStructuredGridReader.h>
#include <vtkPlane.h>
#include <vtkStructuredGrid.h>
#include <vtkCutter.h>
#include <vtkArrowSource.h>
#include <vtkGlyph3D.h>
#include <vtkLookupTable.h>
#include <vtkGeometryFilter.h>
#include <vtkExtractRectilinearGrid.h>

VtkDriverFarfield::VtkDriverFarfield() 
{

}

VtkDriverFarfield::~VtkDriverFarfield()
{

}

void VtkDriverFarfield::setProperties(EntityVis2D3D *visEntity)
{
	EntityPropertiesDouble* cxProperty = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Structure center x"));
	EntityPropertiesDouble* cyProperty = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Structure center y"));
	EntityPropertiesDouble* czProperty = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Structure center z"));

	EntityPropertiesDouble* dProperty = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Structure diagonal"));

	EntityPropertiesDouble* scaleFactorProperty = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Scale factor"));
	EntityPropertiesBoolean* showEdgesProperty = dynamic_cast<EntityPropertiesBoolean*>(visEntity->getProperties().getProperty("Show edges"));

	if (cxProperty != nullptr && cyProperty != nullptr && czProperty != nullptr)
	{
		structureCx = cxProperty->getValue();
		structureCy = cyProperty->getValue();
		structureCz = czProperty->getValue();
	}

	if (dProperty != nullptr)
	{
		structureD = dProperty->getValue();
	}

	if (scaleFactorProperty != nullptr)
	{
		scaleFactor = scaleFactorProperty->getValue();
	}

	if (showEdgesProperty != nullptr)
	{
		showEdges = showEdgesProperty->getValue();
	}

	determinePlotType(visEntity);
}

void VtkDriverFarfield::determinePlotType(EntityVis2D3D* visEntity)
{
	EntityResultFarfield* farfield = dynamic_cast<EntityResultFarfield*>(visEntity);

	if (!farfield->getComplex())
	{
		plotComponent = MAGNITUDE;
		unit = "V/m";
	}
	else
	{
		EntityPropertiesSelection* typeProperty = dynamic_cast<EntityPropertiesSelection*>(visEntity->getProperties().getProperty("Type"));

		if (typeProperty != nullptr)
		{
			if (typeProperty->getValue() == "Magnitude")
			{
				plotComponent = MAGNITUDE;
				unit = "V/m";
			}
			else if (typeProperty->getValue() == "Phase")
			{
				plotComponent = PHASE;
				unit = "Degrees";
			}
			else if (typeProperty->getValue() == "Real")
			{
				plotComponent = REAL;
				unit = "V/m";
			}
			else if (typeProperty->getValue() == "Imaginary")
			{
				plotComponent = IMAGINARY;
				unit = "V/m";
			}
			else
			{
				plotComponent = MAGNITUDE;
				unit = "V/m";
				assert(0); // Unknown type set
			}
		}
		else
		{
			plotComponent = MAGNITUDE;
			unit = "V/m";
			assert(0); // No type property set
		}
	}
}

std::string VtkDriverFarfield::buildSceneNode(DataSourceManagerItem *dataItem, std::string& colorRamp)
{
	std::time_t timer = time(nullptr);
	reportTime("VTK image creation started", timer);

	osg::Node *node = new osg::Switch;

	DataSourceVtkFarfield *dataSource = dynamic_cast<DataSourceVtkFarfield*>(dataItem);

	vtkNew<vtkStructuredGrid> vtkGrid;
	buildVtkGrid(dataSource, vtkGrid);

	vtkNew<vtkGeometryFilter> geometryFilter;
	geometryFilter->SetInputData(vtkGrid);

	vtkNew<vtkPolyDataNormals> normals;
	normals->SetInputConnection(geometryFilter->GetOutputPort());
	normals->ComputePointNormalsOn();
	normals->ComputeCellNormalsOff();
	normals->SplittingOff();
	normals->ConsistencyOn();

	vtkNew<vtkPolyDataMapper> mapper;
	mapper->SetInputConnection(normals->GetOutputPort());
	mapper->ScalarVisibilityOn();
	mapper->SetScalarModeToUsePointData();
	mapper->SetColorModeToMapScalars();
	mapper->SetScalarRange(vtkGrid->GetScalarRange());

	mapper->Update();
	mapper->MapScalars(1.0);

	vtkLookupTable* lut = vtkLookupTable::SafeDownCast(mapper->GetLookupTable());

	if (lut != nullptr)
	{
		lut->Build();
		extractColorRampFromLookupTable(lut);
		colorRamp = colorRampData;
	}

	vtkNew<vtkActor> actor;
	actor->SetMapper(mapper);
	
	vtkProperty* property = actor->GetProperty();

	property->SetRepresentationToSurface();
	property->LightingOn();
	property->SetInterpolationToPhong();

	property->SetAmbient(0.15);
	property->SetDiffuse(0.75);
	property->SetSpecular(0.03);
	property->SetSpecularPower(50.0);

	property->SetEdgeVisibility(showEdges);
	property->SetEdgeColor(0.1, 0.1, 0.1);
	property->SetLineWidth(1.0);

	osg::Node* plotNode = VTKActorToOSG(actor);

	// Now we move and scale the farfield visualization to the mesh center and the structure diagonal
	double maxRadius = getFarfieldMaxRadius(vtkGrid);

	const double scale = (0.5 * structureD) / maxRadius * scaleFactor;

	osg::ref_ptr<osg::MatrixTransform> farFieldTransform = new osg::MatrixTransform();

	farFieldTransform->setMatrix(osg::Matrix::scale(scale, scale, scale) * osg::Matrix::translate(structureCx, structureCy, structureCz));

	farFieldTransform->addChild(plotNode);
	farFieldTransform->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

	dynamic_cast<osg::Switch*>(node)->addChild(farFieldTransform);

	reportTime("VTK data converted to OSG data", timer);

	// Now we serialize and deserialize the node information for testing

	std::stringstream dataOut;

	osgDB::ReaderWriter* rw1 = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
	if (rw1)
	{
		rw1->writeNode(*node, dataOut, new osgDB::Options("Compressor=zlib"));
	}

	reportTime("VTK data serialized", timer);

	return dataOut.str();
}

double VtkDriverFarfield::getPlotComponent(std::complex<double>& value)
{
	switch (plotComponent)
	{
	case MAGNITUDE:
		return std::abs(value);
		break;
	case PHASE:
		return vtkMath::DegreesFromRadians(std::arg(value));
		break;
	case REAL:
		return std::real(value);
		break;
	case IMAGINARY:
		return std::imag(value);
		break;
	}

	assert(0); // Unknown component
	return 0.0;
}

void VtkDriverFarfield::buildVtkGrid(DataSourceVtkFarfield* dataSource, vtkStructuredGrid * vtkGrid)
{
	// Store the points in the vtk data structure
	const int nTheta = static_cast<int>(dataSource->getTheta().size());
	const int nPhi = static_cast<int>(dataSource->getPhi().size());

	vtkNew<vtkPoints> points;
	points->SetDataTypeToDouble();

	for (int j = 0; j < nPhi; ++j)
	{
		const double p = vtkMath::RadiansFromDegrees(dataSource->getPhi()[j]);

		for (int i = 0; i < nTheta; ++i)
		{
			const double t = vtkMath::RadiansFromDegrees(dataSource->getTheta()[i]);
			const double r = getPlotComponent(dataSource->getRadius()[i][j]);

			points->InsertNextPoint(
				r * std::sin(t) * std::cos(p),
				r * std::sin(t) * std::sin(p),
				r * std::cos(t));
		}
	}

	vtkGrid->SetDimensions(nTheta, nPhi, 1);
	vtkGrid->SetPoints(points);

	vtkNew<vtkDoubleArray> scalars;
	scalars->SetName("Radius");
	scalars->SetNumberOfComponents(1);

	for (int j = 0; j < nPhi; ++j)
	{
		for (int i = 0; i < nTheta; ++i)
		{
			const double r = getPlotComponent(dataSource->getRadius()[i][j]);

			scalars->InsertNextValue(r);
		}
	}

	vtkGrid->GetPointData()->SetScalars(scalars);
}

double VtkDriverFarfield::getFarfieldMaxRadius(vtkStructuredGrid* grid)
{
	double maxRadius = 0.0;

	for (vtkIdType i = 0; i < grid->GetNumberOfPoints(); ++i)
	{
		double p[3];
		grid->GetPoint(i, p);

		const double radius = std::hypot(std::hypot(p[0], p[1]), p[2]);

		maxRadius = std::max(maxRadius, radius);
	}

	return maxRadius;
}
