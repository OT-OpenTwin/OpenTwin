// @otlicense
// File: VtkDriverUnstructuredScalarSurface.cpp
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

#include "VtkDriverUnstructuredScalarSurface.h"
#include "DataSourceManager.h"
#include "DataSourceManagerItem.h"
#include "DataSourceUnstructuredMesh.h"

#include "EntityVis2D3D.h"
#include "EntityMeshCartesianData.h"
#include "PlaneProperties.h"
#include "PropertiesVis2D3D.h"

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
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkCellDataToPointData.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPlane.h>
#include <vtkCutter.h>
#include <vtkSphereSource.h>
#include <vtkGlyph3D.h>
#include <vtkInformation.h>
#include <vtkLookupTable.h>
#include <vtkDoubleArray.h>
#include <vtkCellData.h>
#include <vtkFeatureEdges.h>
#include <vtkLookupTable.h>
#include <vtkMaskPoints.h>
#include <vtkBandedPolyDataContourFilter.h>
#include <vtkGeometryFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkPlaneCutter.h>
#include <vtkContourFilter.h>
#include <vtkPlanes.h>
#include <vtkDataArray.h>
#include <vtkExtractEdges.h>

VtkDriverUnstructuredScalarSurface::VtkDriverUnstructuredScalarSurface() {}

VtkDriverUnstructuredScalarSurface::~VtkDriverUnstructuredScalarSurface()
{
	DeletePropertyData();
}

void VtkDriverUnstructuredScalarSurface::CheckForModelUpdates()
{
	bool requiresModelUpdate = scalingData->UpdateMinMaxProperties(scalarRange[0], scalarRange[1]);

	if (requiresModelUpdate)
	{
		long long entityID, entityVersion;
		scalingData->GetEntityProperties(entityID, entityVersion);
		updateTopoEntityID.push_back(entityID);
		updateTopoEntityVersion.push_back(entityVersion);
		updateTopoForceVisibility.push_back(true);
	}
}

void VtkDriverUnstructuredScalarSurface::DeletePropertyData(void)
{
	if (scalingData != nullptr)
	{
		delete scalingData;
		scalingData = nullptr;
	}

	if (visData != nullptr)
	{
		delete visData;
		visData = nullptr;
	}
}

