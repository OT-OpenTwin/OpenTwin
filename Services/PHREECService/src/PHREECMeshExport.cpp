#include "PHREECMeshExport.h"

#include "Application.h"

#include "DataBase.h"
#include "EntityProperties.h"
#include "EntityMaterial.h"
#include "EntityMeshTetItemDataTets.h"
#include "ClassFactory.h"

#include "DocumentAPI.h"
#include "Connection\ConnectionAPI.h"
#include "Document\DocumentManager.h"
#include "Document\DocumentAccessBase.h"
#include "Helper\QueryBuilder.h"
#include "Helper\BsonValuesHelper.h"

#include "EntityInformation.h"
#include "OTServiceFoundation/ModelComponent.h"

#include "EntityAPI.h"

#include "OTModelAPI/ModelServiceAPI.h"

#include <fstream>
#include <iomanip>

PHREECMeshExport::PHREECMeshExport(Application *app) :
	isConnected(false),
	nextSectionIndex(0),
	application(app)
{

}

PHREECMeshExport::~PHREECMeshExport()
{

}

std::string PHREECMeshExport::exportMeshData(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshEntityID, const std::string &outputFileBase, ot::components::ModelComponent *modelComponent)
{
//	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIds);
	// Load the material information
	application->modelComponent()->loadMaterialInformation();

	std::string error;

	std::string meshOutputFileName = outputFileBase + ".msh";
	std::string infoOutputFileName = outputFileBase + ".pmd";
	std::string debugOutputFileName = outputFileBase + ".dbg";

	bool debug = false;

	// Open the binary output file first 
	std::ofstream meshFile(meshOutputFileName, std::ofstream::binary);

	// If requested open a debug ascii file for debugging purposes
	std::ofstream debugFile;
	if (debug)
	{
		debugFile.open(debugOutputFileName);
		debugFile << std::scientific;
	}

	// Write the mesh nodes

	ot::UIDList entityList{ meshEntityID };
	application->prefetchDocumentsFromStorage(entityList);
	unsigned long long meshEntityVersion = application->getPrefetchedEntityVersion(meshEntityID);

	auto meshDoc = bsoncxx::builder::basic::document{};
	error = readDocument(dataBaseURL, projectName, meshEntityID, meshEntityVersion, meshDoc, "EntityMeshTet", 1);
	if (!error.empty()) return error;

	auto meshDocView = meshDoc.view()["Found"].get_document().view();

	unsigned long long meshDataID = meshDocView["MeshData"].get_int64();
	entityList = ot::UIDList{ meshDataID };

	application->prefetchDocumentsFromStorage(entityList);

	unsigned long long meshDataVersion = application->getPrefetchedEntityVersion(meshDataID);

	std::string meshName = meshDocView["Name"].get_utf8().value.data();

	auto meshDataDoc = bsoncxx::builder::basic::document{};
	error = readDocument(dataBaseURL, projectName, meshDataID, meshDataVersion, meshDataDoc, "EntityMeshTetData", 1);
	if (!error.empty()) return error;

	auto meshDataView = meshDataDoc.view()["Found"].get_document().view();

	unsigned long long meshNodesID = meshDataView["MeshNodesID"].get_int64();
	unsigned long long meshFacesID = meshDataView["MeshFacesID"].get_int64();

	unsigned long long meshNodesVersion = meshDataView["MeshNodesVersion"].get_int64();
	unsigned long long meshFacesVersion = meshDataView["MeshFacesVersion"].get_int64();

	std::list<std::pair<ot::UID, ot::UID>> prefetchList{ std::pair<ot::UID, ot::UID>{meshNodesID, meshNodesVersion},  std::pair<ot::UID, ot::UID>{meshFacesID, meshFacesVersion} };
	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchList);

	error = exportMeshNodes(dataBaseURL, projectName, meshNodesID, meshNodesVersion, meshFile, debug, debugFile);
	if (!error.empty()) return error;

	// Now we get a list of all EntityMeshTetItem entites below the Mesh folder
	ot::UIDList allMeshItems = ot::ModelServiceAPI::getIDsOfFolderItemsOfType(meshName + "/Mesh", "EntityMeshTetItem", true);

	application->prefetchDocumentsFromStorage(allMeshItems);

	// Now we search for faces with an annotation and write them to the mesh file

	std::vector<std::string> triangleSectionNames;
	std::list<triangle> triangleList;

 	error = exportMeshFaces(dataBaseURL, projectName, meshFacesID, meshFacesVersion, triangleSectionNames, triangleList);
	if (!error.empty()) return error;

	// Write the number of triangles to the mesh file
	size_t numberOfTriangles = triangleList.size();
	meshFile.write(reinterpret_cast<char *>(&numberOfTriangles), sizeof(size_t));
	if (debug) debugFile << "<size_t> " << std::setw(6) << numberOfTriangles << " [Number of triangles]" << std::endl;

	// Write the mesh nodes of the tets
	size_t tcount = 1;
	for (auto triangle : triangleList)
	{
		meshFile.write(reinterpret_cast<char *>(&triangle.nodes[0]), sizeof(int));
		meshFile.write(reinterpret_cast<char *>(&triangle.nodes[1]), sizeof(int));
		meshFile.write(reinterpret_cast<char *>(&triangle.nodes[2]), sizeof(int));

		if (debug)
		{
			debugFile << "<int> " << std::setw(6) << triangle.nodes[0] << " [Triangle " << tcount << " node 1]" << std::endl;
			debugFile << "<int> " << std::setw(6) << triangle.nodes[1] << " [Triangle " << tcount << " node 2]" << std::endl;
			debugFile << "<int> " << std::setw(6) << triangle.nodes[2] << " [Triangle " << tcount << " node 3]" << std::endl;
		}

		tcount++;
	}

	// Write the section information for each tet
	tcount = 1;
	for (auto triangle : triangleList)
	{
		meshFile.write(reinterpret_cast<char *>(&triangle.sectionID), sizeof(int));
		if (debug)
		{
			debugFile << "<int> " << std::setw(6) << triangle.sectionID << " [Triangle " << tcount << " section]" << std::endl;
		}

		tcount++;
	}

	// Now read the tetrahedrons for the individual mesh items ans store them in a list

	prefetchList.clear();

	std::list<std::pair<std::string, std::string>> objectMaterialList;
	std::map<ot::UID, ot::UID> meshDataTetsID;
	std::map<ot::UID, ot::UID> meshDataTetsVersion;
	std::map<ot::UID, int> sectionID;

	for (auto meshItem : allMeshItems)
	{
		unsigned long long meshItemID = meshItem;
		unsigned long long meshItemVersion = application->getPrefetchedEntityVersion(meshItemID);

		error = prefetchTetList(dataBaseURL, projectName, meshItemID, meshItemVersion, prefetchList, objectMaterialList, meshDataTetsID, meshDataTetsVersion, sectionID);
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchList);

	std::list<tetrahedron> tetList;

	for (auto meshItem : allMeshItems)
	{
		unsigned long long meshItemID = meshItem;

		// This function will also create a new section entry to each mesh item and assign the section ID to the tet
		error = readTetList(dataBaseURL, projectName, meshItemID, tetList, meshDataTetsID, meshDataTetsVersion, sectionID);
		if (!error.empty()) return error;
	}

	// Create the material information
	std::vector<std::string> tetrahedronSectionNames;
	processMaterialData(meshName, objectMaterialList, tetrahedronSectionNames, modelComponent);

	// Write the number of tetrahedrons to the mesh file
	size_t numberOfTets = tetList.size();
	meshFile.write(reinterpret_cast<char *>(&numberOfTets), sizeof(size_t));
	if (debug) debugFile << "<size_t> " << std::setw(6) << numberOfTets << " [Number of tetrahedrons]" << std::endl;

	// Write the mesh nodes of the tets
	tcount = 1;
	for (auto tet : tetList)
	{
		meshFile.write(reinterpret_cast<char *>(&tet.nodes[0]), sizeof(int));
		meshFile.write(reinterpret_cast<char *>(&tet.nodes[1]), sizeof(int));
		meshFile.write(reinterpret_cast<char *>(&tet.nodes[2]), sizeof(int));
		meshFile.write(reinterpret_cast<char *>(&tet.nodes[3]), sizeof(int));

		if (debug)
		{
			debugFile << "<int> " << std::setw(6) << tet.nodes[0] << " [Tetrahedron " << tcount << " node 1]" << std::endl;
			debugFile << "<int> " << std::setw(6) << tet.nodes[1] << " [Tetrahedron " << tcount << " node 2]" << std::endl;
			debugFile << "<int> " << std::setw(6) << tet.nodes[2] << " [Tetrahedron " << tcount << " node 3]" << std::endl;
			debugFile << "<int> " << std::setw(6) << tet.nodes[3] << " [Tetrahedron " << tcount << " node 4]" << std::endl;
		}

		tcount++;
	}

	// Write the section information for each tet
	tcount = 1;
	for (auto tet : tetList)
	{
		meshFile.write(reinterpret_cast<char *>(&tet.sectionID), sizeof(int));

		if (debug)
		{
			debugFile << "<int> " << std::setw(6) << tet.sectionID << " [Tetrahedron " << tcount << " section]" << std::endl;
		}

		tcount++;
	}

	// Finally we write information about the sections
	// Number of sections
	size_t numberOfSections = triangleSectionNames.size() + tetrahedronSectionNames.size();
	meshFile.write(reinterpret_cast<char *>(&numberOfSections), sizeof(size_t));
	if (debug) debugFile << std::setw(6) << "<size_t> " << numberOfSections << " [Number of sections]" << std::endl;

	// For each section, we write the name (length and char *)
	for (auto section : triangleSectionNames)
	{
		size_t length = section.size();
		meshFile.write(reinterpret_cast<char *>(&length), sizeof(size_t));
		meshFile.write(reinterpret_cast<const char *>(section.c_str()), length * sizeof(char));

		if (debug)
		{
			debugFile << "<size_t> " << std::setw(6) << length << " [Length of section name]" << std::endl;
			debugFile << "<char*> " << section << " [Section name]" << std::endl;
		}
	}

	for (auto section : tetrahedronSectionNames)
	{
		size_t length = section.size();
		meshFile.write(reinterpret_cast<char *>(&length), sizeof(size_t));
		meshFile.write(reinterpret_cast<const char *>(section.c_str()), length * sizeof(char));

		if (debug)
		{
			debugFile << "<size_t> " << std::setw(6) << length << " [Length of section name]" << std::endl;
			debugFile << "<char*> " << section << " [Section name]" << std::endl;
		}
	}

	meshFile.close();
	if (debug) debugFile.close();

	// Now write the additional ASCII information file
	std::ofstream infoFile(infoOutputFileName);

	size_t index = meshOutputFileName.rfind('\\');
	std::string meshOutputFileNameRelative = meshOutputFileName.substr(index + 1);

	// Write the general information
	infoFile << "PHREEC CGNS Model" << std::endl;
	infoFile << meshOutputFileNameRelative << std::endl;
	infoFile << "mm" << std::endl;
	infoFile << "electric" << std::endl;

	// Write the triangle list names

	infoFile << "[";
	bool first = true;

	for (auto section : triangleSectionNames)
	{
		if (!first) infoFile << ",";
		first = false;
		infoFile << "'" << section << "'";
	}
	infoFile << "]" << std::endl;

	// Write the material information (tetrahedron list names)
	infoFile << std::scientific;

	for (auto mat : materialList)
	{
		infoFile << mat.name << " = " << mat.epsilon << ", " << mat.mu << ", " << mat.sigma << std::endl;
	}

	infoFile.close();

	return error;
}

