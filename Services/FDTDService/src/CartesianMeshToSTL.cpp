#include "CartesianMeshToSTL.h"

#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityAPI.h"

#include "DataBase.h"
#include "EntityMaterial.h"
#include "EntityMeshCartesianData.h"
#include "EntityMeshCartesianItem.h"
#include "EntityMeshCartesianFaceList.h"
#include "EntityMeshCartesianFace.h"

#include <fstream>
#include <map>
#include <set>

CartesianMeshToSTL::CartesianMeshToSTL(const std::string& meshName, const std::string &tmpFolderName)
{
	// Load the mesh data entity
	std::string meshDataName = meshName + "/Mesh";
	ot::EntityInformation meshDataInfo;
	ot::ModelServiceAPI::getEntityInformation(meshDataName, meshDataInfo);

	EntityMeshCartesianData* meshDataEntity = dynamic_cast<EntityMeshCartesianData*>(ot::EntityAPI::readEntityFromEntityIDandVersion(meshDataInfo.getEntityID(), meshDataInfo.getEntityVersion()));
	if (meshDataEntity == nullptr)
	{
		assert(0);
		return;
	}

	// Get a list of all mesh entities for the given mesh
	ot::UIDList meshEntities = ot::ModelServiceAPI::getIDsOfFolderItemsOfType(meshName + "/Mesh", "EntityMeshCartesianItem", true);

	// Now we need to get full information for these entities
	std::list<ot::EntityInformation> meshEntitiesInfo;
	ot::ModelServiceAPI::getEntityInformation(meshEntities, meshEntitiesInfo);

	// Load all faces from the mesh data entity
	EntityMeshCartesianFaceList* meshFacesList = meshDataEntity->getMeshFaces();

	// Prefetch all the mesh items
	DataBase::instance().prefetchDocumentsFromStorage(meshEntitiesInfo);

	// Iterate through all mesh entities and export their STL data
	size_t objectIndex = 0;
	std::list<std::string> objectMaterialNames;

	for (auto& meshEntity : meshEntitiesInfo)
	{
		EntityMeshCartesianItem* meshItem = dynamic_cast<EntityMeshCartesianItem*> (ot::EntityAPI::readEntityFromEntityIDandVersion(meshEntity.getEntityID(), meshEntity.getEntityVersion()));
		if (meshItem == nullptr)
		{
			assert(0);
			continue;
		}

		std::string objectName = "object_" + std::to_string(objectIndex);
		std::string fileName   = tmpFolderName + "\\" + objectName + ".stl";

		objectMaterialNames.push_back(meshItem->getMaterial());
		fileNamesOfObjects.push_back(fileName);

		std::ofstream objectFile(fileName);
		objectFile << "solid " << objectName << std::endl;

		// Iterate through the faces
		for (size_t faceIndex = 0; faceIndex < meshItem->getNumberFaces(); faceIndex++)
		{
			bool reverseFace = !meshItem->getFaceOrientation(faceIndex);
			int  faceId = abs(meshItem->getFaceId(faceIndex));

			EntityMeshCartesianFace* meshFace = meshFacesList->getFace(faceId);
			if (meshFace == nullptr)
			{
				assert(0);
				continue;
			}

			if (meshFace->getType() != EntityMeshCartesianFace::INDEX_BASED)
			{
				assert(0);
				continue;
			}

			// Now we iterate through all directions and create the triangles for the quads belonging to this face
			processCellFaces(objectFile, meshDataEntity, meshFace, reverseFace);
		}

		objectFile << "endsolid " << objectName << std::endl;
		objectIndex++;
	}

	// As a last step, we load all relevant materials and assign them to the materialsOfObjects vector
	std::map<std::string, EntityMaterial*> materialMap;
	for (auto& materialName : objectMaterialNames)
	{
		materialMap[materialName] = nullptr;
	}

	std::list<std::string> uniqueMaterialNames;
	for (auto& material : materialMap)
	{
		uniqueMaterialNames.push_back(material.first);
	}

	std::list<ot::EntityInformation> materialEntitiesInfo;
	ot::ModelServiceAPI::getEntityInformation(uniqueMaterialNames, materialEntitiesInfo);

	for (auto materialInfo : materialEntitiesInfo)
	{
		EntityMaterial* materialItem = dynamic_cast<EntityMaterial*> (ot::EntityAPI::readEntityFromEntityIDandVersion(materialInfo.getEntityID(), materialInfo.getEntityVersion()));
		if (materialItem == nullptr)
		{
			assert(0);
			continue;
		}

		materialMap[materialItem->getName()] = materialItem;
	}

	// Finally assign the material entities to the objects
	materialsOfObjects.reserve(objectMaterialNames.size());
	for (auto& materialName : objectMaterialNames)
	{
		materialsOfObjects.push_back(materialMap[materialName]);
	}
}

CartesianMeshToSTL::~CartesianMeshToSTL()
{
	// We need to delete all the unique materials (each material may be referred to several times)
	std::set<EntityMaterial*> uniqueMaterialSet;
	for (auto& material : materialsOfObjects)
	{
		uniqueMaterialSet.emplace(material);
	}

	for (auto& uniqueMaterial : uniqueMaterialSet)
	{
		delete uniqueMaterial;
	}

	materialsOfObjects.clear();
}

