#pragma once

#include <string>
#include <vector>

class EntityMaterial;
class EntityMeshCartesianData;
class EntityMeshCartesianFace;

class CartesianMeshToSTL
{
public:
	CartesianMeshToSTL(const std::string &meshName, const std::string& tmpFolderName);
	~CartesianMeshToSTL();

	size_t getNumberOfObjects() { return materialsOfObjects.size(); }
	std::string getFileNameOfObject(size_t id) { if (id < 0 || id >= fileNamesOfObjects.size()) return "";  return fileNamesOfObjects[id]; }
	EntityMaterial* getMaterialOfObject(size_t id) { if (id < 0 || id >= materialsOfObjects.size()) return nullptr; return materialsOfObjects[id]; }
	 
private:
	void processCellFaces(std::ostream &output, EntityMeshCartesianData* meshData, EntityMeshCartesianFace* meshFace, bool reverseFace);
	void processQuad(std::ostream &output, double p1x, double p1y, double p1z, double p2x, double p2y, double p2z, double p3x, double p3y, double p3z, double p4x, double p4y, double p4z, double nx, double ny, double nz);
	void writeFacet(std::ostream &output, double v1x, double v1y, double v1z, double v2x, double v2y, double v2z, double v3x, double v3y, double v3z, double nx, double ny, double nz);

	std::vector<std::string> fileNamesOfObjects;
	std::vector<EntityMaterial*> materialsOfObjects;
};