void PHREECMeshExport::processMaterialData(const std::string &meshName, std::list<std::pair<std::string, std::string>> &objectMaterialList, std::vector<std::string> &tetrahedronSectionNames,
										   ot::components::ModelComponent *modelComponent)
{
	// Build a map of all materials used in the mesh
	std::map<std::string, EntityMaterial *> materialMap;
	for (auto item : objectMaterialList)
	{
		materialMap[item.second] = nullptr;
	}

	// Now retrieve the information about the material items
	std::list<std::string> materialNamesList;
	for (auto material : materialMap)
	{
		materialNamesList.push_back(material.first);
	}

	std::list<ot::EntityInformation> materialInfo;
	ot::ModelServiceAPI::getEntityInformation(materialNamesList, materialInfo);

	// Load the materials one by one and attach them to the map

	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIds;
	for (auto info : materialInfo)
	{
		prefetchIds.push_back(std::pair<unsigned long long, unsigned long long>(info.getEntityID(), info.getEntityVersion()));
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIds);

	for (auto info : materialInfo)
	{
		materialMap[info.getEntityName()] = dynamic_cast<EntityMaterial *>(ot::EntityAPI::readEntityFromEntityIDandVersion(info.getEntityID(), info.getEntityVersion(), application->getClassFactory()));
	}

	// Finally process the list of sections and create the material
	for (auto item : objectMaterialList)
	{
		material mat = createNewMaterial(meshName, item.first, materialMap[item.second]);
		materialList.push_back(mat);

		tetrahedronSectionNames.push_back(mat.name);
	}
}

