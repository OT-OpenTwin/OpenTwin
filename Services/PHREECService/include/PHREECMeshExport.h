#pragma once

#include "Types.h"

#include <string>
#include <vector>
#include <list>
#include <map>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/types/value.hpp>
#include <bsoncxx/document/value.hpp>

namespace ot {
	namespace components {
		class ModelComponent;
	}
}

struct tetrahedron
{
	int nodes[4];
	int sectionID;
};

struct triangle
{
	int nodes[3];
	int sectionID;
};

struct material
{
	std::string name;
	double epsilon;
	double mu;
	double sigma;
};

class Application;
class EntityMaterial;

class PHREECMeshExport
{
public:
	PHREECMeshExport(Application *app);
	virtual ~PHREECMeshExport();

	std::string exportMeshData(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshEntityID, const std::string &outputFileBase, ot::components::ModelComponent *modelComponent);

private:
	std::string readDocument(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshEntityID, unsigned long long meshEntityVersion, bsoncxx::builder::basic::document &doc, const std::string &expectedSchemaType, int expectedMinVersion);
	std::string readPartialDocument(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshEntityID, unsigned long long meshEntityVersion, std::vector<std::string> columnNames, bsoncxx::builder::basic::document &doc, const std::string &expectedSchemaType, int expectedMinVersion);
	bool initializeConnection(const std::string &serverURL);
	std::string exportMeshNodes(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshNodesID, unsigned long long meshNodesVersion, std::ofstream &meshFile, bool debug, std::ofstream &debugFile);
	std::string exportMeshFaces(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshFacesID, unsigned long long meshFacesVersion, std::vector<std::string> &sectionNames, std::list<triangle> &triangleList);
	std::string getMeshFaceAnnotationID(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshFaceID, unsigned long long meshFaceVersion, unsigned long long &annotationID, bool &forward);
	std::string exportMeshFace(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshFaceID, unsigned long long meshFaceVersion, bool forward, int sectionID, std::list<triangle> &triangleList);
	std::string readAnnotationName(const std::string &dataBaseURL, const std::string &projectName, unsigned long long annotationID, unsigned long long annotationVersion, std::string &annotationName);
	std::string readTetList(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshItemID, std::list<tetrahedron> &tetList, std::map<ot::UID, ot::UID> &meshDataTetsID, std::map<ot::UID, ot::UID> &meshDataTetsVersion, std::map<ot::UID, int> &sectionID);
	std::string readFaceAnnotation(const std::string &dataBaseURL, const std::string &projectName, unsigned long long entityID, unsigned long long entityVersion, bsoncxx::builder::basic::document &doc);
	material createNewMaterial(const std::string &meshName, const std::string &meshItemName, EntityMaterial *materialData);
	long long getIntFromIterator(bsoncxx::array::view::const_iterator &it);
	long long getIntFromElement(bsoncxx::array::element &element);
	void prefetchData(bsoncxx::builder::basic::document &doc);
	void processMaterialData(const std::string &meshName, std::list<std::pair<std::string, std::string>> &objectMaterialList, std::vector<std::string> &tetrahedronSectionNames, ot::components::ModelComponent *modelComponent);
	std::string prefetchTetList(const std::string &dataBaseURL, const std::string &projectName, unsigned long long meshItemID, unsigned long long meshItemVersion, std::list<std::pair<ot::UID, ot::UID>> &prefetchList, std::list<std::pair<std::string, std::string>> &objectMaterialList, std::map<ot::UID, ot::UID> &meshDataTetsID, std::map<ot::UID, ot::UID> &meshDataTetsVersion, std::map<ot::UID, int> &sectionID);
	
	bool isConnected;
	std::map<std::string, bool> materialNamesUsed;
	int  nextSectionIndex;
	std::list<material> materialList;
	Application *application;
};
