// @otlicense
// File: VtkDriverCartesianVectorfield.cpp
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

#include "VtkDriverCartesianVectorfield.h"
#include "DataSourceManager.h"
#include "DataSourceManagerItem.h"

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
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPlane.h>
#include <vtkRectilinearGrid.h>
#include <vtkCutter.h>
#include <vtkArrowSource.h>
#include <vtkGlyph3D.h>
#include <vtkLookupTable.h>
#include <vtkExtractRectilinearGrid.h>
#include <vtkDoubleArray.h>
#include <vtkRectilinearGridGeometryFilter.h>
#include <vtkCellData.h>
#include <vtkFeatureEdges.h>
#include <vtkExtractVectorComponents.h>
#include <vtkVectorNorm.h>
#include <vtkLookupTable.h>
#include <vtkMaskPoints.h>
#include <vtkBandedPolyDataContourFilter.h>

VtkDriverCartesianVectorfield::VtkDriverCartesianVectorfield() {}

VtkDriverCartesianVectorfield::~VtkDriverCartesianVectorfield() 
{
	DeletePropertyData();
}

void VtkDriverCartesianVectorfield::CheckForModelUpdates()
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

void VtkDriverCartesianVectorfield::DeletePropertyData(void)
{
	if (planeData != nullptr)
	{
		delete planeData;
		planeData = nullptr;
	}
	if (scalingData != nullptr)
	{
		delete scalingData;
		scalingData = nullptr;
	}
}
#include <vtkPlaneCutter.h>


