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

class Application : public ot::ApplicationBase {
	OT_DECL_ACTION_HANDLER(Application)
public:
	Application();
	virtual ~Application();

	// ##################################################################################################################################

	// Required functions

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;
	
	// ##################################################################################################################################

	OT_HANDLER(handleExecuteModelAction, Application, OT_ACTION_CMD_MODEL_ExecuteAction, ot::SECURE_MESSAGE_TYPES)

	virtual void modelSelectionChanged() override;

	void addSolver(void);
	void runPHREEC(void);
	void addTerminal(void);
	void EnsureVisualizationModelIDKnown(void);

	void solverThread(std::list<ot::EntityInformation> solverInfo, std::string modelVersion, std::list<ot::EntityInformation> meshInfo, std::map<std::string, EntityBase *> solverMap);
	void runSingleSolver(ot::EntityInformation &solver, std::string &modelVersion, std::list<ot::EntityInformation> &meshInfo, EntityBase *solverEntity);

private:
	ot::UID					visualizationModelID;

};
