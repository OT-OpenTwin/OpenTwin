// @otlicense
// File: VtkDriverGeneric.cpp
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

#include "VtkDriverGeneric.h"
#include "DataSourceManager.h"
#include "DataSourceManagerItem.h"
#include "EntityVis2D3D.h"

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
#include <vtkStructuredGrid.h>
#include <vtkCutter.h>
#include <vtkArrowSource.h>
#include <vtkGlyph3D.h>
#include <vtkLookupTable.h>
#include <vtkExtractRectilinearGrid.h>

VtkDriverGeneric::VtkDriverGeneric() :
	visualizationType(ARROWS),
	normal{0.0, 0.0, 1.0}
{

}

VtkDriverGeneric::~VtkDriverGeneric()
{

}

void VtkDriverGeneric::setProperties(EntityVis2D3D *visEntity)
{
	// General
	EntityPropertiesEntityList *resultProperty = dynamic_cast<EntityPropertiesEntityList*>(visEntity->getProperties().getProperty("Result"));
	EntityPropertiesSelection *typeProperty = dynamic_cast<EntityPropertiesSelection*>(visEntity->getProperties().getProperty("Type"));
	EntityPropertiesSelection *componentProperty = dynamic_cast<EntityPropertiesSelection*>(visEntity->getProperties().getProperty("Component"));
	EntityPropertiesDouble *phaseProperty = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Phase"));
	EntityPropertiesDouble *timeProperty = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Time"));

	// Plane

	EntityPropertiesSelection *normalProperty = dynamic_cast<EntityPropertiesSelection*>(visEntity->getProperties().getProperty("Normal"));
	EntityPropertiesDouble *normalXProperty = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Normal X"));
	EntityPropertiesDouble *normalYProperty = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Normal Y"));
	EntityPropertiesDouble *normalZProperty = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Normal Z"));
	EntityPropertiesDouble *centerXProperty = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Center X"));
	EntityPropertiesDouble *centerYProperty = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Center Y"));
	EntityPropertiesDouble *centerZProperty = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Center Z"));

	// Scaling
	EntityPropertiesBoolean *autoscaleProperty = dynamic_cast<EntityPropertiesBoolean*>(visEntity->getProperties().getProperty("Autoscale"));
	EntityPropertiesDouble *minProperty = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Min"));
	EntityPropertiesDouble *maxProperty = dynamic_cast<EntityPropertiesDouble*>(visEntity->getProperties().getProperty("Max"));
	EntityPropertiesBoolean *logscaleProperty = dynamic_cast<EntityPropertiesBoolean*>(visEntity->getProperties().getProperty("Logscale"));

	// Update the visualization -> Create a new visualization data object
	if (typeProperty != nullptr)
	{
		if (typeProperty->getValue() == "Arrows")
		{
			setVisualizationType(VtkDriverGeneric::ARROWS);
		}
		else if (typeProperty->getValue() == "Contour")
		{
			setVisualizationType(VtkDriverGeneric::CONTOUR);
		}
		else if (typeProperty->getValue() == "Carpet")
		{
			setVisualizationType(VtkDriverGeneric::CARPET);
		}
		else if (typeProperty->getValue() == "Isolines")
		{
			setVisualizationType(VtkDriverGeneric::ISOLINES);
		}
		else
		{
			assert(0); // Unknown visualization type
		}
	}

	if (normalProperty != nullptr)
	{
		if (normalProperty->getValue() == "X")
		{
			setNormal(1.0, 0.0, 0.0);
		}
		else if (normalProperty->getValue() == "Y")
		{
			setNormal(0.0, 1.0, 0.0);
		}
		else if (normalProperty->getValue() == "Z")
		{
			setNormal(0.0, 0.0, 1.0);
		}
		else if (normalProperty->getValue() == "Free")
		{
			double nx = normalXProperty->getValue();
			double ny = normalYProperty->getValue();
			double nz = normalZProperty->getValue();

			setNormal(nx, ny, nz);
		}
		else
		{
			assert(0); // Unknown normal type
		}
	}

	if (centerXProperty != nullptr && centerYProperty != nullptr && centerZProperty != nullptr)
	{
		setCenter(centerXProperty->getValue(), centerYProperty->getValue(), centerZProperty->getValue());
	}
}

