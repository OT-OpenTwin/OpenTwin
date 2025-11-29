// @otlicense

#pragma once

#include <string>
#include <vector>
#include <list>

#include <tinyxml2.h>

#include "EntityInformation.h"

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
	void writeMaterialProperties(const EntityMaterial* _material, tinyxml2::XMLDocument* _doc, tinyxml2::XMLElement* _materialElement, tinyxml2::XMLElement* _polyReaderElement) const;
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