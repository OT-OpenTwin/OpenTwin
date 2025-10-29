// @otlicense

#include "MeshImport.h"

#include "Application.h"
#include "TmpFileManager.h"
#include "EntityMeshTet.h"
#include "EntityMeshTetData.h"
#include "EntityBinaryData.h"
#include "Database.h"
#include "MeshWriter.h"

#include "OTGui/FileExtension.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include <gmsh.h_cwrap>

#include "base64.h"
#include "zlib.h"

#include <fstream>
#include <filesystem>
#include <set>

std::string MeshImport::getFileExtensions(void)
{
	return ot::FileExtension::toFilterString({
		ot::FileExtension::GmshMesh,
		ot::FileExtension::Diffpack,
		ot::FileExtension::IDeasUniversal,
		ot::FileExtension::Med,
		ot::FileExtension::INRIAMedit,
		ot::FileExtension::NastranBulkData,
		ot::FileExtension::Plot3d,
		ot::FileExtension::STLSurface,
		ot::FileExtension::VRMLSurface,
		ot::FileExtension::Vtk,
		ot::FileExtension::Ply2Surface,
		ot::FileExtension::AllFiles
		});
}

void MeshImport::importMesh(const std::string& meshName, const std::string& originalName, const std::string& content, ot::UID uncompressedDataLength)
{
	ot::LockTypes lockFlags;

	lockFlags.setFlag(ot::LockType::ModelWrite);
	lockFlags.setFlag(ot::LockType::NavigationWrite);
	lockFlags.setFlag(ot::LockType::ViewWrite);
	lockFlags.setFlag(ot::LockType::Properties);

	application->getUiComponent()->lockUI(lockFlags);
	application->getUiComponent()->setProgressInformation("Reading mesh file", true);

	// Get information about the materials folder
	std::string materialsFolder = "Materials";

	std::list<std::string> entityList{ materialsFolder };
	std::list<ot::EntityInformation> entityInfo;

	ot::ModelServiceAPI::getEntityInformation(entityList, entityInfo);

	assert(entityInfo.size() == 1);
	assert(entityInfo.front().getEntityName() == materialsFolder);

	ot::UID materialsFolderID = entityInfo.front().getEntityID();

	// Store the mesh in a temporary file
	std::string tmpDir = TmpFileManager::createTmpDir();

	std::string extension    = std::filesystem::path(originalName).extension().string();
	std::string meshFileName = tmpDir + "\\mesh" + extension;

	CreateTmpFileFromCompressedData(meshFileName, content, uncompressedDataLength);

	// Load the mesh into gmsh
	gmsh::initialize();

	try
	{
		gmsh::open(meshFileName);
	}
	catch (std::string) 
	{
		application->getUiComponent()->displayErrorPrompt("ERROR: Importing mesh failed (unsupported format or corrupted file).");
		application->getUiComponent()->unlockUI(lockFlags);
		application->getUiComponent()->closeProgressInformation();

		return;
	}

	application->getUiComponent()->setProgressInformation("Converting mesh data", false);
	application->getUiComponent()->setProgress(0);

	// Delete the temporary file
	TmpFileManager::deleteDirectory(tmpDir);

	// Renumber the nodes
	gmsh::model::mesh::renumberNodes();
	gmsh::model::mesh::renumberElements();

	// Create a new tet mesh entity
	EntityMeshTet* meshEntity = new EntityMeshTet(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr, application->getServiceName());

	EntityPropertiesString::createProperty("General", "File name", originalName, "", meshEntity->getProperties());

	meshEntity->setName(meshName);
	meshEntity->setEditable(true);

	// Create a new empty data entity
	meshEntity->getMeshData()->setName(meshEntity->getName() + "/Mesh");
	meshEntity->getMeshData()->setEntityID(application->getModelComponent()->createEntityUID());

	// Now we call the mesh writer to store the mesh

	MeshWriter meshWriter(application, meshEntity);

	// Store the nodes
	meshWriter.convertAndStoreNodes();
	application->getUiComponent()->setProgress(10);

	// In a next step, we convert all mesh faces and store them as triangle lists in the mesh entity
	meshWriter.convertFaces();
	application->getUiComponent()->setProgress(20);

	// In a next step, process each mesh entity and create a corresponding mesh item to store the surface mesh and volume mesh
	writeMeshEntities(&meshWriter, meshEntity, materialsFolder, materialsFolderID);

	// Now store all mesh face entities
	meshWriter.storeFaces();
	application->getUiComponent()->setProgress(80);

	// Next we write the gmsh mesh file into the database
	meshWriter.storeMeshFile();
	application->getUiComponent()->setProgress(90);

	// Write the properties of the mesh data entity
	EntityPropertiesBoolean::createProperty("Mesh Visualization", "Show volume mesh", false, "", meshEntity->getMeshData()->getProperties());

	EntityPropertiesString::createProperty("Mesh Dimensions", "Number of nodes", std::to_string(meshWriter.getNumberOfNodes()), "", meshEntity->getMeshData()->getProperties());
	EntityPropertiesString::createProperty("Mesh Dimensions", "Number of triangles", std::to_string(meshWriter.getNumberOfTriangles()), "", meshEntity->getMeshData()->getProperties());
	EntityPropertiesString::createProperty("Mesh Dimensions", "Number of tetrahedrons", std::to_string(meshWriter.getNumberOfTets()), "", meshEntity->getMeshData()->getProperties());

	meshEntity->getMeshData()->getProperties().getProperty("Number of nodes")->setReadOnly(true);
	meshEntity->getMeshData()->getProperties().getProperty("Number of triangles")->setReadOnly(true);
	meshEntity->getMeshData()->getProperties().getProperty("Number of tetrahedrons")->setReadOnly(true);

	// Finally, store all the newly created objects

	// Store the mesh data entity
	meshEntity->storeMeshData();
	application->getModelComponent()->addNewTopologyEntity(meshEntity->getMeshData()->getEntityID(), meshEntity->getMeshData()->getEntityStorageVersion(), false);
	
	// Store the mesh entity itself
	meshEntity->storeToDataBase();
	application->getModelComponent()->addNewTopologyEntity(meshEntity->getEntityID(), meshEntity->getEntityStorageVersion(), false);

	// Finalize the operation and send all newly created entities to the modeler
	application->getModelComponent()->storeNewEntities("import mesh: " + originalName);

	application->getUiComponent()->setProgress(100);
	application->getUiComponent()->unlockUI(lockFlags);
	application->getUiComponent()->closeProgressInformation();
}

