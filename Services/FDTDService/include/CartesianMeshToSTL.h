// @otlicense
// File: CartesianMeshToSTL.h
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

#pragma once

#include "EntityInformation.h"

#include <tinyxml2.h>

#include <string>
#include <vector>
#include <list>

class EntityMaterial;
class EntityMeshCartesianData;
class EntityMeshCartesianFace;
class EntityMeshCartesianFaceList;

class CartesianMeshToSTL
{
public:
	CartesianMeshToSTL(const std::string& meshName, const std::string& tmpFolderName);
	virtual ~CartesianMeshToSTL();

	size_t getNumberOfObjects() const { return materialsOfObjects.size(); }
	std::string getFileNameOfObject(size_t id) const { if (id < 0 || id >= fileNamesOfObjects.size()) return "";  return fileNamesOfObjects[id]; }
	EntityMaterial* getMaterialOfObject(size_t id) const { if (id < 0 || id >= materialsOfObjects.size()) return nullptr; return materialsOfObjects[id]; }

	//! @brief Helper function to get the XML tag for a material. "Metal" (PEC) or "Material" (Volumetric)
	std::string getMaterialTag(const EntityMaterial* _material) const;

	//! @brief Helper function to write the material properties into the provided XML elements
	//! @param _material The material to write
	//! @param _doc The XML document
	//! @param _materialElement The XML element to write the material properties into
	//! @param _writeForPolyhedron If true, write the properties for a polyhedron definition, otherwise for a standard material definition
	void writeMaterialProperties(const EntityMaterial* _material, tinyxml2::XMLDocument* _doc, tinyxml2::XMLElement* _materialElement, bool _writeForPolyhedron) const;

	//! @brief Write the object information into an XML element
	//! @param _parentElement The parent XML element of the central XML element to write
	//! @return The created XML element containing the object information
	tinyxml2::XMLElement* writeToXML(tinyxml2::XMLElement& _parentElement) const;
	 
private:
	EntityMeshCartesianData* loadMeshDataEntity(const std::string& meshName);
	std::string processMeshEntity(const ot::EntityInformation& meshEntity, const std::string &objectName, const std::string &fileName, EntityMeshCartesianFaceList* meshFacesList, EntityMeshCartesianData* meshDataEntity);
	void processFaceQuads(std::ostream &output, EntityMeshCartesianData* meshData, EntityMeshCartesianFace* meshFace, bool reverseFace);
	void processQuad(std::ostream &output, double p1x, double p1y, double p1z, double p2x, double p2y, double p2z, double p3x, double p3y, double p3z, double p4x, double p4y, double p4z, double nx, double ny, double nz);
	void writeFacet(std::ostream &output, double v1x, double v1y, double v1z, double v2x, double v2y, double v2z, double v3x, double v3y, double v3z, double nx, double ny, double nz);
	void processRequiredMaterials(const std::list<std::string>& objectMaterialNames);

	std::vector<std::string> fileNamesOfObjects;
	std::vector<EntityMaterial*> materialsOfObjects;

};