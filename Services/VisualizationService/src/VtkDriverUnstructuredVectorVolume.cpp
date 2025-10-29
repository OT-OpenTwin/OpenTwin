// @otlicense

#include "stdafx.h"

#include "VtkDriverUnstructuredVectorVolume.h"
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
#include <vtkLookupTable.h>
#include <vtkMaskPoints.h>
#include <vtkBandedPolyDataContourFilter.h>
#include <vtkGeometryFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkPlaneCutter.h>
#include <vtkHedgeHog.h>

VtkDriverUnstructuredVectorVolume::VtkDriverUnstructuredVectorVolume() {}

VtkDriverUnstructuredVectorVolume::~VtkDriverUnstructuredVectorVolume() 
{
	DeletePropertyData();
}

void VtkDriverUnstructuredVectorVolume::CheckForModelUpdates()
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

void VtkDriverUnstructuredVectorVolume::DeletePropertyData(void)
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

	if (visData != nullptr)
	{
		delete visData;
		visData = nullptr;
	}
}

std::string VtkDriverUnstructuredVectorVolume::buildSceneNode(DataSourceManagerItem *dataItem) 
{
	objectsToDelete.clear();

	updateTopoEntityID.clear();
	updateTopoEntityVersion.clear();

	std::time_t timer = time(nullptr);
	reportTime("VTK image creation started", timer);

	osg::Node *node = new osg::Switch;
	
	dataSource = dynamic_cast<DataSourceUnstructuredMesh*>(dataItem);

	if (dataSource != nullptr)
	{
		vtkNew<vtkCellDataToPointData> cellToPoint;

		dataConnection = nullptr;

		if (dataSource->GetHasCellScalar() || dataSource->GetHasCellVector())
		{
			cellToPoint->SetInputData(dataSource->GetVtkGrid());
			cellToPoint->ProcessAllArraysOn();
			cellToPoint->Update();

			dataConnection = cellToPoint->GetOutputPort();
		}

		if (visData->GetSelectedVisType() == PropertiesVisUnstructuredVector::VisualizationType::Arrows3D)
		{
			Assemble3DNode(node);
		}
		else
		{
			Assemble2DNode(node);
		}

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

vtkAlgorithmOutput * VtkDriverUnstructuredVectorVolume::ApplyCutplane(osg::Node * parent)
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
	else planeCut->SetInputData(dataSource->GetVtkGrid());

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

vtkAlgorithmOutput* VtkDriverUnstructuredVectorVolume::GetArrowSource(void)
{
	if (visData->GetSelectedArrowType() == PropertiesVisUnstructuredVector::VisualizationArrowType::ARROW_FLAT)
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
	else if (visData->GetSelectedArrowType() == PropertiesVisUnstructuredVector::VisualizationArrowType::ARROW_SHADED)
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

void VtkDriverUnstructuredVectorVolume::Assemble3DNode(osg::Node* parent)
{
	vtkAlgorithmOutput* data = SetScalarValues();

	AddNodeVectors(data, parent);
}

void VtkDriverUnstructuredVectorVolume::Assemble2DNode(osg::Node *parent)
{
	dataConnection = ApplyCutplane(parent);

	if (visData->GetSelectedVisType() == PropertiesVisUnstructuredVector::VisualizationType::Arrows2D)
	{
		vtkAlgorithmOutput* data = SetScalarValues();

		AddNodeVectors(data, parent);
	}
	else if (visData->GetSelectedVisType() == PropertiesVisUnstructuredVector::VisualizationType::Contour2D)
	{
		vtkAlgorithmOutput* scalar = SetScalarValues();

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
	else if (visData->GetSelectedVisType() == PropertiesVisUnstructuredVector::VisualizationType::UNKNOWN)
	{
		throw std::invalid_argument("Unsuported vector visualization type");
	}
}

vtkAlgorithmOutput * VtkDriverUnstructuredVectorVolume::SetScalarValues()
{
	if (   visData->GetSelectedVisType() != PropertiesVisUnstructuredVector::VisualizationType::Contour2D
		|| visData->GetSelectedVisComp() == PropertiesVisUnstructuredVector::VisualizationComponent::Abs)
	{
		vtkVectorNorm * vectorNorm = vtkVectorNorm::New();
		objectsToDelete.push_back(vectorNorm);
		
		if (dataConnection != nullptr) vectorNorm->SetInputConnection(dataConnection);
		else vectorNorm->SetInputData(dataSource->GetVtkGrid());

		vectorNorm->SetNormalize(false);
		vectorNorm->Update();
		scalarRange = vectorNorm->GetOutput()->GetScalarRange();
		return vectorNorm->GetOutputPort();
	}
	else if (visData->GetSelectedVisComp() == PropertiesVisUnstructuredVector::VisualizationComponent::UNKNOWN)
	{
		throw std::invalid_argument("Unsupported visualization component");
	}
	else
	{
		vtkExtractVectorComponents* vectorComponent = vtkExtractVectorComponents::New();
		objectsToDelete.push_back(vectorComponent);

		if (dataConnection != nullptr) vectorComponent->SetInputConnection(dataConnection);
		else vectorComponent->SetInputData(dataSource->GetVtkGrid());

		vectorComponent->Update();
		vectorComponent->SetExtractToFieldData(false);
		if (visData->GetSelectedVisComp() == PropertiesVisUnstructuredVector::VisualizationComponent::X)
		{
			scalarRange = vectorComponent->GetOutput(0)->GetScalarRange();
			return vectorComponent->GetOutputPort(0);
		}
		else if (visData->GetSelectedVisComp() == PropertiesVisUnstructuredVector::VisualizationComponent::Y)
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

void VtkDriverUnstructuredVectorVolume::AddNodeVectors(vtkAlgorithmOutput *input, osg::Node* parent)
{
	vtkNew<vtkMaskPoints> downSampling;
	downSampling->SetInputConnection(input);
	downSampling->SetMaximumNumberOfPoints(std::max(1, visData->getMaxArrows()));
	downSampling->SetRandomModeType(5);
	downSampling->Update();

	vtkNew<vtkPolyDataMapper> vectorFieldMapper;
	vtkNew<vtkGlyph3D> glyph;
	vtkNew<vtkHedgeHog> hedgehog;

	if (   visData->GetSelectedArrowType() == PropertiesVisUnstructuredVector::VisualizationArrowType::ARROW_FLAT
		|| visData->GetSelectedArrowType() == PropertiesVisUnstructuredVector::VisualizationArrowType::ARROW_SHADED)
	{
		glyph->SetSourceConnection(GetArrowSource());
		glyph->SetInputConnection(downSampling->GetOutputPort());
		glyph->ScalingOn();
		glyph->SetColorModeToColorByScalar();
		glyph->SetScaleModeToScaleByScalar();
		glyph->SetVectorModeToUseVector();
		double normalization = std::abs(scalarRange[1]);

		double dx = dataSource->GetVtkGrid()->GetBounds()[1] - dataSource->GetVtkGrid()->GetBounds()[0];
		double dy = dataSource->GetVtkGrid()->GetBounds()[3] - dataSource->GetVtkGrid()->GetBounds()[2];
		double dz = dataSource->GetVtkGrid()->GetBounds()[5] - dataSource->GetVtkGrid()->GetBounds()[5];

		double pointRadius = 0.1 * sqrt(dx * dx + dy * dy + dz * dz);

		if (normalization != 0)
		{
			glyph->SetScaleFactor(pointRadius * visData->GetArrowScale() / normalization);
		}
		else
		{
			glyph->SetScaleFactor(pointRadius * visData->GetArrowScale());
		}
		glyph->OrientOn();
		glyph->Update();

		vectorFieldMapper->SetInputConnection(glyph->GetOutputPort());
	}
	else if (visData->GetSelectedArrowType() == PropertiesVisUnstructuredVector::VisualizationArrowType::HEDGHEHOG)
	{
		hedgehog->SetInputConnection(downSampling->GetOutputPort());
		hedgehog->SetVectorModeToUseVector();
		double normalization = std::abs(scalarRange[1]);

		double dx = dataSource->GetVtkGrid()->GetBounds()[1] - dataSource->GetVtkGrid()->GetBounds()[0];
		double dy = dataSource->GetVtkGrid()->GetBounds()[3] - dataSource->GetVtkGrid()->GetBounds()[2];
		double dz = dataSource->GetVtkGrid()->GetBounds()[5] - dataSource->GetVtkGrid()->GetBounds()[5];

		double pointRadius = 0.1 * sqrt(dx * dx + dy * dy + dz * dz);

		if (normalization != 0)
		{
			hedgehog->SetScaleFactor(pointRadius * visData->GetArrowScale() / normalization);
		}
		else
		{
			hedgehog->SetScaleFactor(pointRadius * visData->GetArrowScale());
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

void VtkDriverUnstructuredVectorVolume::SetColouring(vtkPolyDataMapper * mapper)
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

void VtkDriverUnstructuredVectorVolume::setProperties(EntityVis2D3D *visEntity) 
{
	DeletePropertyData();
	planeData = new PropertyBundleDataHandlePlane(visEntity);
	scalingData = new PropertyBundleDataHandleScaling(visEntity);
	visData = new PropertyBundleDataHandleVisUnstructuredVector(visEntity);
}
