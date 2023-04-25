#pragma once

#include <Types.h>

#include <string>

class Application;
class ModelComponent;

class MeshWriter;
class EntityMeshTet;

class MeshImport
{
public:
	MeshImport(Application *app) : application(app) {};
	~MeshImport() {};

	std::string getFileExtensions(void);

	void importMesh(const std::string &meshName, const std::string &originalName, const std::string &content, ot::UID uncompressedDataLength);


private:
	void CreateTmpFileFromCompressedData(std::string fileName, const std::string& data, ot::UID uncompressedDataLength);
	void writeMeshEntities(MeshWriter* meshWriter, EntityMeshTet* mesh, const std::string& materialsFolder, ot::UID materialsFolderID);
	void getNewColor(size_t count, double& colorR, double& colorG, double& colorB);

	Application *application;
	std::string tmpDir;

};