std::string VtkDriverUnstructuredScalarSurface::buildSceneNode(DataSourceManagerItem* dataItem)
{
	objectsToDelete.clear();

	updateTopoEntityID.clear();
	updateTopoEntityVersion.clear();

	std::time_t timer = time(nullptr);
	reportTime("VTK image creation started", timer);

	osg::Node* node = new osg::Switch;

	dataSource = dynamic_cast<DataSourceUnstructuredMesh*>(dataItem);

	if (dataSource != nullptr)
	{
		vtkNew<vtkCellDataToPointData> cellToPoint;

		dataConnection = nullptr;

		if (dataSource->GetHasCellScalar())
		{
			cellToPoint->SetInputData(dataSource->GetVtkGrid());
			cellToPoint->ProcessAllArraysOn();
			cellToPoint->Update();

			scalarRange = cellToPoint->GetOutput()->GetScalarRange();

			dataConnection = cellToPoint->GetOutputPort();
		}
		else
		{
			scalarRange = dataSource->GetVtkGrid()->GetScalarRange();
		}

		AssembleNode(node);

		CheckForModelUpdates();
	}

	for (auto item : objectsToDelete)
	{
		item->Delete();
	}
	objectsToDelete.clear();

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

vtkAlgorithmOutput* VtkDriverUnstructuredScalarSurface::GetPointSource(void)
{
	vtkNew<vtkSphereSource> sphere;

	sphere->SetRadius(0.1);
	sphere->SetPhiResolution(6);
	sphere->SetThetaResolution(12);

	vtkPolyDataNormals* shadedSphere = vtkPolyDataNormals::New();
	objectsToDelete.push_back(shadedSphere);

	shadedSphere->SetInputConnection(sphere->GetOutputPort());
	shadedSphere->SetFeatureAngle(80.0);
	shadedSphere->FlipNormalsOff();
	shadedSphere->Update();

	return shadedSphere->GetOutputPort();
}

void VtkDriverUnstructuredScalarSurface::AssembleNode(osg::Node* parent)
{
	if (visData->GetSelectedVisType() == PropertiesVisUnstructuredScalarSurface::VisualizationType::Contour2D)
	{
		AddNodeContour(parent);
	}
	else if (visData->GetSelectedVisType() == PropertiesVisUnstructuredScalarSurface::VisualizationType::Points)
	{
		AddNodePoints(parent);
	}
	else
	{
		assert(0); // Unknown type
	}
}

void VtkDriverUnstructuredScalarSurface::AddNodeContour(osg::Node* parent)
{
	vtkNew<vtkGeometryFilter> filter;
	if (dataConnection != nullptr) filter->SetInputConnection(dataConnection);
	else filter->SetInputData(dataSource->GetVtkGrid());

	if (visData->GetShow2dMesh())
	{
		vtkNew<vtkExtractEdges> edges;
		edges->SetInputConnection(filter->GetOutputPort());

		vtkNew<vtkPolyDataMapper> edgesMapper;
		edgesMapper->SetInputConnection(edges->GetOutputPort());
		edgesMapper->ScalarVisibilityOff();

		vtkNew<vtkActor> edgesActor;
		edgesActor->SetMapper(edgesMapper);
		edgesActor->GetProperty()->SetColor(visData->GetColor2dMeshR(), visData->GetColor2dMeshG(), visData->GetColor2dMeshB());

		osg::Node* planeNode = VTKActorToOSG(edgesActor);
		dynamic_cast<osg::Switch*>(parent)->addChild(planeNode);
	}

	vtkNew<vtkBandedPolyDataContourFilter> bf;
	bf->SetInputConnection(filter->GetOutputPort());
	bf->SetGenerateContourEdges(true);
	bf->SetScalarModeToValue();
	bf->GenerateValues(scalingData->GetColourResolution(), scalarRange);
	bf->Update();

	vtkNew<vtkPolyDataNormals> shadedContour;
	shadedContour->SetInputConnection(bf->GetOutputPort());
	shadedContour->SetFeatureAngle(45.0);
	//shadedContour->AutoOrientNormalsOn();  // This feature cannot be used if the surfaces are not closed surfaces (e.g. 2D data).

	if (visData->GetShow2dIsolines())
	{
		vtkNew<vtkPolyDataMapper> edgeMapper;
		edgeMapper->SetInputData(bf->GetContourEdgesOutput());
		edgeMapper->SetResolveCoincidentTopologyToPolygonOffset();

		vtkNew<vtkActor> edgeActor;
		edgeActor->SetMapper(edgeMapper);
		edgeActor->GetProperty()->SetColor(visData->GetColor2dIsolinesR(), visData->GetColor2dIsolinesG(), visData->GetColor2dIsolinesB());

		osg::Node* edgeNode = VTKActorToOSG(edgeActor);
		dynamic_cast<osg::Switch*>(parent)->addChild(edgeNode);
	}

	vtkNew<vtkPolyDataMapper> scalarFieldMapper;
	scalarFieldMapper->SetInputConnection(shadedContour->GetOutputPort());
	SetColouring(scalarFieldMapper);
	scalarFieldMapper->UseLookupTableScalarRangeOn();
	scalarFieldMapper->SetScalarModeToUseCellData();

	vtkNew<vtkActor> scalarFieldActor;
	scalarFieldActor->SetMapper(scalarFieldMapper);

	osg::Node* cutNode = VTKActorToOSG(scalarFieldActor);
	dynamic_cast<osg::Switch*>(parent)->addChild(cutNode);
}

void VtkDriverUnstructuredScalarSurface::AddNodePoints(osg::Node* parent)
{
	vtkNew<vtkMaskPoints> downSampling;

	if (dataConnection != nullptr) downSampling->SetInputConnection(dataConnection);
	else downSampling->SetInputData(dataSource->GetVtkGrid());

	downSampling->SetMaximumNumberOfPoints(std::max(1, visData->GetMaxPoints()));
	downSampling->SetRandomModeType(5);
	downSampling->Update();

	scalarRange = downSampling->GetOutput()->GetScalarRange();

	vtkNew<vtkPolyDataMapper> scalarFieldMapper;
	vtkNew<vtkGlyph3D> glyph;

	glyph->SetSourceConnection(GetPointSource());
	glyph->SetInputConnection(downSampling->GetOutputPort());
	glyph->ScalingOn();
	glyph->SetColorModeToColorByScalar();
	glyph->SetScaleModeToScaleByScalar();
	double normalization = std::abs(scalarRange[1]);

	double dx = dataSource->GetVtkGrid()->GetBounds()[1] - dataSource->GetVtkGrid()->GetBounds()[0];
	double dy = dataSource->GetVtkGrid()->GetBounds()[3] - dataSource->GetVtkGrid()->GetBounds()[2];
	double dz = dataSource->GetVtkGrid()->GetBounds()[5] - dataSource->GetVtkGrid()->GetBounds()[5];

	double pointRadius = 0.1 * sqrt(dx * dx + dy * dy + dz * dz);

	if (normalization != 0)
	{
		glyph->SetScaleFactor(pointRadius * visData->GetPointScale() / normalization);
	}
	else
	{
		glyph->SetScaleFactor(pointRadius * visData->GetPointScale());
	}
	glyph->OrientOn();
	glyph->Update();

	scalarFieldMapper->SetInputConnection(glyph->GetOutputPort());

	SetColouring(scalarFieldMapper);
	scalarFieldMapper->UseLookupTableScalarRangeOn();
	scalarFieldMapper->SetScalarModeToUsePointData();
	scalarFieldMapper->SetColorModeToMapScalars();

	vtkNew<vtkActor> scalarFieldActor;
	scalarFieldActor->SetMapper(scalarFieldMapper);

	osg::Node* cutNode = VTKActorToOSG(scalarFieldActor);
	dynamic_cast<osg::Switch*>(parent)->addChild(cutNode);
}


void VtkDriverUnstructuredScalarSurface::SetColouring(vtkPolyDataMapper* mapper)
{
	vtkNew<vtkLookupTable> lut;
	lut->SetNumberOfTableValues(scalingData->GetColourResolution());
	lut->SetHueRange(.667, 0.0);
	lut->SetAlphaRange(1., 1.);
	lut->IndexedLookupOff();
	lut->SetVectorModeToMagnitude();

	assert(scalingData != nullptr);
	assert(scalarRange != nullptr);

	auto scalingMethod = scalingData->GetScalingMethod();
	double minVal, maxVal;
	if (scalingMethod == ScalingProperties::ScalingMethod::rangeScale)
	{

		minVal = scalingData->GetRangeMin();
		maxVal = scalingData->GetRangeMax();
		if (minVal > maxVal)
		{
			//ToDo: UI display message
			minVal = maxVal;
		}
	}
	else if (scalingMethod == ScalingProperties::ScalingMethod::autoScale)
	{
		minVal = scalarRange[0];
		maxVal = scalarRange[1];
	}
	else
	{
		throw std::invalid_argument("Not supported scaling method");
	}

	auto scalingFunction = scalingData->GetScalingFunction();
	if (scalingFunction == ScalingProperties::ScalingFunction::linScale)
	{
		lut->SetScaleToLinear();
	}
	else if (scalingFunction == ScalingProperties::ScalingFunction::logScale)
	{
		//Log scaling requires the range to be > 0
		if (minVal < 0)
		{
			minVal = 0;
			//ToDo: Message to the UI!
		}
		if (scalingData->GetRangeMax() < 0)
		{
			maxVal = 0;
			if (maxVal < minVal)
			{
				maxVal = minVal;
			}
			//ToDo: Message to the UI!
		}

		lut->SetScaleToLog10();
	}
	else
	{
		throw std::invalid_argument("Not supported scaling function");
	}
	lut->SetTableRange(minVal, maxVal);
	lut->Build();
	mapper->SetLookupTable(lut);
}

void VtkDriverUnstructuredScalarSurface::setProperties(EntityVis2D3D* visEntity)
{
	DeletePropertyData();
	scalingData = new PropertyBundleDataHandleScaling(visEntity);
	visData = new PropertyBundleDataHandleVisUnstructuredScalarSurface(visEntity);
}
