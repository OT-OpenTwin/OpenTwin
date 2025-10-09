/*
 * Application.h
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

#pragma once

// Open twin header
#include "OTCore/CoreTypes.h"					// open twins rapid json layer
#include "OTGuiAPI/ButtonHandler.h"
#include "OTServiceFoundation/ApplicationBase.h"		// Base class
#include "OldTreeIcon.h"
#include "EntityInformation.h"

#include "ClassFactoryCAD.h"

// C++ header
#include <string>

class EntityBase;

// Forward declaration
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

class Application : public ot::ApplicationBase, public ot::ButtonHandler {
	OT_DECL_NOCOPY(Application)
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

	virtual void modelSelectionChanged() override;

	// ##################################################################################################################################

	// Add your custom functions/ members here
	void createMesh(void);
	void updateMesh(void);

	void mesherThread(std::list<ot::EntityInformation> mesherInfo, std::map<std::string, EntityBase *> mesherMap);
	void runSingleMesher(ot::EntityInformation &mesher, EntityBase *meshEntity);

private:
	static std::string materialsFolder;
	static ot::UID materialsFolderID;

	ClassFactoryCAD classFactoryCAD;
};