std::string PHREECMeshExport::prefetchTetList(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshItemID, unsigned long long meshItemVersion,
									          std::list<std::pair<ot::UID, ot::UID>> &prefetchList, std::list<std::pair<std::string, std::string>> &objectMaterialList,
											  std::map<ot::UID, ot::UID> &meshDataTetsID, std::map<ot::UID, ot::UID> &meshDataTetsVersion, std::map<ot::UID, int> &sectionID)
{
	std::string error;

	// We first read the EntityMeshItem information for the given ID

	auto meshItemDoc = bsoncxx::builder::basic::document{};

	error = readDocument(dataBaseURL, projectName, meshItemID, meshItemVersion, meshItemDoc, "", 0);
	if (!error.empty()) return error;

	auto meshItemDocView = meshItemDoc.view()["Found"].get_document().view();
	std::string entitySchemaType = meshItemDocView["SchemaType"].get_utf8().value.data();

	int schemaVersion = (int)DataBase::GetIntFromView(meshItemDocView, std::string("SchemaVersion_EntityMeshTetItem").c_str());
	if (schemaVersion < 1)
	{
		return "Too early version of document with entity ID" + std::to_string(meshItemID) + " (expected version at least 1)";
	}

	std::string meshItemName = meshItemDocView["Name"].get_utf8().value.data();

	meshDataTetsID[meshItemID]      = meshItemDocView["MeshDataTetsID"].get_int64();
	meshDataTetsVersion[meshItemID] = meshItemDocView["MeshDataTetsVersion"].get_int64();

	prefetchList.push_back(std::pair<ot::UID, ot::UID>(meshDataTetsID[meshItemID], meshDataTetsVersion[meshItemID]));

	sectionID[meshItemID] = nextSectionIndex;
	nextSectionIndex++;

	// Read the material properties
	auto bsonObj = meshItemDocView["Properties"].get_document();
	std::string propertiesJSON = bsoncxx::to_json(bsonObj);
	EntityProperties properties;
	properties.buildFromJSON(propertiesJSON, nullptr);

	EntityPropertiesEntityList *materialProperty = dynamic_cast<EntityPropertiesEntityList*>(properties.getProperty("Material"));

	if (materialProperty != nullptr)
	{
		std::string materialName = application->modelComponent()->getCurrentMaterialName(materialProperty);

		objectMaterialList.push_back(std::pair<std::string, std::string>(meshItemName, materialName));
	}
	else
	{
		objectMaterialList.push_back(std::pair<std::string, std::string>(meshItemName, ""));
	}

	return error;
}

