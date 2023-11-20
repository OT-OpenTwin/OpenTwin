#pragma once

#include "OTCore/CoreTypes.h"

#include "rapidjson/document.h"

#include "Types.h"
#include "Geometry.h"

#include <list>

extern std::string globalUIserviceURL;

namespace MicroserviceAPI
{
	extern "C" _declspec(dllexport) const char *performAction(const char *json, const char *senderIP);
	extern "C" _declspec(dllexport) const char *queueAction(const char *json, const char *senderIP);
	extern "C" _declspec(dllexport) void deallocateData(const char *str);
	extern "C" _declspec(dllexport) int init(const char *localDirectoryServiceURL, const char *serviceIP, const char *databaseIP, const char *directoryIP);
	extern "C" _declspec(dllexport) const char *getServiceURL(void);

	enum RequestType {EXECUTE, QUEUE};
	bool sendHttpRequest(RequestType operation, const std::string &url, rapidjson::Document &doc, std::string &response);
	bool sendHttpRequest(RequestType operation, const std::string &url, const std::string & message, std::string &response);
	std::string getStringFromDocument(rapidjson::Document &doc, const char *attribute);
	std::string dispatchAction(rapidjson::Document &doc, const std::string &senderIP);
	std::string getReturnJSONFromUID(ot::UID uid);
	std::string getReturnJSONFromUIDList(std::list<ot::UID> list);
	std::string getReturnJSONFromStringList(std::list<std::string> list);
	std::string getReturnJSONFromString(std::string props);
	std::string getReturnJSONFromBool(bool flag);
	std::list<ot::UID> getUIDListFromDocument(rapidjson::Document &doc, const std::string &itemName);
	std::list<bool> getBooleanListFromDocument(rapidjson::Document &doc, const std::string &itemName);
	std::list<std::string> getStringListFromDocument(rapidjson::Document &doc, const std::string &itemName);
	std::string getEntityInformation(std::list<ot::UID> &entityIDList);

	void AddUIDListToJsonDoc(rapidjson::Document &doc, const std::string &itemName, const std::list<ot::UID> &list);
	void AddStringListToJsonDoc(rapidjson::Document &doc, const std::string &itemName, const std::list<std::string> &list);
	void AddUIDVectorToJsonDoc(rapidjson::Document &doc, const std::string &itemName, const std::vector<ot::UID> &vector);
	void AddDoubleArrayVectorToJsonDoc(rapidjson::Document &doc, const std::string &name, const std::vector<std::array<double, 3>> &vector);
	void AddDoubleArrayPointerToJsonDoc(rapidjson::Document &doc, const std::string &name, const double *doubleArray, int size);
	void addTreeIconsToJsonDoc(rapidjson::Document &doc, const TreeIcon &treeIcons);

	void AddEdgeToJsonDoc(rapidjson::Document &doc, const std::string &name, const std::list<Geometry::Edge> &edges);
	void AddTriangleToJsonDoc(rapidjson::Document &doc, const std::string &name, const std::list<Geometry::Triangle> &triangles);
	void AddNodeToJsonDoc(rapidjson::Document &doc, const std::string &name, const std::vector<Geometry::Node> &nodes);

	bool checkForUiConnection();
	bool registerAtUI();
	bool deregisterAtUI();

	std::vector<ot::UID> getVectorFromDocument(rapidjson::Document &doc, const std::string &itemName);
	
	rapidjson::Document BuildJsonDocFromAction(const std::string &action);
	rapidjson::Document BuildJsonDocFromString(std::string json);

	std::string getUIURL(void);

	std::string CreateTmpFileFromCompressedData(const std::string &data, ot::UID uncompressedDataLength);

	void queuedHttpRequestToUI(rapidjson::Document &doc, std::list<std::pair<ot::UID, ot::UID>> &prefetchIds);
	void enableQueuingHttpRequests(bool flag);
	void flushQueuedHttpRequests(void);

	std::string sendMessageToService(RequestType operation, const std::string &owner, rapidjson::Document &doc);
	void getSessionServices(void);
			   
	//! @brief Will shutdown this service
	//! @param _receivedAsCommand If true, the shutdown request came as a command form the session service
	void shutdown(bool _receivedAsCommand = false);
}

