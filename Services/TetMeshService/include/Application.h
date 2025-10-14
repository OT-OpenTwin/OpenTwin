/*
* Application.h
*
*  Created on:
*	Author:
*  Copyright (c)
*/

#pragma once

// Open twin header
#include "OTGuiAPI/ButtonHandler.h"
#include "OTServiceFoundation/ApplicationBase.h"		// Base class
#include "EntityInformation.h"

#include "OldTreeIcon.h"

// C++ header
#include <string>

class EntityBase;
class EntityMeshTet;

// Forward declaration
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

class Application : public ot::ApplicationBase, public ot::ActionHandler, public ot::ButtonHandler {
public:
	static Application * instance(void);
	static void deleteInstance(void);

private:
	Application();
	virtual ~Application();
public:

	// ##################################################################################################################################

	// Required functions

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	// ##################################################################################################################################

	// Add your custom functions/ members here
	virtual void modelSelectionChanged() override;

	void createMesh(void);
	void updateMesh(void);
	void exportMesh(void);
	void importMesh(void);

	void handleImportMesh(ot::JsonDocument& _doc);
	void handleExportMesh(ot::JsonDocument& _doc);

	void mesherThread(std::list<ot::EntityInformation> mesherInfo, std::map<std::string, EntityMeshTet *> mesherMap);
	void runSingleMesher(ot::EntityInformation &mesher, EntityMeshTet *meshEntity);
	void exportMeshFile(const std::string &fileName);
	void importMeshFile(const std::string& originalName, const std::string& content, ot::UID uncompressedDataLength);
	void exportMeshThread(const std::string meshName, const std::string originalName, const std::string content, ot::UID uncompressedDataLength);

	std::string getCurrentlySelectedMeshName(void);
	ot::UID getCurrentlySelectedMeshDataID(void);

private:
	static std::string materialsFolder;
	static ot::UID materialsFolderID;
};