std::string PHREECMeshExport::readTetList(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshItemID,
										  std::list<tetrahedron> &tetList, std::map<ot::UID, ot::UID> &meshDataTetsID, std::map<ot::UID, ot::UID> &meshDataTetsVersion,
										  std::map<ot::UID, int> &sectionID)
{
	std::string error;

	// And finally read the MeshDataTets document to read the tetrahedrons

	auto meshTetsDoc = bsoncxx::builder::basic::document{};
	error = readDocument(dataBaseURL, projectName, meshDataTetsID[meshItemID], meshDataTetsVersion[meshItemID], meshTetsDoc, "EntityMeshTetItemDataTets", 1);
	if (!error.empty()) return error;

	auto meshTetsDocView = meshTetsDoc.view()["Found"].get_document().view();

	try
	{
		// This is new code for reading the tets where each tet can have an arbitrary order
		auto nodes = meshTetsDocView["Nodes"].get_array().value;

		auto n = nodes.begin();

		// Read the number of triangles
		size_t numberTets = DataBase::GetIntFromArrayViewIterator(n);
		n++;

		int thisSectionID = sectionID[meshItemID];

		for (unsigned long index = 0; index < numberTets; index++)
		{
			int numberNodes = (int)DataBase::GetIntFromArrayViewIterator(n);
			n++;

			if (numberNodes != 4) return "The solver currently supports first order meshes only.";

			// The indices in the mesh file are 1 based whereas the database entries are zero based. 
			// Therefore we need to add 1 here.

			tetrahedron tet;
			tet.nodes[0] = (int)getIntFromIterator(n) + 1; n++;
			tet.nodes[1] = (int)getIntFromIterator(n) + 1; n++;
			tet.nodes[2] = (int)getIntFromIterator(n) + 1; n++;
			tet.nodes[3] = (int)getIntFromIterator(n) + 1; n++;
			tet.sectionID = thisSectionID;

			tetList.push_back(tet);
		}
	}
	catch (std::exception)
	{
		// This is for backward compatibility

		auto node1 = meshTetsDocView["Node1"].get_array().value;
		auto node2 = meshTetsDocView["Node2"].get_array().value;
		auto node3 = meshTetsDocView["Node3"].get_array().value;
		auto node4 = meshTetsDocView["Node4"].get_array().value;

		size_t numberTets = std::distance(node1.begin(), node1.end());
		assert(numberTets == std::distance(node2.begin(), node2.end()));
		assert(numberTets == std::distance(node3.begin(), node3.end()));
		assert(numberTets == std::distance(node4.begin(), node4.end()));

		auto n1 = node1.begin();
		auto n2 = node2.begin();
		auto n3 = node3.begin();
		auto n4 = node4.begin();

		int thisSectionID = sectionID[meshItemID];

		for (unsigned long index = 0; index < numberTets; index++)
		{
			// The indices in the mesh file are 1 based whereas the database entries are zero based. 
			// Therefore we need to add 1 here.

			tetrahedron tet;
			tet.nodes[0] = (int)getIntFromIterator(n1) + 1;
			tet.nodes[1] = (int)getIntFromIterator(n2) + 1;
			tet.nodes[2] = (int)getIntFromIterator(n3) + 1;
			tet.nodes[3] = (int)getIntFromIterator(n4) + 1;
			tet.sectionID = thisSectionID;

			tetList.push_back(tet);

			n1++;
			n2++;
			n3++;
			n4++;
		}
	}

	return error;
}

material PHREECMeshExport::createNewMaterial(const std::string &meshName, const std::string &meshItemName, EntityMaterial *materialData)
{
	// Build a new object name
	std::string strip = meshName + "/Mesh/";
	std::string objectName = meshItemName.substr(strip.size());

	// Replace forbidden characters
	std::replace( objectName.begin(), objectName.end(), ' ', '_');
	std::replace( objectName.begin(), objectName.end(), '=', '-');

	if (materialNamesUsed.count(objectName) > 0)
	{
		int count = 1;
		while (materialNamesUsed.count(objectName + "_" + std::to_string(count)) > 0) count++;
		objectName += "_" + std::to_string(count);
	}

	// As long as we do not have a proper material management, we defined the material based on the object name
	material mat;

	mat.name = objectName;

	std::string backgroundName = meshName + "/Mesh/Background";

	if (materialData != nullptr)
	{
		EntityPropertiesDouble *permittivity = dynamic_cast<EntityPropertiesDouble*>(materialData->getProperties().getProperty("Permittivity (relative)"));
		EntityPropertiesDouble *permeability = dynamic_cast<EntityPropertiesDouble*>(materialData->getProperties().getProperty("Permeability (relative)"));
		EntityPropertiesDouble *conductivity = dynamic_cast<EntityPropertiesDouble*>(materialData->getProperties().getProperty("Conductivity"));

		assert(permittivity != nullptr);
		assert(permeability != nullptr);
		assert(conductivity != nullptr);

		mat.epsilon = permittivity->getValue();
		mat.mu      = permeability->getValue();
		mat.sigma   = conductivity->getValue();
	}
	else
	{
		// Backward compatibility in case that no material has been defined

		bool isBackground = false;

		if (backgroundName == meshItemName)
		{
			isBackground = true;
		}
		else if (meshItemName.size() > backgroundName.size())
		{
			std::string backgroundNameBase = backgroundName + "/";

			if (backgroundNameBase == meshItemName.substr(0, backgroundNameBase.size()))
			{
				isBackground = true;
			}
		}

		if (isBackground)
		{
			mat.epsilon = 1.0;
			mat.mu = 1.0;
			mat.sigma = 0.0;
		}
		else
		{
			mat.epsilon = 1.0;
			mat.mu = 1.0;
			mat.sigma = 5e7;
		}
	}

	return mat;
}

