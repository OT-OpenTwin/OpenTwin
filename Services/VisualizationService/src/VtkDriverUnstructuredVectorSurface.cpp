#include "stdafx.h"

#include "VtkDriverUnstructuredVectorSurface.h"
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

VtkDriverUnstructuredVectorSurface::VtkDriverUnstructuredVectorSurface() {}

VtkDriverUnstructuredVectorSurface::~VtkDriverUnstructuredVectorSurface() 
{
	DeletePropertyData();
}

void VtkDriverUnstructuredVectorSurface::CheckForModelUpdates()
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

void VtkDriverUnstructuredVectorSurface::DeletePropertyData(void)
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

std::string VtkDriverUnstructuredVectorSurface::buildSceneNode(DataSourceManagerItem *dataItem) 
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

vtkAlgorithmOutput* VtkDriverUnstructuredVectorSurface::GetArrowSource(void)
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

void VtkDriverUnstructuredVectorSurface::AssembleNode(osg::Node* parent)
{
	vtkAlgorithmOutput* data = SetScalarValues();

	AddNodeVectors(data, parent);
}

vtkAlgorithmOutput * VtkDriverUnstructuredVectorSurface::SetScalarValues()
{
	vtkVectorNorm* vectorNorm = vtkVectorNorm::New();
	objectsToDelete.push_back(vectorNorm);

	if (dataConnection != nullptr) vectorNorm->SetInputConnection(dataConnection);
	else vectorNorm->SetInputData(dataSource->GetVtkGrid());

	vectorNorm->SetNormalize(false);
	vectorNorm->Update();
	scalarRange = vectorNorm->GetOutput()->GetScalarRange();
	return vectorNorm->GetOutputPort();
}

void VtkDriverUnstructuredVectorSurface::AddNodeVectors(vtkAlgorithmOutput *input, osg::Node* parent)
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

void VtkDriverUnstructuredVectorSurface::SetColouring(vtkPolyDataMapper * mapper)
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

void VtkDriverUnstructuredVectorSurface::setProperties(EntityVis2D3D *visEntity) 
{
	DeletePropertyData();
	scalingData = new PropertyBundleDataHandleScaling(visEntity);
	visData = new PropertyBundleDataHandleVisUnstructuredVectorSurface(visEntity);
}
