// @otlicense
// File: CartesianMeshToSTL.cpp
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

#include "CartesianMeshToSTL.h"

#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityAPI.h"

#include "DataBase.h"
#include "EntityMaterial.h"
#include "EntityMeshCartesianData.h"
#include "EntityMeshCartesianItem.h"
#include "EntityMeshCartesianFaceList.h"
#include "EntityMeshCartesianFace.h"

#include <tinyxml2.h>

#include <fstream>
#include <map>
#include <set>
#include <stdexcept>

CartesianMeshToSTL::CartesianMeshToSTL(const std::string& meshName, const std::string& tmpFolderName)
{
	// Load the mesh data entity
	EntityMeshCartesianData* meshDataEntity = loadMeshDataEntity(meshName);

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
		std::string objectName = "object_" + std::to_string(objectIndex);
		std::string fileName   = tmpFolderName + "\\" + objectName + ".stl";

		std::string materialName = processMeshEntity(meshEntity, objectName, fileName, meshFacesList, meshDataEntity);

		fileNamesOfObjects.push_back(fileName);
		objectMaterialNames.push_back(materialName);

		objectIndex++;
	}

	// As a last step, we load all relevant materials and assign them to the materialsOfObjects vector
	processRequiredMaterials(objectMaterialNames);
}

std::string CartesianMeshToSTL::getMaterialTag(const EntityMaterial* _material) const {
	if (!_material) {
		// fallback to material
		return "Material";
	}
	const EntityProperties& properties = _material->getProperties();
	const auto type = _material->getProperties().getProperty("Material type", "General");
	if (!type) {
		// fallback to material
		return "Material";
	}
	auto selection = dynamic_cast<const EntityPropertiesSelection*>(type);
	if (!selection) {
		// fallback to material
		return "Material";
	}
	// Determine if PEC or general material
	const std::string& materialType = selection->getValue();
	return (materialType == "PEC") ? "Metal" : "Material";
}

void CartesianMeshToSTL::writeMaterialProperties(const EntityMaterial* _material, tinyxml2::XMLDocument* _doc, tinyxml2::XMLElement* _materialElement, bool _writeForPolyhedron) const {
	const EntityProperties& properties = _material->getProperties();
	const auto materialType = properties.getProperty("Material type", "General");
	const auto& materialSelection = dynamic_cast<const EntityPropertiesSelection*>(materialType)->getValue();

	// write priority only for polyhedron definitions, if requested
	if (_writeForPolyhedron) {
		const auto* priority = properties.getProperty("Mesh priority", "General");
		if (!priority) {
			throw std::runtime_error("[CartesianMeshToSTL::writeMaterialProperties] Missing mesh priority property");
		}
		const EntityPropertiesDouble* priorityValue = dynamic_cast<const EntityPropertiesDouble*>(priority);
		if (!priorityValue || priorityValue->getValue() < 0.0) {
			throw std::runtime_error("[CartesianMeshToSTL::writeMaterialProperties] Invalid mesh priority value");
		}
		_materialElement->SetAttribute("Priority", priorityValue->getValue());
		return;
	}
	
	if (materialSelection != "PEC") {
		tinyxml2::XMLElement* propertyElement = _doc->NewElement("Property");

		// Permittivity
		const auto* permittivity = properties.getProperty("Permittivity (relative)", "Electromagnetic");
		if (!permittivity) {
			throw std::runtime_error("[CartesianMeshToSTL::writeMaterialProperties] Missing permittivity property");
		}
		const EntityPropertiesDouble* epsilonValue = dynamic_cast<const EntityPropertiesDouble*>(permittivity);
		if (!epsilonValue || epsilonValue->getValue() < 0.0) {
			throw std::runtime_error("[CartesianMeshToSTL::writeMaterialProperties] Invalid permittivity value");
		}
		propertyElement->SetAttribute("Epsilon", epsilonValue->getValue());

		// Permeability
		const auto* permeability = properties.getProperty("Permeability (relative)", "Electromagnetic");
		if (!permeability) {
			throw std::runtime_error("[CartesianMeshToSTL::writeMaterialProperties] Missing permeability property");
		}
		const EntityPropertiesDouble* mueValue = dynamic_cast<const EntityPropertiesDouble*>(permeability);
		if (!mueValue || mueValue->getValue() < 0.0) {
			throw std::runtime_error("[CartesianMeshToSTL::writeMaterialProperties] Invalid permeability value");
		}
		propertyElement->SetAttribute("Mue", mueValue->getValue());
		
		// Conductivity
		const auto* conductivity = properties.getProperty("Conductivity", "Electromagnetic");
		if (!conductivity) {
			throw std::runtime_error("[CartesianMeshToSTL::writeMaterialProperties] Missing conductivity property");
		}
		const EntityPropertiesDouble* kappaValue = dynamic_cast<const EntityPropertiesDouble*>(conductivity);
		if (!kappaValue || kappaValue->getValue() < 0.0) {
			throw std::runtime_error("[CartesianMeshToSTL::writeMaterialProperties] Invalid conductivity value");
		}

		propertyElement->SetAttribute("Kappa", kappaValue->getValue());
		_materialElement->InsertEndChild(propertyElement);
	}
}