std::string PHREECMeshExport::exportMeshNodes(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshNodesID, unsigned long long meshNodesVersion, std::ofstream &meshFile, bool debug, std::ofstream &debugFile)
{
	std::string error;

	auto meshNodesDoc = bsoncxx::builder::basic::document{};
	error = readDocument(dataBaseURL, projectName, meshNodesID, meshNodesVersion, meshNodesDoc, "EntityMeshTetNodes", 1);
	if (!error.empty()) return error;

	auto meshNodesDocView = meshNodesDoc.view()["Found"].get_document().view();

	auto pointX = meshNodesDocView["CoordX"].get_array().value;
	auto pointY = meshNodesDocView["CoordY"].get_array().value;
	auto pointZ = meshNodesDocView["CoordZ"].get_array().value;

	size_t numberNodes = std::distance(pointX.begin(), pointX.end());
	assert(numberNodes == std::distance(pointY.begin(), pointY.end()));
	assert(numberNodes == std::distance(pointZ.begin(), pointZ.end()));

	auto px = pointX.begin();
	auto py = pointY.begin();
	auto pz = pointZ.begin();

	meshFile.write(reinterpret_cast<char *>(&numberNodes), sizeof(size_t));

	if (debug) debugFile << "<size_t> " << numberNodes << " [Number of nodes]" << std::endl;

	for (unsigned long index = 0; index < numberNodes; index++)
	{
		double nodeX = px->get_double();
		double nodeY = py->get_double();
		double nodeZ = pz->get_double();

		meshFile.write(reinterpret_cast<char *>(&nodeX), sizeof(double));
		meshFile.write(reinterpret_cast<char *>(&nodeY), sizeof(double));
		meshFile.write(reinterpret_cast<char *>(&nodeZ), sizeof(double));

		if (debug)
		{
			debugFile << "<double> " << nodeX << " [Node " << index + 1 << " x coordinate]" << std::endl;
			debugFile << "<double> " << nodeY << " [Node " << index + 1 << " y coordinate]" << std::endl;
			debugFile << "<double> " << nodeZ << " [Node " << index + 1 << " z coordinate]" << std::endl;
		}

		px++;
		py++;
		pz++;
	}

	return error;
}

std::string PHREECMeshExport::exportMeshFaces(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshFacesID, unsigned long long meshFacesVersion,
										     std::vector<std::string> &sectionNames, std::list<triangle> &triangleList)
{
	std::string error;

	auto meshFacesDoc = bsoncxx::builder::basic::document{};
	error = readDocument(dataBaseURL, projectName, meshFacesID, meshFacesVersion, meshFacesDoc, "EntityMeshTetFaceData", 1);
	if (!error.empty()) return error;

	auto meshFacesDocView = meshFacesDoc.view()["Found"].get_document().view();

	// Iterate over all faces in the mesh
	auto facesID = meshFacesDocView["MeshFacesID"].get_array().value;
	auto facesVersion = meshFacesDocView["MeshFacesVersion"].get_array().value;

	size_t numberFaces = std::distance(facesID.begin(), facesID.end());
	assert(numberFaces == std::distance(facesVersion.begin(), facesVersion.end()));

	// Prefetch all faces
	auto faceID = facesID.begin();
	auto faceVersion = facesVersion.begin();

	std::list<std::pair<ot::UID, ot::UID>> prefetchList;

	for (unsigned long index = 0; index < numberFaces; index++)
	{
		unsigned long long meshFaceID = faceID->get_int64();
		unsigned long long meshFaceVersion = faceVersion->get_int64();

		prefetchList.push_back(std::pair<ot::UID, ot::UID>(meshFaceID, meshFaceVersion));

		faceID++;
		faceVersion++;
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchList);

	// Prefetch all annotations
	faceID = facesID.begin();
	faceVersion = facesVersion.begin();

	std::map<unsigned long, unsigned long long> annotationIndexMap;
	std::map<unsigned long, bool> annotationForwardMap;
	ot::UIDList annotationIDList;

	for (unsigned long index = 0; index < numberFaces; index++)
	{
		unsigned long long meshFaceID = faceID->get_int64();
		unsigned long long meshFaceVersion = faceVersion->get_int64();

		unsigned long long annotationID{ 0 };
		bool forward{ false };
		error = getMeshFaceAnnotationID(dataBaseURL, projectName, meshFaceID, meshFaceVersion, annotationID, forward);
		if (!error.empty()) return error;

		if (annotationID != 0)
		{
			annotationIDList.push_back(annotationID);
		}

		annotationIndexMap[index] = annotationID;
		annotationForwardMap[index] = forward;

		faceID++;
		faceVersion++;
	}

	application->prefetchDocumentsFromStorage(annotationIDList);

	// Now process the face annotations and write the faces with annotations
	faceID = facesID.begin();
	faceVersion = facesVersion.begin();

	for (unsigned long index = 0; index < numberFaces; index++)
	{
		unsigned long long meshFaceID = faceID->get_int64();
		unsigned long long meshFaceVersion = faceVersion->get_int64();

		unsigned long long annotationID = annotationIndexMap[index];
		bool forward = annotationForwardMap[index];

		if (annotationID != 0)
		{
			// This face has an annotation attached -> export it to the mesh file

			// Read the name of the annotation

			unsigned long long annotationVersion = application->getPrefetchedEntityVersion(annotationID);

			std::string annotationName;
			error = readAnnotationName(dataBaseURL, projectName, annotationID, annotationVersion, annotationName);

			// Create a new section
			int sectionID = nextSectionIndex;
			nextSectionIndex++;

			sectionNames.push_back(annotationName);

			// Now read the face triangles and export them to the mesh file
			error = exportMeshFace(dataBaseURL, projectName, meshFaceID, meshFaceVersion, forward, sectionID, triangleList);
			if (!error.empty()) return error;
		}

		faceID++;
		faceVersion++;
	}

	return error;
}

