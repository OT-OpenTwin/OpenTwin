#pragma once

#include "OldTreeIcon.h"

#include <string>

class Application;
class ModelComponent;

class MeshExport
{
public:
	MeshExport(Application *app) : application(app) {};
	~MeshExport() {};

	std::string getFileExtensions(void);
	void getExportFileContent(const std::string &extension, std::string &fileContent, unsigned long long &uncompressedDataLength);

	bool loadMesh(ot::UID currentMeshDataID);
	void cleanUp(void);

private:
	void ReadFileContent(const std::string &fileName, std::string &fileContent, unsigned long long &uncompressedDataLength);
	ot::UID  getGmshDataStorageID(ot::UID currentMeshDataID);
	void loadGmshData(ot::UID gmshDataStorageID, std::vector<char> &meshContent);

	Application *application;
	std::string tmpDir;

};