tinyxml2::XMLElement* CartesianMeshToSTL::writeToXML(tinyxml2::XMLElement& _parentElement) const {
	auto doc = _parentElement.GetDocument();
	tinyxml2::XMLElement* lastMaterialElement = nullptr;
	
	// Creating a map of materials to object indices
	std::map<EntityMaterial*, std::vector<size_t>> materialObjectsMap;
	for (size_t objIndex = 0; objIndex < getNumberOfObjects(); objIndex++) {
		EntityMaterial* material = getMaterialOfObject(objIndex);
		materialObjectsMap[material].push_back(objIndex);
	}

	// Iterate through the map and create XML elements for each material and assign the objects to that material
	for (const auto& [material, objIndices] : materialObjectsMap) {
		const auto& materialTag = getMaterialTag(material);
		tinyxml2::XMLElement* materialElement = doc->NewElement(materialTag.c_str());
		lastMaterialElement = materialElement;
		if (material != nullptr) {
			materialElement->SetAttribute("Name", material->getName().c_str());
			writeMaterialProperties(material, doc, materialElement, false);
		}

		tinyxml2::XMLElement* primitivesElement = doc->NewElement("Primitives");
		for (const auto& objIndex : objIndices) {
			tinyxml2::XMLElement* polyReaderElement = doc->NewElement("PolyhedronReader");
			polyReaderElement->SetAttribute("FileName", getFileNameOfObject(objIndex).c_str());
			polyReaderElement->SetAttribute("FileType", "STL");
			if (material != nullptr) {
				// Write material properties for the polyhedron
				// Other material properties will be ignored for the polyhedron definition
				writeMaterialProperties(material, doc, polyReaderElement, true);
			}
			primitivesElement->InsertEndChild(polyReaderElement);
		}
		materialElement->InsertEndChild(primitivesElement);
		_parentElement.InsertEndChild(materialElement);
	}
	return lastMaterialElement;
}

std::string CartesianMeshToSTL::processMeshEntity(const ot::EntityInformation &meshEntity, const std::string &objectName, const std::string &fileName, 
												  EntityMeshCartesianFaceList* meshFacesList, EntityMeshCartesianData* meshDataEntity)
{
	EntityMeshCartesianItem* meshItem = dynamic_cast<EntityMeshCartesianItem*> (ot::EntityAPI::readEntityFromEntityIDandVersion(meshEntity.getEntityID(), meshEntity.getEntityVersion()));
	if (meshItem == nullptr)
	{
		throw std::runtime_error("Error: Unable to load mesh item: " + meshEntity.getEntityName());
	}

	std::string materialName = meshItem->getMaterial();

	std::ofstream objectFile(fileName);
	if (!objectFile.is_open())
	{
		throw std::runtime_error("Error: Unable to open file: " + fileName);
	}

	objectFile << "solid " << objectName << std::endl;

	// Iterate through the faces assigned to this mesh item
	for (size_t faceIndex = 0; faceIndex < meshItem->getNumberFaces(); faceIndex++)
	{
		// bool reverseFace = meshItem->getFaceOrientation(faceIndex);
		int  faceId = abs(meshItem->getFaceId(faceIndex));

		EntityMeshCartesianFace* meshFace = meshFacesList->getFace(faceId);
		if (meshFace == nullptr)
		{
			throw std::runtime_error("Error: Mesh face is missing for mesh item: " + meshItem->getName());
		}

		if (meshFace->getType() != EntityMeshCartesianFace::INDEX_BASED)
		{
			throw std::runtime_error("Error: Mesh data is not stored index based for mesh item: " + meshItem->getName());
		}

		// Now we iterate through all directions and create the triangles for the quads belonging to this face
		processFaceQuads(objectFile, meshDataEntity, meshFace, true);
	}

	objectFile << "endsolid " << objectName << std::endl;

	delete meshItem;
	meshItem = nullptr;

	return materialName;
}