std::string PHREECMeshExport::readAnnotationName(const std::string &dataBaseURL, const std::string &projectName, unsigned long long annotationID, unsigned long long annotationVersion, std::string &annotationName)
{
	// We need to find the data base entry for the face annotation with entity ID annotationID. 
	// We can then read the annotationName from this entry

	std::string error;

	auto annotationDoc = bsoncxx::builder::basic::document{};
	error = readFaceAnnotation(dataBaseURL, projectName, annotationID, annotationVersion, annotationDoc);
	if (!error.empty()) return error;

	auto annotationDocView = annotationDoc.view()["Found"].get_document().view();

	annotationName = annotationDocView["Name"].get_utf8().value.data();

	size_t index = annotationName.rfind('/');
	annotationName = annotationName.substr(index + 1);

	return error;
}

std::string PHREECMeshExport::exportMeshFace(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshFaceID, unsigned long long meshFaceVersion,
										    bool forward, int sectionID, std::list<triangle> &triangleList)
{
	std::string error;

	// Here we read a single mesh face and write its triangles into the mesh file. 
	
	auto meshFaceDoc = bsoncxx::builder::basic::document{};
	error = readDocument(dataBaseURL, projectName, meshFaceID, meshFaceVersion, meshFaceDoc, "EntityMeshTetFace", 1);
	if (!error.empty()) return error;

	auto meshFaceDocView = meshFaceDoc.view()["Found"].get_document().view();

	// Now we determine the number of annotations attached to this face
	auto faceTrianglesDoc = meshFaceDocView["Triangles"].get_document();
	auto faceTrianglesDocView = faceTrianglesDoc.view();

	try
	{
		// This is new code for reading the triangles where each triangle can have an arbitrary order
		auto nodes = faceTrianglesDocView["Nodes"].get_array().value;

		auto n = nodes.begin();

		// Read the number of triangles
		size_t numberTriangles = DataBase::GetIntFromArrayViewIterator(n);
		n++;

		for (unsigned long index = 0; index < numberTriangles; index++)
		{
			int numberNodes = (int)DataBase::GetIntFromArrayViewIterator(n);
			n++;

			if (numberNodes != 3) return "The solver currently supports first order meshes only.";

			int n1Index = (int)getIntFromIterator(n) + 1; n++;
			int n2Index = (int)getIntFromIterator(n) + 1; n++;
			int n3Index = (int)getIntFromIterator(n) + 1; n++;

			triangle tri;

			if (forward)
			{
				tri.nodes[0] = n1Index;
				tri.nodes[1] = n2Index;
				tri.nodes[2] = n3Index;
			}
			else
			{
				tri.nodes[0] = n1Index;
				tri.nodes[1] = n3Index;
				tri.nodes[2] = n2Index;
			}

			tri.sectionID = sectionID;

			triangleList.push_back(tri);
		}
	}
	catch (std::exception)
	{
		// This is for backward compatibility

		auto node1 = faceTrianglesDocView["Node1"].get_array().value;
		auto node2 = faceTrianglesDocView["Node2"].get_array().value;
		auto node3 = faceTrianglesDocView["Node3"].get_array().value;

		size_t numberTriangles = std::distance(node1.begin(), node1.end());
		assert(numberTriangles == std::distance(node2.begin(), node2.end()));
		assert(numberTriangles == std::distance(node3.begin(), node3.end()));

		auto n1 = node1.begin();
		auto n2 = node2.begin();
		auto n3 = node3.begin();

		for (size_t index = 0; index < numberTriangles; index++)
		{
			// The indices in the mesh file are 1 based whereas the database entries are zero based. 
			// Therefore we need to add 1 here.

			int n1Index = (int)getIntFromIterator(n1) + 1;
			int n2Index = (int)getIntFromIterator(n2) + 1;
			int n3Index = (int)getIntFromIterator(n3) + 1;

			triangle tri;

			if (forward)
			{
				tri.nodes[0] = n1Index;
				tri.nodes[1] = n2Index;
				tri.nodes[2] = n3Index;
			}
			else
			{
				tri.nodes[0] = n1Index;
				tri.nodes[1] = n3Index;
				tri.nodes[2] = n2Index;
			}

			tri.sectionID = sectionID;

			triangleList.push_back(tri);

			n1++;
			n2++;
			n3++;
		}
	}

	return error;
}


