// @otlicense
// File: VtkDriverCartesianVectorVolume.cpp
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

#include "VtkDriverCartesianVectorVolume.h"
#include "DataSourceManager.h"
#include "DataSourceManagerItem.h"
#include "DataSourceUnstructuredMesh.h"

#include "OTModelEntities/EntityVis2D3D.h"
#include "OTModelEntities/PlaneProperties.h"
#include "OTModelEntities/PropertiesVis2D3D.h"
#include "OTModelEntities/EntityMeshCartesianData.h"

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
#include <vtkRectilinearGrid.h>
#include <vtkCutter.h>
#include <vtkArrowSource.h>
#include <vtkGlyph3D.h>
#include <vtkInformation.h>
#include <vtkLookupTable.h>
#include <vtkExtractRectilinearGrid.h>
#include <vtkDoubleArray.h>
#include <vtkRectilinearGridGeometryFilter.h>
#include <vtkCellData.h>
#include <vtkFeatureEdges.h>
#include <vtkExtractVectorComponents.h>
#include <vtkVectorNorm.h>
#include <vtkMaskPoints.h>
#include <vtkBandedPolyDataContourFilter.h>
#include <vtkGeometryFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkPlaneCutter.h>
#include <vtkHedgeHog.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkMergeArrays.h>
#include <vtkArrayCalculator.h>

VtkDriverCartesianVectorVolume::VtkDriverCartesianVectorVolume() : dataSource(nullptr), dataConnection(nullptr) {}

VtkDriverCartesianVectorVolume::~VtkDriverCartesianVectorVolume() 
{
	DeletePropertyData();
}

