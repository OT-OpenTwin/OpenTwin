// @otlicense

#include "MeshExport.h"

#include "Application.h"
#include "TmpFileManager.h"
#include "EntityMeshTetData.h"
#include "EntityBinaryData.h"

#include "EntityAPI.h"
#include "OTServiceFoundation/ModelComponent.h"

#include <gmsh.h_cwrap>

#include "base64.h"
#include "zlib.h"

#include <fstream>

std::string MeshExport::getFileExtensions(void)
{
	return 
		"Gmsh MSH(*.msh);;"
		"Abaqus INP (*.inp);;"
		"LSDYNA KEY (*.key);;"
		"CELUM (.celum);;"
		"Diffpack 3D (.diff);;"
		"I-deas Universal (*.unv);;"
		"Iridium (*ir3);;"
		"MED (*.med);;"
		"INRIA Medit (*.mesh);;"
		"CEA Triangulation (*.mail);;"
		"Matlab (*.m);;"
		"Nastran Bulk Data File (*.bdf);;"
		"Plot3D Structured Mesh (*.p3d);;"
		"STL Surface (*.stl);;"
		"VRML Surface (*.wrl);;"
		"VTK (*.vtk);;"
		"Tochnog (*.dat);;"
		"PLY2 Surface (*.ply2);;"
		"SU2 (*.su2);;"
		"GAMBIT Neutral File (*.neu)";
}

void MeshExport::getExportFileContent(const std::string &extension, std::string &fileContent, unsigned long long &uncompressedDataLength)
{
	// Save the msh data to the temporary directory
	std::string exportMshFile = tmpDir + "\\out" + extension;

	gmsh::option::setNumber("Mesh.Binary", 0);   // We don't want binary files for the export
	gmsh::option::setNumber("Mesh.MshFileVersion", 2.2); // For compatibility with Palace
	gmsh::write(exportMshFile);

	// Read the file content of msh file and return it to the caller
	ReadFileContent(exportMshFile, fileContent, uncompressedDataLength);
}

void MeshExport::ReadFileContent(const std::string &fileName, std::string &fileContent, unsigned long long &uncompressedDataLength)
{
	fileContent.clear();

	// Read the file content
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	int data_length = (int)file.tellg();
	file.seekg(0, std::ios::beg);

	char *data = new char[data_length];
	if (!file.read(data, data_length)) return;

	uncompressedDataLength = data_length;

	// Compress the file data content
	uLong compressedSize = compressBound((uLong)data_length);

	char *compressedData = new char[compressedSize];
	compress((Bytef*)compressedData, &compressedSize, (Bytef*)data, data_length);

	delete[] data;
	data = nullptr;

	// Convert the binary to an encoded string
	int encoded_data_length = Base64encode_len(compressedSize);
	char* base64_string = new char[encoded_data_length];

	Base64encode(base64_string, compressedData, compressedSize); // "base64_string" is a then null terminated string that is an encoding of the binary data pointed to by "data"

	delete[] compressedData;
	compressedData = nullptr;

	fileContent = std::string(base64_string);

	delete[] base64_string;
	base64_string = nullptr;
}

bool MeshExport::loadMesh(ot::UID currentMeshDataID)
{
	// Determine the gmsh data storage entity id
	ot::UID gmshDataStorageID = getGmshDataStorageID(currentMeshDataID);
	if (gmshDataStorageID == 0) return false;

	// Now load the gmsh data storage
	std::vector<char> meshContent;
	loadGmshData(gmshDataStorageID, meshContent);
	if (meshContent.empty()) return false;

	// In a next step, create a temporary directory
	tmpDir = TmpFileManager::createTmpDir();

	// Now save the mesh data into the temp dir
	std::string mshFile = tmpDir + "\\in.msh";

	std::ofstream file(mshFile, std::ios::binary);
	file.write(meshContent.data(), meshContent.size());
	file.close();

	// Load the data into gmsh
	gmsh::initialize();
	gmsh::open(mshFile);

	return true;
}

void MeshExport::cleanUp(void)
{
	if (tmpDir.empty()) return;

	// Terminate gmsh
	gmsh::finalize();

	// Delete the temporaty directory
	TmpFileManager::deleteDirectory(tmpDir);
	tmpDir.clear();
}

ot::UID MeshExport::getGmshDataStorageID(ot::UID currentMeshDataID)
{
	ot::UIDList entityList;
	entityList.push_back(currentMeshDataID);
	application->prefetchDocumentsFromStorage(entityList);

	EntityMeshTetData *entity = dynamic_cast<EntityMeshTetData*> (ot::EntityAPI::readEntityFromEntityIDandVersion(currentMeshDataID, application->getPrefetchedEntityVersion(currentMeshDataID)));
	if (entity == nullptr) return 0;

	ot::UID gmshDataStorageID = entity->getGmshDataStorageId();

	delete entity;
	entity = nullptr;

	return gmshDataStorageID;
}

void MeshExport::loadGmshData(ot::UID gmshDataStorageID, std::vector<char> &meshContent)
{
	ot::UIDList entityList;
	entityList.push_back(gmshDataStorageID);
	application->prefetchDocumentsFromStorage(entityList);

	EntityBinaryData *entityData = dynamic_cast<EntityBinaryData*> (ot::EntityAPI::readEntityFromEntityIDandVersion(gmshDataStorageID, application->getPrefetchedEntityVersion(gmshDataStorageID)));
	if (entityData == nullptr) return;

	meshContent = entityData->getData();

	delete entityData;
	entityData = nullptr;
}