void CartesianMeshToSTL::processCellFaces(std::ostream& output, EntityMeshCartesianData* meshData, EntityMeshCartesianFace* meshFace, bool reverseFace)
{
	size_t numberFacesX = meshFace->getNumberCellFaces(0);
	size_t numberFacesY = meshFace->getNumberCellFaces(1);
	size_t numberFacesZ = meshFace->getNumberCellFaces(2);

	size_t nx = meshData->getNumberLinesX();
	size_t ny = meshData->getNumberLinesY();
	size_t nz = meshData->getNumberLinesZ();

	for (unsigned long index = 0; index < numberFacesX; index++)
	{
		long long faceIndex = meshFace->getCellFace(0, index);
		bool reverseQuad = faceIndex < 0 ? !reverseFace : reverseFace;
		faceIndex = abs(faceIndex);

		size_t iz = faceIndex / (nx * ny);
		size_t iy = (faceIndex - iz * nx * ny) / nx;
		size_t ix = (faceIndex - iz * nx * ny - iy * nx);

		assert(ix >= 0 && ix < nx);
		assert(iy >= 0 && iy < ny - 1);
		assert(iz >= 0 && iz < nz - 1);

		double x    = meshData->getMeshLinesX()[ix];
		double ymin = meshData->getMeshLinesY()[iy];
		double ymax = meshData->getMeshLinesY()[iy + 1];
		double zmin = meshData->getMeshLinesZ()[iz];
		double zmax = meshData->getMeshLinesZ()[iz + 1];

		if (!reverseQuad)
		{
			processQuad(output, x, ymin, zmin, 	x, ymax, zmin,	x, ymax, zmax,	x, ymin, zmax, 1.0, 0.0, 0.0);
		}
		else
		{
			processQuad(output, x, ymin, zmax,  x, ymax, zmax,	x, ymax, zmin,	x, ymin, zmin,-1.0, 0.0, 0.0);
		}
	}

	for (unsigned long index = 0; index < numberFacesY; index++)
	{
		long long faceIndex = meshFace->getCellFace(1, index);
		bool reverseQuad = faceIndex < 0 ? !reverseFace : reverseFace;
		faceIndex = abs(faceIndex);

		size_t iz = faceIndex / (nx * ny);
		size_t iy = (faceIndex - iz * nx * ny) / nx;
		size_t ix = (faceIndex - iz * nx * ny - iy * nx);

		assert(ix >= 0 && ix < nx - 1);
		assert(iy >= 0 && iy < ny);
		assert(iz >= 0 && iz < nz - 1);

		double y    = meshData->getMeshLinesY()[iy];
		double xmin = meshData->getMeshLinesX()[ix];
		double xmax = meshData->getMeshLinesX()[ix + 1];
		double zmin = meshData->getMeshLinesZ()[iz];
		double zmax = meshData->getMeshLinesZ()[iz + 1];

		if (reverseQuad)
		{
			processQuad(output, xmin, y, zmin, xmax, y, zmin, xmax, y, zmax, xmin, y, zmax, 0.0, -1.0, 0.0);

		}
		else
		{
			processQuad(output, xmin, y, zmax, xmax, y, zmax, xmax, y, zmin, xmin, y, zmin, 0.0, 1.0, 0.0);
		}
	}

	for (unsigned long index = 0; index < numberFacesZ; index++)
	{
		long long faceIndex = meshFace->getCellFace(2, index);
		bool reverseQuad = faceIndex < 0 ? !reverseFace : reverseFace;
		faceIndex = abs(faceIndex);

		size_t iz = faceIndex / (nx * ny);
		size_t iy = (faceIndex - iz * nx * ny) / nx;
		size_t ix = (faceIndex - iz * nx * ny - iy * nx);

		assert(ix >= 0 && ix < nx - 1);
		assert(iy >= 0 && iy < ny - 1);
		assert(iz >= 0 && iz < nz);

		double z = meshData->getMeshLinesZ()[iz];
		double xmin = meshData->getMeshLinesX()[ix];
		double xmax = meshData->getMeshLinesX()[ix + 1];
		double ymin = meshData->getMeshLinesY()[iy];
		double ymax = meshData->getMeshLinesY()[iy + 1];

		if (!reverseQuad)
		{
			processQuad(output, xmin, ymin, z, xmax, ymin, z, xmax, ymax, z, xmin, ymax, z, 0.0, 0.0, 1.0);
		}
		else
		{
			processQuad(output, xmin, ymax, z, xmax, ymax, z, xmax, ymin, z, xmin, ymin, z, 0.0, 0.0, -1.0);
		}
	}
}

void CartesianMeshToSTL::processQuad(std::ostream& output, double p1x, double p1y, double p1z, double p2x, double p2y, double p2z, double p3x, double p3y, double p3z, double p4x, double p4y, double p4z, double nx, double ny, double nz)
{
	// triangle 1: P1, P2, P3
	writeFacet(output, p1x, p1y, p1z, p2x, p2y, p2z, p3x, p3y, p3z, nx, ny, nz);

	// triangle 2: P1, P3, P4 
	writeFacet(output, p1x, p1y, p1z, p3x, p3y, p3z, p4x, p4y, p4z, nx, ny, nz);
}

void CartesianMeshToSTL::writeFacet(std::ostream& output, double v1x, double v1y, double v1z, double v2x, double v2y, double v2z, double v3x, double v3y, double v3z, double nx, double ny, double nz)
{
	output << "  facet normal " << nx << " " << ny << " " << nz << "\n";
	output << "    outer loop\n";
	output << "      vertex " << v1x << " " << v1y << " " << v1z << "\n";
	output << "      vertex " << v2x << " " << v2y << " " << v2z << "\n";
	output << "      vertex " << v3x << " " << v3y << " " << v3z << "\n";
	output << "    endloop\n";
	output << "  endfacet\n";
}