void VtkDriverCartesianVectorVolume::CheckForModelUpdates()
{
	if (scalarRange == nullptr) return;

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

void VtkDriverCartesianVectorVolume::DeletePropertyData(void)
{
	VtkDriverWithScaling::DeletePropertyData();

	if (planeData != nullptr)
	{
		delete planeData;
		planeData = nullptr;
	}

	if (visData != nullptr)
	{
		delete visData;
		visData = nullptr;
	}
}

std::string VtkDriverCartesianVectorVolume::buildSceneNode(DataSourceManagerItem *dataItem, std::string& colorRampData)
{
	objectsToDelete.clear();

	updateTopoEntityID.clear();
	updateTopoEntityVersion.clear();

	std::time_t timer = time(nullptr);
	reportTime("VTK image creation started", timer);

	osg::Node *node = new osg::Switch;
	
	dataSource = dynamic_cast<DataSourceCartesianMesh*>(dataItem);

	if (dataSource != nullptr)
	{
		vtkNew<vtkCellDataToPointData> cellToPoint;
		dataConnection = nullptr;

		prepareComplexData();

		if (dataSource->GetHasCellScalar() || dataSource->GetHasCellVector())
		{
			cellToPoint->SetInputConnection(dataConnection);
			cellToPoint->ProcessAllArraysOn();
			cellToPoint->Update();

			dataConnection = cellToPoint->GetOutputPort();
		}

		if (visData->GetSelectedVisQuantity() == PropertiesVisCartesianVector::VisualizationQuantity::PHASE)
		{
			unit = "Degrees";  // Here we overwrite the unit from the data entity, since we have degrees here.
		}

		if (visData->GetSelectedVisType() == PropertiesVisCartesianVector::VisualizationType::Arrows3D)
		{
			Assemble3DNode(node);
		}
		else
		{
			Assemble2DNode(node);
		}

		getColorRampData(colorRampData);

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

void VtkDriverCartesianVectorVolume::prepareComplexData()
{
	// First, set correct names for the input data arrays (magnitude and phase)
	auto magGrid = dataSource->GetVtkGridAbs();
	auto phaseGrid = dataSource->GetVtkGridArg();

	vtkDataSetAttributes* magAttrs = dataSource->GetHasCellVector()
		? static_cast<vtkDataSetAttributes*>(magGrid->GetCellData())
		: static_cast<vtkDataSetAttributes*>(magGrid->GetPointData());

	vtkDataSetAttributes* phaseAttrs = dataSource->GetHasCellVector()
		? static_cast<vtkDataSetAttributes*>(phaseGrid->GetCellData())
		: static_cast<vtkDataSetAttributes*>(phaseGrid->GetPointData());

	auto magArr = magAttrs->GetVectors();
	auto phaArr = phaseAttrs->GetVectors();

	if (!magArr || !phaArr)
	{
		assert(0);
		return;
	}

	magArr->SetName("Magnitude");
	phaArr->SetName("Phase");

	// Merge the input data arrays into one
	auto mergeFilter = vtkMergeArrays::New();
	objectsToDelete.push_back(mergeFilter);
	mergeFilter->AddInputData(magGrid);
	mergeFilter->AddInputData(phaseGrid);

	// Scale the data according to the geometry scale. Attention: This will also scale the values, so we need to divide later
	auto transform = vtkTransform::New();
	objectsToDelete.push_back(transform);
	transform->Scale(dataSource->getScaleFactor(), dataSource->getScaleFactor(), dataSource->getScaleFactor());

	// TransformFilter
	auto transformFilter = vtkTransformFilter::New();
	objectsToDelete.push_back(transformFilter);
	transformFilter->TransformAllInputVectorsOn();
	transformFilter->SetTransform(transform);
	transformFilter->SetInputConnection(mergeFilter->GetOutputPort());

	transformFilter->Update();

	// And calculate the combination
	auto calcFilter = vtkArrayCalculator::New();
	objectsToDelete.push_back(calcFilter);
	calcFilter->SetInputConnection(transformFilter->GetOutputPort());

	if (dataSource->GetHasCellVector())
	{
		calcFilter->SetAttributeTypeToCellData();
	}
	else
	{
		calcFilter->SetAttributeTypeToPointData();
	}

	calcFilter->AddVectorVariable("M", "Magnitude");
	calcFilter->AddVectorVariable("P", "Phase");

	calcFilter->SetResultArrayName("DisplayField");

	calcFilter->AddScalarVariable("Mx", "Magnitude", 0);
	calcFilter->AddScalarVariable("My", "Magnitude", 1);
	calcFilter->AddScalarVariable("Mz", "Magnitude", 2);

	calcFilter->AddScalarVariable("Px", "Phase", 0);
	calcFilter->AddScalarVariable("Py", "Phase", 1);
	calcFilter->AddScalarVariable("Pz", "Phase", 2);

	calcFilter->SetResultArrayName("DisplayField");

	std::string sScale = std::to_string(1.0 / dataSource->getScaleFactor());

	switch (visData->GetSelectedVisQuantity())
	{
	case PropertiesVisCartesianVector::VisualizationQuantity::REAL:
		calcFilter->SetFunction((  "Mx * cos(Px * " + sScale + ") * iHat * " + sScale +
								 "+ My * cos(Py * " + sScale + ") * jHat * " + sScale +
								 "+ Mz * cos(Pz * " + sScale + ") * kHat * " + sScale).c_str());
		break;
	case PropertiesVisCartesianVector::VisualizationQuantity::IMAG:
		calcFilter->SetFunction((  "Mx * sin(Px * " + sScale + ") * iHat * " + sScale +
								 "+ My * sin(Py * " + sScale + ") * jHat * " + sScale +
								 "+ Mz * sin(Pz * " + sScale + ") * kHat * " + sScale).c_str());
		break;
	case PropertiesVisCartesianVector::VisualizationQuantity::MAG:
		calcFilter->SetFunction(("(Mx * iHat + My * jHat + Mz * kHat) * " + sScale).c_str());
		break;
	case PropertiesVisCartesianVector::VisualizationQuantity::PHASE:
		calcFilter->SetFunction(
			("(Px*" + sScale + "*180.0/3.141592653589793 - 360.0*floor((Px*" + sScale + "*180.0/3.141592653589793 + 180.0)/360.0))*iHat + "
			 "(Py*" + sScale + "*180.0/3.141592653589793 - 360.0*floor((Py*" + sScale + "*180.0/3.141592653589793 + 180.0)/360.0))*jHat + "
			 "(Pz*" + sScale + "*180.0/3.141592653589793 - 360.0*floor((Pz*" + sScale + "*180.0/3.141592653589793 + 180.0)/360.0))*kHat").c_str());
		break;
	case PropertiesVisCartesianVector::VisualizationQuantity::PHASE_PROJECTION:
		{
			// here we assume a positive phase definition exp(+i omega t)
			std::string sPhase = std::to_string(visData->GetPhase() * 3.14159265359 / 180.0);
			calcFilter->SetFunction((  "Mx*cos(Px*" + sScale + "+" + sPhase + ")*iHat * " + sScale +
									 "+ My*cos(Py*" + sScale + "+" + sPhase + ")*jHat * " + sScale +
									 "+ Mz*cos(Pz*" + sScale + "+" + sPhase + ")*kHat * " + sScale).c_str());
		}
		break;
	default:
		assert(0);
	}

	calcFilter->Update();

	dataConnection = calcFilter->GetOutputPort();
}

vtkAlgorithmOutput * VtkDriverCartesianVectorVolume::ApplyCutplane(osg::Node * parent)
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
	objectsToDelete.push_back(planeCut);

	if (dataConnection != nullptr) planeCut->SetInputConnection(dataConnection);
	else planeCut->SetInputData(dataSource->GetVtkGridAbs());

	planeCut->SetCutFunction(plane);
	planeCut->Update();

	// Create the mesh visualization
	if (visData->GetShow2dMesh())
	{
		vtkNew<vtkFeatureEdges> edges;
		edges->SetInputConnection(planeCut->GetOutputPort());
		edges->BoundaryEdgesOn();
		edges->ColoringOff();

		vtkNew<vtkPolyDataMapper> planeMapper;
		planeMapper->SetInputConnection(edges->GetOutputPort());
		planeMapper->SetScalarModeToUseCellData();

		vtkNew<vtkActor> planeActor;
		planeActor->SetMapper(planeMapper);
		planeActor->GetProperty()->SetColor(visData->GetColor2dMeshR(), visData->GetColor2dMeshG(), visData->GetColor2dMeshB());

		osg::Node* planeNode = VTKActorToOSG(planeActor);
		dynamic_cast<osg::Switch*>(parent)->addChild(planeNode);
	}

	return planeCut->GetOutputPort();
}

vtkAlgorithmOutput* VtkDriverCartesianVectorVolume::GetArrowSource(void)
{
	if (visData->GetSelectedArrowType() == PropertiesVisCartesianVector::VisualizationArrowType::ARROW_FLAT)
	{
		vtkArrowSource *arrow = vtkArrowSource::New();
		objectsToDelete.push_back(arrow);

		arrow->SetTipResolution(6);
		arrow->SetTipRadius(0.1);
		arrow->SetTipLength(0.35);
		arrow->SetShaftResolution(6);
		arrow->SetShaftRadius(0.03);

		return arrow->GetOutputPort();
	}
	else if (visData->GetSelectedArrowType() == PropertiesVisCartesianVector::VisualizationArrowType::ARROW_SHADED)
	{
		vtkNew<vtkArrowSource> arrow;

		arrow->SetTipResolution(6);
		arrow->SetTipRadius(0.1);
		arrow->SetTipLength(0.35);
		arrow->SetShaftResolution(6);
		arrow->SetShaftRadius(0.03);

		vtkPolyDataNormals *shadedArrow = vtkPolyDataNormals::New();
		objectsToDelete.push_back(shadedArrow);

		shadedArrow->SetInputConnection(arrow->GetOutputPort());
		shadedArrow->SetFeatureAngle(80.0);
		shadedArrow->FlipNormalsOff();
		shadedArrow->Update();

		return shadedArrow->GetOutputPort();
	}
	else
	{
		assert(0); // Unknown arrow type
	}

	return nullptr;
}

void VtkDriverCartesianVectorVolume::Assemble3DNode(osg::Node* parent)
{
	vtkAlgorithmOutput* data = SetScalarValues();

	if (scalarRange[0] > scalarRange[1]) return; // Empty data

	AddNodeVectors(data, parent);
}

void VtkDriverCartesianVectorVolume::Assemble2DNode(osg::Node *parent)
{
	dataConnection = ApplyCutplane(parent);

	// Check whether we have valid output from the cutplane
	vtkAlgorithm* producer = dataConnection->GetProducer();
	int outPort = dataConnection->GetIndex();

	producer->Update();

	vtkDataObject* obj = producer->GetOutputDataObject(outPort);
	vtkDataSet* ds = vtkDataSet::SafeDownCast(obj);

	if (ds == nullptr)
	{
		// We have no data set (maybe cutplane is outside valid region)
		return;
	}

	if (ds->GetNumberOfPoints() == 0 || ds->GetNumberOfCells() == 0)
	{
		// We have no data set (maybe cutplane is outside valid region)
		return;
	}


	// Now we are sure to have data, process it according to the visualization settings
	if (visData->GetSelectedVisType() == PropertiesVisCartesianVector::VisualizationType::Arrows2D)
	{
		vtkAlgorithmOutput* data = SetScalarValues();

		if (scalarRange[0] > scalarRange[1]) return; // Empty data

		AddNodeVectors(data, parent);
	}
	else if (visData->GetSelectedVisType() == PropertiesVisCartesianVector::VisualizationType::Contour2D)
	{
		vtkAlgorithmOutput* scalar = SetScalarValues();

		if (scalarRange[0] > scalarRange[1]) return; // Empty data

		vtkNew<vtkBandedPolyDataContourFilter> bf;
		bf->SetInputConnection(scalar);
		bf->SetGenerateContourEdges(true);
		bf->SetScalarModeToValue();
		bf->GenerateValues(scalingData->GetColourResolution(), scalarRange);
		bf->Update();

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

		vtkNew<vtkPolyDataMapper> vectorFieldMapper;
		vectorFieldMapper->SetInputConnection(bf->GetOutputPort());
		SetColouring(vectorFieldMapper);
		vectorFieldMapper->UseLookupTableScalarRangeOn();
		vectorFieldMapper->SetScalarModeToUseCellData();

		vtkNew<vtkActor> vectorFieldActor;
		vectorFieldActor->SetMapper(vectorFieldMapper);

		osg::Node* cutNode = VTKActorToOSG(vectorFieldActor);
		dynamic_cast<osg::Switch*>(parent)->addChild(cutNode);
	}
	else if (visData->GetSelectedVisType() == PropertiesVisCartesianVector::VisualizationType::UNKNOWN)
	{
		throw std::invalid_argument("Unsuported vector visualization type");
	}
}

vtkAlgorithmOutput * VtkDriverCartesianVectorVolume::SetScalarValues()
{
	if (   visData->GetSelectedVisType() != PropertiesVisCartesianVector::VisualizationType::Contour2D
		|| visData->GetSelectedVisComp() == PropertiesVisCartesianVector::VisualizationComponent::Abs)
	{
		vtkVectorNorm * vectorNorm = vtkVectorNorm::New();
		objectsToDelete.push_back(vectorNorm);
		
		if (dataConnection != nullptr) vectorNorm->SetInputConnection(dataConnection);
		else vectorNorm->SetInputData(dataSource->GetVtkGridAbs());

		vectorNorm->SetNormalize(false);
		vectorNorm->Update();
		scalarRange = vectorNorm->GetOutput()->GetScalarRange();
		return vectorNorm->GetOutputPort();
	}
	else if (visData->GetSelectedVisComp() == PropertiesVisCartesianVector::VisualizationComponent::UNKNOWN)
	{
		throw std::invalid_argument("Unsupported visualization component");
	}
	else
	{
		vtkExtractVectorComponents* vectorComponent = vtkExtractVectorComponents::New();
		objectsToDelete.push_back(vectorComponent);

		if (dataConnection != nullptr) vectorComponent->SetInputConnection(dataConnection);
		else vectorComponent->SetInputData(dataSource->GetVtkGridAbs());

		vectorComponent->SetExtractToFieldData(false);
		vectorComponent->Update();

		if (visData->GetSelectedVisComp() == PropertiesVisCartesianVector::VisualizationComponent::X)
		{
			scalarRange = vectorComponent->GetOutput(0)->GetScalarRange();
			return vectorComponent->GetOutputPort(0);
		}
		else if (visData->GetSelectedVisComp() == PropertiesVisCartesianVector::VisualizationComponent::Y)
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

void VtkDriverCartesianVectorVolume::AddNodeVectors(vtkAlgorithmOutput *input, osg::Node* parent)
{
	vtkNew<vtkMaskPoints> downSampling;
	downSampling->SetInputConnection(input);
	downSampling->SetMaximumNumberOfPoints(std::max(1, visData->getMaxArrows()));
	downSampling->RandomModeOn();
	downSampling->SetRandomModeType(5);
	downSampling->Update();

	vtkNew<vtkPolyDataMapper> vectorFieldMapper;
	vtkNew<vtkGlyph3D> glyph;
	vtkNew<vtkHedgeHog> hedgehog;

	if (   visData->GetSelectedArrowType() == PropertiesVisCartesianVector::VisualizationArrowType::ARROW_FLAT
		|| visData->GetSelectedArrowType() == PropertiesVisCartesianVector::VisualizationArrowType::ARROW_SHADED)
	{
		glyph->SetSourceConnection(GetArrowSource());
		glyph->SetInputConnection(downSampling->GetOutputPort());
		glyph->ScalingOn();
		glyph->SetColorModeToColorByScalar();
		glyph->SetScaleModeToScaleByScalar();
		glyph->SetVectorModeToUseVector();
		double normalization = std::abs(scalarRange[1]);

		double dx = dataSource->GetVtkGridAbs()->GetBounds()[1] - dataSource->GetVtkGridAbs()->GetBounds()[0];
		double dy = dataSource->GetVtkGridAbs()->GetBounds()[3] - dataSource->GetVtkGridAbs()->GetBounds()[2];
		double dz = dataSource->GetVtkGridAbs()->GetBounds()[5] - dataSource->GetVtkGridAbs()->GetBounds()[4];

		double pointRadius = 0.1 * sqrt(dx * dx + dy * dy + dz * dz);

		if (normalization != 0)
		{
			glyph->SetScaleFactor(pointRadius * visData->GetArrowScale() * dataSource->getScaleFactor() / normalization);
		}
		else
		{
			glyph->SetScaleFactor(pointRadius * visData->GetArrowScale() * dataSource->getScaleFactor());
		}
		glyph->OrientOn();
		glyph->Update();

		vectorFieldMapper->SetInputConnection(glyph->GetOutputPort());
	}
	else if (visData->GetSelectedArrowType() == PropertiesVisCartesianVector::VisualizationArrowType::HEDGHEHOG)
	{
		hedgehog->SetInputConnection(downSampling->GetOutputPort());
		hedgehog->SetVectorModeToUseVector();
		double normalization = std::abs(scalarRange[1]);

		double dx = dataSource->GetVtkGridAbs()->GetBounds()[1] - dataSource->GetVtkGridAbs()->GetBounds()[0];
		double dy = dataSource->GetVtkGridAbs()->GetBounds()[3] - dataSource->GetVtkGridAbs()->GetBounds()[2];
		double dz = dataSource->GetVtkGridAbs()->GetBounds()[5] - dataSource->GetVtkGridAbs()->GetBounds()[4];

		double pointRadius = 0.1 * sqrt(dx * dx + dy * dy + dz * dz);

		if (normalization != 0)
		{
			hedgehog->SetScaleFactor(pointRadius * visData->GetArrowScale() * dataSource->getScaleFactor() / normalization);
		}
		else
		{
			hedgehog->SetScaleFactor(pointRadius * visData->GetArrowScale() * dataSource->getScaleFactor());
		}
		hedgehog->Update();

		vectorFieldMapper->SetInputConnection(hedgehog->GetOutputPort());
	}
	else
	{
		assert(0); // Unknown arrow type
	}

	SetColouring(vectorFieldMapper);
	vectorFieldMapper->UseLookupTableScalarRangeOn();
	vectorFieldMapper->SetScalarModeToUsePointData();
	vectorFieldMapper->SetColorModeToMapScalars();

	vtkNew<vtkActor> vectorFieldActor;
	vectorFieldActor->SetMapper(vectorFieldMapper);

	osg::Node* cutNode = VTKActorToOSG(vectorFieldActor);
	dynamic_cast<osg::Switch*>(parent)->addChild(cutNode);
}

void VtkDriverCartesianVectorVolume::setProperties(EntityVis2D3D *visEntity) 
{
	DeletePropertyData();
	VtkDriverWithScaling::setProperties(visEntity);
	planeData = new PropertyBundleDataHandlePlane(visEntity);
	visData = new PropertyBundleDataHandleVisCartesianVector(visEntity);
}
