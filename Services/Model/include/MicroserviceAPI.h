#pragma once

#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"

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
	bool sendHttpRequest(RequestType operation, const std::string &url, ot::JsonDocument &doc, std::string &response);
	bool sendHttpRequest(RequestType operation, const std::string &url, const std::string & message, std::string &response);
	std::string getStringFromDocument(ot::JsonDocument &doc, const char *attribute);
	std::string dispatchAction(ot::JsonDocument &doc, const std::string &senderIP);
	std::string getReturnJSONFromUID(ot::UID uid);
	std::string getReturnJSONFromUIDList(std::list<ot::UID> list);
	std::string getReturnJSONFromStringList(std::list<std::string> list);
	std::string getReturnJSONFromString(std::string props);
	std::string getReturnJSONFromBool(bool flag);
	std::list<ot::UID> getUIDListFromDocument(ot::JsonDocument &doc, const std::string &itemName);
	std::list<bool> getBooleanListFromDocument(ot::JsonDocument &doc, const std::string &itemName);
	std::list<std::string> getStringListFromDocument(ot::JsonDocument &doc, const std::string &itemName);
	std::string getEntityInformation(std::list<ot::UID> &entityIDList);

	void AddUIDListToJsonDoc(ot::JsonDocument &doc, const std::string &itemName, const std::list<ot::UID> &list);
	void AddStringListToJsonDoc(ot::JsonDocument &doc, const std::string &itemName, const std::list<std::string> &list);
	void AddUIDVectorToJsonDoc(ot::JsonDocument &doc, const std::string &itemName, const std::vector<ot::UID> &vector);
	void AddDoubleArrayVectorToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::vector<std::array<double, 3>> &vector);
	void AddDoubleArrayPointerToJsonDoc(ot::JsonDocument &doc, const std::string &name, const double *doubleArray, int size);
	void addTreeIconsToJsonDoc(ot::JsonDocument &doc, const TreeIcon &treeIcons);

	void AddEdgeToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::list<Geometry::Edge> &edges);
	void AddTriangleToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::list<Geometry::Triangle> &triangles);
	void AddNodeToJsonDoc(ot::JsonDocument &doc, const std::string &name, const std::vector<Geometry::Node> &nodes);

	bool checkForUiConnection();
	bool registerAtUI();
	bool deregisterAtUI();

	std::vector<ot::UID> getVectorFromDocument(ot::JsonDocument &doc, const std::string &itemName);
	
	ot::JsonDocument BuildJsonDocFromAction(const std::string &action);
	ot::JsonDocument BuildJsonDocFromString(std::string json);

	std::string getUIURL(void);

	std::string CreateTmpFileFromCompressedData(const std::string &data, ot::UID uncompressedDataLength);

	void queuedHttpRequestToUI(ot::JsonDocument &doc, std::list<std::pair<ot::UID, ot::UID>> &prefetchIds);
	void enableQueuingHttpRequests(bool flag);
	void flushQueuedHttpRequests(void);

	std::string sendMessageToService(RequestType operation, const std::string &owner, ot::JsonDocument &doc);
	void getSessionServices(void);
			   
	//! @brief Will shutdown this service
	//! @param _receivedAsCommand If true, the shutdown request came as a command form the session service
	void shutdown(bool _receivedAsCommand = false);
}

