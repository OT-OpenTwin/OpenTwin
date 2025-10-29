// @otlicense

#pragma once

#include <string>
#include <list>
#include <map>

#include "OldTreeIcon.h"

#include "XCAFDoc_ColorTool.hxx"
#include "XCAFDoc_ShapeTool.hxx"

class EntityBase;
class EntityGeometry;
class Application;

class STEPCAFControl_Reader;
class TopoDS_Shape;
class TDF_Label;
class Model;

class STEPReader
{
public:
	STEPReader(Application *_app, const std::string &_serviceName) : application(_app), serviceName(_serviceName) { };
	virtual ~STEPReader() { };

	void importSTEPFile(const std::string &fileName, bool removeFile, const std::string &originalName);

private:
	void importSTEPFileWorker(const std::string &fileName, bool removeFile, const std::string &originalName);
	void readStepFile(std::string fileName, const std::string &rootName, bool removeFile, std::string &messages);
	std::string getNameFromLabel(const TDF_Label &label);
	std::string getEntityName(const std::string &prefix, const TDF_Label &label, const TopoDS_Shape &theShape, STEPCAFControl_Reader *aReader);
	Quantity_Color readColor(const TDF_Label &label, const TopoDS_Shape &shape);
	void processNode(const TDF_Label &itemLabel, std::string prefix, STEPCAFControl_Reader *reader, TopLoc_Location aLocation, const std::string &rootName, std::map<std::string, bool> &existingEntityNames, std::string &messages, int &shapesIndex, int numberOfShapes, bool buildShapes);
	void splitCompoundShapes(TopoDS_Shape &itemShape, std::list<TopoDS_Shape> &shapeList);
	void analyzeGeometry(EntityGeometry *entityGeom, std::string &messages);
	void simplifyComplexSplines(EntityGeometry *entityGeom, double maxRatioForComplexSplines);
	std::string determineUniqueRootName(const std::string &fileName);

	std::list<EntityGeometry *> entityList;

	Handle(XCAFDoc_ColorTool) colorTool;
	Handle(XCAFDoc_ShapeTool) shapeTool;

	Application *application;
	std::string serviceName;
	static std::string materialsFolder;
	static ot::UID materialsFolderID;
};