void MeshImport::writeMeshEntities(MeshWriter* meshWriter, EntityMeshTet* mesh, const std::string& materialsFolder, ot::UID materialsFolderID)
{
	int progressStart = 20;
	int progressEnd = 70;

	// List of all entity names used so far
	std::set<std::string> entityNames;

	// Get all volumes from the model
	gmsh::vectorpair volumes;
	gmsh::model::getEntities(volumes, 3);

	if (volumes.empty()) return;

	double progressInc = 1.0 * (progressEnd - progressStart) / volumes.size();
	double progressNow = progressStart;

	int solidCount = 1;
	for (auto volume : volumes)
	{
		int entityTag = volume.second;
		std::string entityName;

		// Try to find a physical group name for the entity
		std::vector<int> physicalTags;
		gmsh::model::getPhysicalGroupsForEntity(3, entityTag, physicalTags);

		for (auto tag : physicalTags)
		{
			gmsh::model::getPhysicalName(3, tag, entityName);
			if (!entityName.empty()) break;
		}

		// If the name begins with a *, (exportet from OpenTwin), remove this *
		if (!entityName.empty())
		{
			if (entityName[0] == '*')
			{
				entityName = entityName.substr(1);
			}
		}

		// If the name is empty, create a default name
		if (entityName.empty())
		{
			entityName = "solid " + std::to_string(solidCount);
			solidCount++;
		}

		// Now make the name unique
		if (entityNames.contains(entityName))
		{
			// The name is already used, append _#
			int count = 1;

			std::string baseName = entityName;
			
			do 
			{
				entityName = baseName + "_" + std::to_string(count);
				count++;

			} while (entityNames.contains(entityName));
		}
		entityNames.insert(entityName);

		// And finally create the mesh entity
		std::string meshEntityName = mesh->getName() + "/Mesh/" + entityName;

		double colorR(0.0), colorG(0.0), colorB(0.0);
		getNewColor(entityNames.size(), colorR, colorG, colorB);

		meshWriter->storeMeshEntityFromPhysicalGroup(meshEntityName, entityTag, colorR, colorG, colorB, materialsFolder, materialsFolderID);

		progressNow += progressInc;
		application->getUiComponent()->setProgress((int) progressNow);
	}

	application->getUiComponent()->setProgress(progressEnd);
}

void MeshImport::CreateTmpFileFromCompressedData(std::string fileName, const std::string& data, ot::UID uncompressedDataLength)
{
	// Decode the encoded string into binary data
	int decoded_compressed_data_length = Base64decode_len(data.c_str());
	char* decodedCompressedString = new char[decoded_compressed_data_length];

	Base64decode(decodedCompressedString, data.c_str());

	// Decompress the data
	char* decodedString = new char[uncompressedDataLength];
	uLongf destLen = (uLongf)uncompressedDataLength;
	uLong  sourceLen = decoded_compressed_data_length;
	uncompress((Bytef*)decodedString, &destLen, (Bytef*)decodedCompressedString, sourceLen);

	delete[] decodedCompressedString;
	decodedCompressedString = nullptr;

	std::ofstream file(fileName, std::ios::binary);
	file.write(decodedString, uncompressedDataLength);
	file.close();

	delete[] decodedString;
	decodedString = nullptr;
}

void MeshImport::getNewColor(size_t count, double& colorR, double& colorG, double& colorB)
{
	std::vector<std::vector<int>> groupColors = { {0, 255, 255}, {76, 255, 0}, {255, 216, 0}, {178, 0, 255}, {255, 127, 127} };

	colorR = groupColors[count % groupColors.size()][0] / 255.0;
	colorG = groupColors[count % groupColors.size()][1] / 255.0;
	colorB = groupColors[count % groupColors.size()][2] / 255.0;
}

