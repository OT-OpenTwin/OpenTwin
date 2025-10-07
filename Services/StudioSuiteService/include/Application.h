/*
 * Application.h
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

#pragma once

// Open twin header
#include "OTServiceFoundation/ApplicationBase.h"		// Base class
#include "EntityInformation.h"
#include "EntityGeometry.h"

#include "InfoFileManager.h"

// C++ header
#include <string>
#include <list>
#include <map>

// Forward declaration
class EntityBase;
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

class EntityUnits;

class Application : public ot::ApplicationBase {
public:
	Application();
	virtual ~Application();

	// ##################################################################################################################################

	// Required functions

	//! @brief Will be called whenever a action should be processed. Core actions will be processed in the base and will not be forwarded to this function (see documentation)
	//! @param _action The action that should be processed
	//! @param _doc The document containing all the information
	virtual std::string processAction(const std::string & _action, ot::JsonDocument& _doc) override;

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	// ##################################################################################################################################

	void handleExecuteModelAction(ot::JsonDocument& _document);

	void importProject(void);
	void setCSTFile(void);
	void showInformation(void);
	void commitChanges(void);
	void getChanges(void);
	void setLocalFileName(const std::string& hostName, const std::string& fileName);

	void EnsureVisualizationModelIDKnown(void);

	void changeUnits(const std::string &content);
	void changeMaterials(const std::string& content);
	void changeParameters(const std::string& content);
	void changeHistory(const std::string& content);
	void shapeInformation(const std::string &content);
	void processSingleUnit(const std::string& unitName, std::stringstream& buffer, EntityUnits* units, bool& changed);
	bool processSingleMaterial(std::stringstream& buffer, std::map<std::string, bool>& materialProcessed);
	void processParameterBuffer(std::stringstream& buffer, std::map<std::string, bool>& parameterProcessed);
	void readDoubleTriple(const std::string& line, double& a, double& b, double& c);
	void shapeTriangles(std::list<std::string>& shapeNames, std::list<std::string>& shapeTriangles, std::list<std::string>& shapeHash);
	void result1D(bool appendData, std::string &data, size_t uncompressedDataLength);
	void storeShape(const std::string& name, const std::string& triangles, const std::string& materialsFolder, ot::UID materialsFolderID);
	void createFacets(const std::string& data, std::vector<Geometry::Node>& nodes, std::list<Geometry::Triangle>& triangles, std::list<Geometry::Edge>& edges);
	void writeProjectInformation(const std::string &simpleFileName, std::list<std::pair<std::string, std::string>>& hostNamesAndFileNames);
	bool readProjectInformation(std::string &simpleFileName, std::list<std::pair<std::string, std::string>>& hostNamesAndFileNames);
	bool isProjectInitialized();
	std::string getLocalFileName(const std::string& hostName);
	void addHostNameAndFileName(const std::string& hostName, const std::string& fileName, std::list<std::pair<std::string, std::string>>& hostNamesAndFileNames);
	std::string getSimpleFileName();
	long long getCurrentModelEntityVersion(void);

private:
	void uploadNeeded(ot::JsonDocument& _doc);
	void downloadNeeded(ot::JsonDocument& _doc);
	void filesUploaded(ot::JsonDocument& _doc);

	ot::UID					visualizationModelID;

	std::map<std::string, std::tuple<double, double, double>> materialColors;
	std::map<std::string, std::string> shapeMaterials;
	InfoFileManager infoFileManager;
};
