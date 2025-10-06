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

#include "ClassFactoryCAD.h"

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

class Application : public ot::ApplicationBase {
	OT_DECL_ACTION_HANDLER(Application)
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

	OT_HANDLER(handleExecuteModelAction, Application, OT_ACTION_CMD_MODEL_ExecuteAction, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleExecuteFunction, Application, OT_ACTION_CMD_MODEL_ExecuteFunction, ot::SECURE_MESSAGE_TYPES)

	// Add your custom functions/ members here
	virtual void modelSelectionChanged() override;

	void createMesh(void);
	void updateMesh(void);
	void exportMesh(void);
	void importMesh(void);

	void mesherThread(std::list<ot::EntityInformation> mesherInfo, std::map<std::string, EntityMeshTet *> mesherMap);
	void runSingleMesher(ot::EntityInformation &mesher, EntityMeshTet *meshEntity);
	void exportMeshFile(const std::string &fileName);
	void importMeshFile(const std::string& originalName, const std::string& content, ot::UID uncompressedDataLength);
	void exportMeshThread(const std::string meshName, const std::string originalName, const std::string content, ot::UID uncompressedDataLength);

	std::string getCurrentlySelectedMeshName(void);
	ot::UID getCurrentlySelectedMeshDataID(void);

private:
	ot::UID				visualizationModelID;

	static std::string materialsFolder;
	static ot::UID materialsFolderID;

	ClassFactoryCAD classFactoryCAD;
};
