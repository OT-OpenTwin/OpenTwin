#include "ClassFactory.h"

#include "EntityAnnotation.h"
#include "EntityAnnotationData.h"
#include "EntityContainer.h"
#include "EntityFacetData.h"
#include "EntityMeshTetFace.h"
#include "EntityMeshTetData.h"
#include "EntityMeshTetFaceData.h"
#include "EntityMeshTetNodes.h"
#include "EntityMeshTetItemDataFaces.h"
#include "EntityMeshTetItemDataTets.h"
#include "EntitySolver.h"
#include "EntitySolverKriging.h"
#include "EntitySolverPHREEC.h"
#include "EntitySolverFITTD.h"
#include "EntityMaterial.h"
#include "EntityMesh.h"
#include "EntityMeshTet.h"
#include "EntityMeshCartesian.h"
#include "EntityMeshCartesianData.h"
#include "EntityMeshCartesianFaceList.h"
#include "EntityMeshCartesianFace.h"
#include "EntityMeshCartesianItem.h"
#include "EntityMeshCartesianNodes.h"
#include "EntityMeshTetItem.h"
#include "EntityResult1D.h"
#include "EntityResult1DData.h"
#include "EntityPlot1D.h"
#include "EntityResultText.h"
#include "EntityResultTextData.h"
#include "EntityResultTable.h"
#include "EntityResultTableData.h"
#include "EntityVis2D3D.h"
#include "EntityBinaryData.h"
#include "EntityMeshTetItemDataTetedges.h"
#include "EntityCompressedVector.h"
#include "EntityCartesianVector.h"
#include "EntityVisCartesianFaceScalar.h"
#include "EntityResult3D.h"
#include "EntityResult3DData.h"
#include "EntitySolverMonitor.h"
#include "EntitySolverPort.h"
#include "EntitySignalType.h"
#include "EntityParameter.h"
#include "EntityUnits.h"
#include "EntityFile.h"
#include "EntityFileCSV.h"
#include "EntityParameterizedDataTable.h"
#include "EntityParameterizedDataCategorization.h"
#include "EntityTableSelectedRanges.h"
#include "EntityParameterizedDataPreviewTable.h"
#include "OpenTwinCore/TypeNames.h"
#include "EntityResearchMetadata.h"
#include "EntitySolverGetDP.h"
#include "EntityMeasurementMetadata.h"
#include "EntityBlock.h"
#include "EntityBlockDatabaseAccess.h"


EntityBase *ClassFactory::CreateEntity(const std::string &entityType)
{
	if (entityType == "EntityAnnotation")
	{
		return new EntityAnnotation(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityAnnotationData")
	{
		return new EntityAnnotationData(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityContainer")
	{
		return new EntityContainer(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityFacetData")
	{
		return new EntityFacetData(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeshTetFace")
	{
		return new EntityMeshTetFace(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeshTetData")
	{
		return new EntityMeshTetData(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeshTetFaceData")
	{
		return new EntityMeshTetFaceData(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeshTetNodes")
	{
		return new EntityMeshTetNodes(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeshTetItemDataFaces")
	{
		return new EntityMeshTetItemDataFaces(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeshTetItemDataTets")
	{
		return new EntityMeshTetItemDataTets(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntitySolver")
	{
		return new EntitySolver(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntitySolverKriging")
	{
		return new EntitySolverKriging(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMesh")
	{
		return new EntityMesh(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeshTet")
	{
		return new EntityMeshTet(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeshTetItem")
	{
		return new EntityMeshTetItem(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMaterial")
	{
		return new EntityMaterial(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityResult1D")
	{
		return new EntityResult1D(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityResult1DData")
	{
		return new EntityResult1DData(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityPlot1D")
	{
		return new EntityPlot1D(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeshCartesian")
	{
		return new EntityMeshCartesian(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeshCartesianData")
	{
		return new EntityMeshCartesianData(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeshCartesianFaceList")
	{
		return new EntityMeshCartesianFaceList(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeshCartesianFace")
	{
		return new EntityMeshCartesianFace(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeshCartesianItem")
	{
		return new EntityMeshCartesianItem(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeshCartesianNodes")
	{
		return new EntityMeshCartesianNodes(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityResultText")
	{
		return new EntityResultText(0, nullptr, nullptr, nullptr, this, "");
	}	
	else if (entityType == "EntityResultTextData")
	{
		return new EntityResultTextData(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityResultTable_" + ot::TypeNames::getInt32TypeName())
	{
		return new EntityResultTable<int32_t>(0, nullptr, nullptr, nullptr, this, "");
	}	
	else if (entityType == "EntityResultTable_" + ot::TypeNames::getInt64TypeName())
	{
		return new EntityResultTable<int64_t>(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityResultTable_" + ot::TypeNames::getStringTypeName())
	{
		return new EntityResultTable<std::string>(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityResultTable_" + ot::TypeNames::getDoubleTypeName())
	{
		return new EntityResultTable<double>(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityResultTableData_" + ot::TypeNames::getInt32TypeName())
	{
		return new EntityResultTableData<int32_t>(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityResultTableData_" + ot::TypeNames::getInt64TypeName())
	{
		return new EntityResultTableData<int64_t>(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityResultTableData_" + ot::TypeNames::getDoubleTypeName())
	{
		return new EntityResultTableData<double>(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityResultTableData_" + ot::TypeNames::getStringTypeName())
	{
		return new EntityResultTableData<std::string>(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityVis2D3D")
	{
		return new EntityVis2D3D(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBinaryData")
	{
		return new EntityBinaryData(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeshTetItemDataTetedges")
	{
	return new EntityMeshTetItemDataTetedges(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntitySolverPHREEC")
	{
		return new EntitySolverPHREEC(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntitySolverFITTD")
	{
		return new EntitySolverFITTD(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityCompressedVector")
	{
		return new EntityCompressedVector(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityCartesianVector")
	{
		return new EntityCartesianVector(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityVisCartesianFaceScalar")
	{
		return new EntityVisCartesianFaceScalar(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityResult3D")
	{
		return new EntityResult3D (0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityResult3DData")
	{
		return new EntityResult3DData(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntitySolverMonitor")
	{
		return new EntitySolverMonitor(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntitySolverPort")
	{
		return new EntitySolverPort(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntitySignalType")
	{
	return new EntitySignalType(0, nullptr, nullptr, nullptr, this, "");
	}	
	else if (entityType == "EntityParameter")
	{
		return new EntityParameter(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityUnits")
	{
		return new EntityUnits(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityFile")
	{
		return new EntityFile(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityFileCSV")
	{
		return new EntityFileCSV(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityParameterizedDataTable")
	{
		return new EntityParameterizedDataTable(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityParameterizedDataCategorization")
	{
		return new EntityParameterizedDataCategorization(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityTableSelectedRanges")
	{
		return new EntityTableSelectedRanges(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityParameterizedDataPreviewTable")
	{
		return new EntityParameterizedDataPreviewTable(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityResearchMetadata")
	{
		return new EntityResearchMetadata(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityMeasurementMetadata")
	{
		return new EntityMeasurementMetadata(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntitySolverGetDP")
	{
		return new EntitySolverGetDP(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlock")
	{
		return new EntityBlock(0, nullptr, nullptr, nullptr, this, "");
	}
	else if (entityType == "EntityBlockDatabaseAccess")
	{
		return new EntityBlockDatabaseAccess(0, nullptr, nullptr, nullptr, this, "");
	}
	else
	{
		return nullptr;
	}
}