std::string VtkDriverCartesianVectorfield::buildSceneNode(DataSourceManagerItem *dataItem) {
	updateTopoEntityID.clear();
	updateTopoEntityVersion.clear();
	std::time_t timer = time(nullptr);
	reportTime("VTK image creation started", timer);

	osg::Node *node = new osg::Switch;
	
	auto * dataSource = dynamic_cast<DataSourceResult3D*>(dataItem);

	if (dataSource != nullptr)
	{
		auto output = ApplyCutplane(dataSource, node);

		AssembleNode(output, node);

		//CheckForModelUpdates();
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

//#include <vtkStructuredDataPlaneCutter.h> with version 9.2
vtkAlgorithmOutput * VtkDriverCartesianVectorfield::ApplyCutplane(DataSourceResult3D *dataSource, osg::Node * parent)
{	
	assert(planeData != nullptr);
	assert(planeData->GetNormalDescription() != PlaneProperties::UNKNOWN);
	
	double normalX(0.), normalY(0.), normalZ(0.);

	if (planeData->GetNormalDescription() == PlaneProperties::X)
	{
		normalX = 1;
		normalY = 0;
		normalZ = 0;
	}
	else if(planeData->GetNormalDescription() == PlaneProperties::Y)
	{
		normalX = 0;
		normalY = 1;
		normalZ = 0;
	}
	else if (planeData->GetNormalDescription() == PlaneProperties::Z)
	{
		normalX = 0;
		normalY = 0;
		normalZ = 1;
	}
	else
	{
		normalX = planeData->GetNormalValueX();
		normalY = planeData->GetNormalValueY();
		normalZ = planeData->GetNormalValueZ();

		if (normalX == 0 && normalY == 0 && normalZ == 0)
		{
			normalX = 1;
			//ToDo: Message on UI
		}
	}


	vtkNew<vtkPlane> plane;
	plane->SetNormal(normalX, normalY,normalZ);
	double x(0), y(0), z(0);


	planeData->GetCenterValueX() < dataSource->GetXMinCoordinate() ? x = dataSource->GetXMinCoordinate() :
		planeData->GetCenterValueX() > dataSource->GetXMaxCoordinate() ? x = dataSource->GetXMaxCoordinate() :
		x = planeData->GetCenterValueX();
	planeData->GetCenterValueY() < dataSource->GetYMinCoordinate() ? y = dataSource->GetYMinCoordinate() :
		planeData->GetCenterValueY() > dataSource->GetYMaxCoordinate() ? y = dataSource->GetYMaxCoordinate() :
		y = planeData->GetCenterValueY();
	planeData->GetCenterValueZ() < dataSource->GetZMinCoordinate() ? z = dataSource->GetZMinCoordinate() :
		planeData->GetCenterValueZ() > dataSource->GetZMaxCoordinate() ? z = dataSource->GetZMaxCoordinate() :
		z = planeData->GetCenterValueZ();

	plane->SetOrigin(x,y,z);

	vtkCutter* planeCut = vtkCutter::New();
	planeCut->SetInputData(dataSource->GetVtkGrid());
	planeCut->SetCutFunction(plane);
	planeCut->Update();

	auto temp = planeCut->GetOutput()->GetPointData()->GetVectors();

	vtkVectorNorm * vectorNorm = vtkVectorNorm::New();
	vectorNorm->SetInputConnection(planeCut->GetOutputPort());
	vectorNorm->SetNormalize(false);
	vectorNorm->Update();
	scalarRange = vectorNorm->GetOutput()->GetScalarRange();


	vtkNew<vtkFeatureEdges> edges;
	edges->SetInputConnection(planeCut->GetOutputPort());
	edges->BoundaryEdgesOn();
	edges->SetColoring(false);

	vtkNew<vtkPolyDataMapper> planeMapper;
	planeMapper->SetInputConnection(edges->GetOutputPort());

	vtkNew<vtkActor> planeActor;
	planeActor->SetMapper(planeMapper);
	planeActor->GetProperty()->SetColor(1,1,1);

	osg::Node *planeNode = VTKActorToOSG(planeActor);
	dynamic_cast<osg::Switch *>(parent)->addChild(planeNode);

	return planeCut->GetOutputPort();
}

void VtkDriverCartesianVectorfield::AssembleNode(vtkAlgorithmOutput * input, osg::Node *parent)
{
	   	 
	vtkAlgorithmOutput * visualization = SetScalarValues(input);

	if (vis2D3DData->GetSelectedVisType() == PropertiesVis2D3D::VisualizationType::Arrows)
	{
		visualization = AddNodeVectors(visualization);
	}
	else if (vis2D3DData->GetSelectedVisType() == PropertiesVis2D3D::VisualizationType::Contour)
	{

		auto bf = vtkBandedPolyDataContourFilter::New();
		bf->SetInputConnection(visualization);
		bf->SetGenerateContourEdges(true);
		bf->SetScalarModeToValue();
		bf->GenerateValues(scalingData->GetColourResolution(), scalarRange);
		bf->Update();

		//vtkNew<vtkLookupTable> lut;
		//lut->SetNumberOfTableValues(scalingData->GetColourResolution());
		//lut->SetTableRange(scalarRange);
		//lut->SetHueRange(.667, 0.0);
		//lut->SetAlphaRange(1., 1.);
		//lut->IndexedLookupOff();
		//lut->Build();

		vtkNew<vtkPolyDataMapper> edgeMapper;
		edgeMapper->SetInputData(bf->GetContourEdgesOutput());
		edgeMapper->SetResolveCoincidentTopologyToPolygonOffset();
		
		vtkNew<vtkActor> edgeActor;
		edgeActor->SetMapper(edgeMapper);
		edgeActor->GetProperty()->SetColor(0, 0, 0);
		osg::Node * edgeNode = VTKActorToOSG(edgeActor);
		dynamic_cast<osg::Switch *>(parent)->addChild(edgeNode);

		//vtkNew<vtkPolyDataMapper> testAusgabe;
		//testAusgabe->SetInputConnection(bf->GetOutputPort());
		//testAusgabe->SetLookupTable(lut);
		//testAusgabe->SetScalarRange(scalarRange);
		//testAusgabe->SetScalarModeToUsePointData();
		//testAusgabe->SetColorModeToMapScalars();
		
		//testAusgabe->Update();

		//vtkNew<vtkActor> testAktor;
		//testAktor->SetMapper(testAusgabe);

		//osg::Node *cutNode = VTKActorToOSG(testAktor);
		//dynamic_cast<osg::Switch *>(parent)->addChild(cutNode);
		//input->Delete();


		//visualization = bf->GetOutputPort();
	}
	else if (vis2D3DData->GetSelectedVisType() == PropertiesVis2D3D::VisualizationType::UNKNOWN)
	{
		throw std::invalid_argument("Unsuported 2D3D visualization type");
	}
	//Banded filter für Contour! 
	if (visualization == nullptr)
	{
		throw std::logic_error("Failed to create a visualization output.");
	}

	vtkNew<vtkPolyDataMapper> vectorFieldMapper;
	vectorFieldMapper->SetInputConnection(visualization);
	SetColouring(vectorFieldMapper);
	vectorFieldMapper->UseLookupTableScalarRangeOn();
	vectorFieldMapper->SetScalarModeToUsePointData();
	vectorFieldMapper->SetColorModeToMapScalars();
	//vectorFieldMapper->SetScalarVisibility(false);

	vtkNew<vtkActor> vectorFieldActor;
	vectorFieldActor->SetMapper(vectorFieldMapper);

	osg::Node *cutNode = VTKActorToOSG(vectorFieldActor);
	dynamic_cast<osg::Switch *>(parent)->addChild(cutNode);
	input->Delete();
	visualization->Delete();
}

vtkAlgorithmOutput * VtkDriverCartesianVectorfield::SetScalarValues(vtkAlgorithmOutput * input)
{
	if (vis2D3DData->GetSelectedVisComp() == PropertiesVis2D3D::VisualizationComponent::Abs)
	{
		vtkVectorNorm * vectorNorm = vtkVectorNorm::New();
		vectorNorm->SetInputConnection(input);
		vectorNorm->SetNormalize(false);
		vectorNorm->Update();
		scalarRange = vectorNorm->GetOutput()->GetScalarRange();
		return vectorNorm->GetOutputPort();
	}
	else if (vis2D3DData->GetSelectedVisComp() == PropertiesVis2D3D::VisualizationComponent::UNKNOWN)
	{
		throw std::invalid_argument("Unsupported visualization component");
	}
	else
	{
		vtkExtractVectorComponents* vectorComponent = vtkExtractVectorComponents::New();
		vectorComponent->SetInputConnection(input);
		vectorComponent->Update();
		vectorComponent->SetExtractToFieldData(false);
		if (vis2D3DData->GetSelectedVisComp() == PropertiesVis2D3D::VisualizationComponent::X)
		{
			scalarRange = vectorComponent->GetOutput(0)->GetScalarRange();
			return vectorComponent->GetOutputPort(0);
		}
		else if (vis2D3DData->GetSelectedVisComp() == PropertiesVis2D3D::VisualizationComponent::Y)
		{
			scalarRange = vectorComponent->GetOutput(1)->GetScalarRange();
			return vectorComponent->GetOutputPort(1);
		}
		else
		{
			scalarRange = vectorComponent->GetOutput(2)->GetScalarRange();
			return vectorComponent->GetOutputPort(2);
		}
	}
	return nullptr;
}

vtkAlgorithmOutput * VtkDriverCartesianVectorfield::AddNodeVectors(vtkAlgorithmOutput * input)
{
	vtkNew<vtkMaskPoints> downSampling;
	downSampling->SetInputConnection(input);
	downSampling->SetOnRatio(vis2D3DData->GetDownsamplingRate());
	downSampling->Update();

	// Add glyphs for vector visualization
	vtkNew<vtkArrowSource> arrow;
	arrow->SetTipResolution(6);
	arrow->SetTipRadius(0.1);
	arrow->SetTipLength(0.35);
	arrow->SetShaftResolution(6);
	arrow->SetShaftRadius(0.03);

	vtkGlyph3D* glyph = vtkGlyph3D::New();
	glyph->SetSourceConnection(arrow->GetOutputPort());
	glyph->SetInputConnection(downSampling->GetOutputPort());
	glyph->SetColorModeToColorByScalar();
	glyph->SetVectorModeToUseVector();
	glyph->ScalingOn();
	double normalization = std::abs(scalarRange[1]);
	if (normalization != 0)
	{
		glyph->SetScaleFactor(1 / normalization);
	}
	else
	{
		glyph->SetScaleFactor(1);
	}
	glyph->OrientOn();
	glyph->Update();
	return glyph->GetOutputPort();
}

void VtkDriverCartesianVectorfield::SetColouring(vtkPolyDataMapper * mapper)
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

void VtkDriverCartesianVectorfield::setProperties(EntityVis2D3D *visEntity) 
{
	DeletePropertyData();
	planeData = new PropertyBundleDataHandlePlane(visEntity);
	scalingData = new PropertyBundleDataHandleScaling(visEntity);
	vis2D3DData = new PropertyBundleDataHandleVis2D3D(visEntity);
}