std::string PHREECMeshExport::getMeshFaceAnnotationID(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshFaceID, unsigned long long meshFaceVersion, unsigned long long &annotationID, bool &forward)
{
	std::string error;

	// Here we read a single mesh face. We are only interested in faces which have an annotation attached. Therefore, we first read the 
	// annotation information.

	auto meshFaceDoc = bsoncxx::builder::basic::document{};
	std::vector<std::string> columnNames;

	columnNames.push_back("FaceAnnotationIDs");

	error = readPartialDocument(dataBaseURL, projectName, meshFaceID, meshFaceVersion, columnNames, meshFaceDoc, "EntityMeshTetFace", 1);
	if (!error.empty()) return error;

	auto meshFaceDocView = meshFaceDoc.view()["Found"].get_document().view();

	// Now we determine the number of annotations attached to this face
	auto faceAnnotationDoc = meshFaceDocView["FaceAnnotationIDs"].get_document();
	auto faceAnnotationDocView = faceAnnotationDoc.view();

	std::map<unsigned long long , bool> annotationIDMap;

	auto annotations = faceAnnotationDocView["AnnotationID"].get_array().value;
	auto orientations = faceAnnotationDocView["Orientation"].get_array().value;

	auto orientation = orientations.begin();

	for (auto annotation : annotations)
	{
		annotationIDMap[(unsigned long long) getIntFromElement(annotation)] = orientation->get_bool();
		orientation++;
	}

	if (annotationIDMap.empty())
	{
		annotationID = 0;
		return "";
	}
	else if (annotationIDMap.size() > 1)
	{
		error = "Face with more than one annotation found (entity ID " + std::to_string(meshFaceID) + ")";
		return error;
	}

	annotationID = annotationIDMap.begin()->first;
	forward = annotationIDMap.begin()->second;

	return "";
}

std::string PHREECMeshExport::readDocument(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshEntityID, unsigned long long meshEntityVersion,
										  bsoncxx::builder::basic::document &doc, const std::string &expectedSchemaType, int expectedMinVersion)
{
	std::vector<std::string> columnNames; 
	
	return readPartialDocument(dataBaseURL, projectName, meshEntityID, meshEntityVersion, columnNames, doc, expectedSchemaType, expectedMinVersion);
}

std::string PHREECMeshExport::readFaceAnnotation(const std::string &dataBaseURL, const std::string &projectName, unsigned long long entityID, unsigned long long entityVersion, bsoncxx::builder::basic::document &doc)
{
	// This function searches for a document with entity ID "entityID". Afterward it checks whether the SchemaType is "EntityFaceAnnotation" and the 
	// version is at least 1. If so, the document is returned.

	if (!initializeConnection(dataBaseURL))
	{
		return "Unable to connect to database";
	}

	assert(isConnected);

	DataStorageAPI::DocumentManager docManager;

	std::vector<std::string> columnNames;
	std::map<std::string, bsoncxx::types::value> filterPairs;
	filterPairs.insert(std::pair<std::string, bsoncxx::types::value>("EntityID", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(entityID)));
	filterPairs.insert(std::pair<std::string, bsoncxx::types::value>("Version", DataStorageAPI::BsonValuesHelper::getInt64BsonValue(entityVersion)));

	DataStorageAPI::QueryBuilder queryBuilder;
	auto filterQuery = queryBuilder.GenerateFilterQuery(filterPairs);
	auto projectionQuery = queryBuilder.GenerateSelectQuery(columnNames, false);

	DataStorageAPI::DataStorageResponse res = docManager.GetDocument(projectName, filterPairs, columnNames, true);

	if (!res.getSuccess()) return "Unable to find document with entity ID: " + std::to_string(entityID) + " (schema type EntityFaceAnnotation)";

	auto results = res.getBsonResult();

	doc.append(bsoncxx::builder::basic::kvp("Found", results.value()));

	auto doc_view = doc.view()["Found"].get_document().view();

	std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

	if (entityType != "EntityFaceAnnotation")
	{
		return "Wrong schema type of document with entity ID: " + std::to_string(entityID) + " (expected schema type EntityFaceAnnotation)";
	}

	int schemaVersion = (int)DataBase::GetIntFromView(doc_view, std::string("SchemaVersion_EntityFaceAnnotation").c_str());
	if (schemaVersion < 1)
	{
		return "Too early version of document with entity ID: " + std::to_string(entityID) + " (expected version at least 1)";
	}

	return "";
}