EntityMeshCartesianData* CartesianMeshToSTL::loadMeshDataEntity(const std::string& meshName)
{
	std::string meshDataName = meshName + "/Mesh";
	ot::EntityInformation meshDataInfo;
	ot::ModelServiceAPI::getEntityInformation(meshDataName, meshDataInfo);

	EntityMeshCartesianData* meshDataEntity = dynamic_cast<EntityMeshCartesianData*>(ot::EntityAPI::readEntityFromEntityIDandVersion(meshDataInfo.getEntityID(), meshDataInfo.getEntityVersion()));
	if (meshDataEntity == nullptr)
	{
		throw std::runtime_error("Error: Unable to load mesh data: " + meshDataInfo.getEntityName());
	}

	return meshDataEntity;
}

void CartesianMeshToSTL::processRequiredMaterials(const std::list<std::string> &objectMaterialNames)
{
	// Here we first determine the unique set of materials being used by the mesh objects
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

	// Get entity information for the unique materials
	std::list<ot::EntityInformation> materialEntitiesInfo;
	ot::ModelServiceAPI::getEntityInformation(uniqueMaterialNames, materialEntitiesInfo);

	// Load the unique materials and store them in a map
	for (auto materialInfo : materialEntitiesInfo)
	{
		EntityMaterial* materialItem = dynamic_cast<EntityMaterial*> (ot::EntityAPI::readEntityFromEntityIDandVersion(materialInfo.getEntityID(), materialInfo.getEntityVersion()));
		if (materialItem == nullptr)
		{
			// The material could not be found, so we cannot continue
			for (auto material : materialMap)
			{
				delete material.second;
			}

			throw std::runtime_error("Error: Unable to load material: " + materialInfo.getEntityName());
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

void CartesianMeshToSTL::processFaceQuads(std::ostream& output, EntityMeshCartesianData* meshData, EntityMeshCartesianFace* meshFace, bool reverseFace)
{
	// We process all quads belonging to a particular mesh face, convert them to two triangles each and write them to the output stream in STL format.
	
	size_t numberFacesX = meshFace->getNumberCellFaces(0);
	size_t numberFacesY = meshFace->getNumberCellFaces(1);
	size_t numberFacesZ = meshFace->getNumberCellFaces(2);

	size_t nx = meshData->getNumberLinesX();
	size_t ny = meshData->getNumberLinesY();
	size_t nz = meshData->getNumberLinesZ();

	// Process all cell quads with x normal direction
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

	// Process all cell quads with y normal direction
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

	// Process all cell quads with z normal direction
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
	// Subdivide a single quad into two triangles and write each of them to the output stream in STL format
	
	// triangle 1: P1, P2, P3
	writeFacet(output, p1x, p1y, p1z, p2x, p2y, p2z, p3x, p3y, p3z, nx, ny, nz);

	// triangle 2: P1, P3, P4 
	writeFacet(output, p1x, p1y, p1z, p3x, p3y, p3z, p4x, p4y, p4z, nx, ny, nz);
}

void CartesianMeshToSTL::writeFacet(std::ostream& output, double v1x, double v1y, double v1z, double v2x, double v2y, double v2z, double v3x, double v3y, double v3z, double nx, double ny, double nz)
{
	// Write a single triangle to the output stream in STL format

	output << "  facet normal " << nx << " " << ny << " " << nz << "\n";
	output << "    outer loop\n";
	output << "      vertex " << v1x << " " << v1y << " " << v1z << "\n";
	output << "      vertex " << v2x << " " << v2y << " " << v2z << "\n";
	output << "      vertex " << v3x << " " << v3y << " " << v3z << "\n";
	output << "    endloop\n";
	output << "  endfacet\n";
}
