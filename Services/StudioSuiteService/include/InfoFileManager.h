#pragma once

#include <string>
#include <map>
#include <list>

#include "OTCore/CoreTypes.h"

class Application;

class InfoFileManager
{
public:
	InfoFileManager() : application(nullptr), hasChanged(false), infoEntityID(0), infoEntityVersion(0) {};
	~InfoFileManager() {};

	void setData(Application* app);
	void readInformation();
	void getShapes(std::map<std::string, bool>& shapes);
	void setShapeHash(const std::string& name, const std::string& hash);
	void deleteShape(const std::string& name);
	void writeInformation();
	void addDeletedShapesToList(std::list<std::string>& list);
	ot::UID getInfoEntityID() { return infoEntityID; }
	ot::UID getInfoEntityVersion() { return infoEntityVersion; }
	void clearResult1D(void);


private:
	Application* application;
	bool hasChanged;
	std::map<std::string, std::string> shapeHashMap;
	std::list<std::string> deletedShapes;
	ot::UID infoEntityID;
	ot::UID infoEntityVersion;
};