std::string PHREECMeshExport::readPartialDocument(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshEntityID, unsigned long long meshEntityVersion, std::vector<std::string> columnNames,
												 bsoncxx::builder::basic::document &doc, const std::string &expectedSchemaType, int expectedMinVersion)
{
	if (!initializeConnection(dataBaseURL))
	{
		return "Unable to connect to database";
	}

	assert(isConnected);

	//DataStorageAPI::DocumentManager docManager;

	//if (!columnNames.empty())
	//{
	//	columnNames.push_back("_id");
	//	columnNames.push_back("InsertType");
	//	columnNames.push_back("SchemaType");
	//	columnNames.push_back("SchemaVersion_" + expectedSchemaType);
	//}

	//std::map<std::string, bsoncxx::types::value> filterPairs;
	//filterPairs.insert(std::pair<std::string, bsoncxx::types::value>("_id", DataStorageAPI::BsonValuesHelper::getOIdValue(objectID)));

	//DataStorageAPI::QueryBuilder queryBuilder;
	//auto filterQuery = queryBuilder.GenerateFilterQuery(filterPairs);
	//auto projectionQuery = queryBuilder.GenerateSelectQuery(columnNames, false);

	bool success = DataBase::GetDataBase()->GetDocumentFromEntityIDandVersion(meshEntityID, meshEntityVersion, doc);
	//DataStorageAPI::DataStorageResponse res = docManager.GetDocument(projectName, filterPairs, columnNames, true);

	if (!success) return "Unable to read document with entity ID " + std::to_string(meshEntityID) + " (schema type " + expectedSchemaType + ")";

	//auto results = res.getBsonResult();

	//doc.append(bsoncxx::builder::basic::kvp("Found", results.value()));

	if (!expectedSchemaType.empty())
	{
		auto doc_view = doc.view()["Found"].get_document().view();

		std::string entityType = doc_view["SchemaType"].get_utf8().value.data();

		if (entityType != expectedSchemaType)
		{
			return "Wrong schema type of document with entity ID " + std::to_string(meshEntityID) + " (expected schema type " + expectedSchemaType + ")";
		}

		int schemaVersion = (int)DataBase::GetIntFromView(doc_view, std::string("SchemaVersion_" + expectedSchemaType).c_str());
		if (schemaVersion < expectedMinVersion)
		{
			return "Too early version of document with entity ID " + std::to_string(meshEntityID) + " (expected version at least " + std::to_string(expectedMinVersion) + ")";
		}
	}

	return "";
}

bool PHREECMeshExport::initializeConnection(const std::string &serverURL)
{
	if (isConnected) return true; // The connection has already been established successfully

	try
	{
		DataStorageAPI::ConnectionAPI::establishConnection(serverURL,DataBase::GetDataBase()->getUserName(), DataBase::GetDataBase()->getUserPassword());

		isConnected = true;

		return true; // Everything worked well
	}
	catch (std::exception)
	{
		return false; // Connection failed
	}
}

long long PHREECMeshExport::getIntFromIterator(bsoncxx::array::view::const_iterator &it)
{
	try
	{
		if (it->type() == bsoncxx::type::k_int32) return it->get_int32();
		if (it->type() == bsoncxx::type::k_int64) return it->get_int64();

		assert(0); // Unknown type
	}
	catch (std::exception)
	{
		assert(0);  // The specified element does not exist
	}

	return 0;
}

long long PHREECMeshExport::getIntFromElement(bsoncxx::array::element &element)
{
	try
	{
		if (element.type() == bsoncxx::type::k_int32) return element.get_int32();
		if (element.type() == bsoncxx::type::k_int64) return element.get_int64();

		assert(0); // Unknown type
	}
	catch (std::exception)
	{
		assert(0);  // The specified element does not exist
	}

	return 0;
}

void PHREECMeshExport::prefetchData(bsoncxx::builder::basic::document &doc)
{
	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIds;

	// Now we read all ids from the mesh information, since we need to prefetch all info

	auto doc_view = doc.view()["Found"].get_document().view();

	auto generalID = doc_view["GeneralID"].get_array().value;
	auto generalVersion = doc_view["GeneralVersion"].get_array().value;
	auto facesID = doc_view["FacesID"].get_array().value;
	auto facesVersion = doc_view["FacesVersion"].get_array().value;
	auto volumesID = doc_view["VolumesID"].get_array().value;
	auto volumesVersion = doc_view["VolumesVersion"].get_array().value;

	size_t numberGeneral = std::distance(generalID.begin(), generalID.end());
	assert(numberGeneral == std::distance(generalVersion.begin(), generalVersion.end()));

	size_t numberFaces = std::distance(facesID.begin(), facesID.end());
	assert(numberFaces == std::distance(facesVersion.begin(), facesVersion.end()));

	size_t numberVolumes = std::distance(volumesID.begin(), volumesID.end());
	assert(numberVolumes == std::distance(volumesVersion.begin(), volumesVersion.end()));

	auto idGeneralID = generalID.begin();
	auto idGeneralVersion = generalVersion.begin();
	auto idFacesID = facesID.begin();
	auto idFacesVersion = facesVersion.begin();
	auto idVolumeID = volumesID.begin();
	auto idVolumeVersion = volumesVersion.begin();

	for (size_t index = 0; index < numberGeneral; index++)
	{
		prefetchIds.push_back(std::pair<unsigned long long, unsigned long long>(idGeneralID->get_int64(), idGeneralVersion->get_int64()));
		idGeneralID++;
		idGeneralVersion++;
	}

 	for (size_t index = 0; index < numberFaces; index++)
	{
		prefetchIds.push_back(std::pair<unsigned long long, unsigned long long>(idFacesID->get_int64(), idFacesVersion->get_int64()));
		idFacesID++;
		idFacesVersion++;
	}

	for (size_t index = 0; index < numberVolumes; index++)
	{
		prefetchIds.push_back(std::pair<unsigned long long, unsigned long long>(idVolumeID->get_int64(), idVolumeVersion->get_int64()));
		idVolumeID++;
		idVolumeVersion++;
	}

	// Now we prefetch all these documents together
	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(prefetchIds);
}