std::string VtkDriverGeneric::buildSceneNode(DataSourceManagerItem *dataItem)
{
	std::time_t timer = time(nullptr);
	reportTime("VTK image creation started", timer);

	/*
	vtkNew<vtkLookupTable> lut;

	lut->SetNumberOfColors(256);
	lut->SetHueRange(0.0, 0.667);
	lut->Build();

	set red {1.0 0.0 0.0}
	set green {0.0 1.0 0.0}
	set blue {0.0 0.0 1.0}
	set alpha 1

	for {set i 0} {$i<63} {incr i 1} {
	eval lut SetTableValue $i $red 1
	}
	for {set i 63} {$i<128} {incr i 1} {
	eval lut SetTableValue $i $green 1
	}
	for {set i 128} {$i<256} {incr i 1} {
	eval lut SetTableValue $i $blue 1
	}
	*/

	osg::Node *node = new osg::Switch;

	bool arrows = (visualizationType == ARROWS);
	bool contour = (visualizationType == CONTOUR);

	vtkNew<vtkPlane> plane;
	vtkNew<vtkArrowSource> arrow;
	vtkNew<vtkGlyph3D> glyph;
	vtkNew<vtkPolyDataMapper> mapper;
	vtkNew<vtkExtractRectilinearGrid> downsample;

	if (arrows)
	{
		/*
		downsample->SetInputData(dataItem->getDataSource());
		downsample->SetSampleRate(5, 5, 5);

		plane->SetOrigin(center[0], center[1], center[2]);

		double nabs = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
		plane->SetNormal(normal[0] / nabs, normal[1] / nabs, normal[2] / nabs);

		vtkNew<vtkCutter> planeCut;
		//planeCut->SetInputData(dataItem->getDataSource());
		planeCut->SetInputConnection(downsample->GetOutputPort());
		planeCut->SetCutFunction(plane);

		arrow->SetTipResolution(6);
		arrow->SetTipRadius(0.1);
		arrow->SetTipLength(0.35);
		arrow->SetShaftResolution(6);
		arrow->SetShaftRadius(0.03);

		//glyph->SetInputConnection(reader->GetOutputPort());
		glyph->SetInputConnection(planeCut->GetOutputPort());
		glyph->SetSourceConnection(arrow->GetOutputPort());
		glyph->SetVectorModeToUseVector();
		glyph->SetColorModeToColorByScalar();
		glyph->SetScaleModeToDataScalingOff();
		glyph->OrientOn();
		glyph->SetScaleFactor(0.2);

		mapper->SetInputConnection(glyph->GetOutputPort());
		mapper->CreateDefaultLookupTable();
		//glyphMapper->SetLookupTable(lut);
		mapper->ScalarVisibilityOn();
		mapper->SetScalarRange(dataItem->GetScalarRange());

		vtkNew<vtkActor> actor;
		actor->SetMapper(mapper);

		osg::Node *arrowNode = VTKActorToOSG(actor);

		dynamic_cast<osg::Switch *>(node)->addChild(arrowNode);
		*/
	}

	if (contour)
	{
		/*
		plane->SetOrigin(center[0], center[1], center[2]);

		double nabs = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
		plane->SetNormal(normal[0] / nabs, normal[1] / nabs, normal[2] / nabs);

		vtkNew<vtkCutter> planeCut;
		planeCut->SetInputData(dataItem->getDataSource());
		//planeCut->SetInputConnection(dataSource-);// dataItem->getDataSource());
		planeCut->SetCutFunction(plane);

		vtkNew<vtkBandedPolyDataContourFilter> bf;
		bf->SetInputConnection(planeCut->GetOutputPort());
		bf->GenerateValues(30, dataItem->GetScalarRange());
		bf->GenerateContourEdgesOn();

		vtkNew<vtkLookupTable> lut;
		lut->SetNumberOfTableValues( 30 ); 
		lut->SetHueRange(0.0, 1.0);
		//lut->SetAlphaRange(0.5, 0.5);
		lut->SetAlphaRange(1.0, 1.0);
		lut->SetRange(0, 29);
		lut->Build();

		mapper->SetInputConnection(bf->GetOutputPort());
		//mapper->SetScalarModeToUsePointData();
		mapper->SetScalarRange(0, 29);
		mapper->SetScalarModeToUseCellData();
		mapper->SetLookupTable(lut);
		//mapper->UseLookupTableScalarRangeOn();

		vtkNew<vtkActor> actor;
		actor->SetMapper(mapper);

		osg::Node *contourNode = VTKActorToOSG(actor);

		dynamic_cast<osg::Switch *>(node)->addChild(contourNode);

		vtkNew<vtkPolyDataMapper> lineMapper;

		lineMapper->SetInputConnection(bf->GetOutputPort(1));
		lineMapper->SetScalarModeToUsePointData();
		lineMapper->SetScalarRange(0, 29);
		//lineMapper->SetScalarModeToUseCellData();
		lineMapper->SetLookupTable(lut);

		vtkNew<vtkActor> lineActor;
		lineActor->SetMapper(lineMapper);
		lineActor->GetProperty()->SetColor(0.0, 0.0, 0.0);

		osg::Node *lineNode = VTKActorToOSG(lineActor);

		dynamic_cast<osg::Switch *>(node)->addChild(lineNode);
		*/
	}

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
