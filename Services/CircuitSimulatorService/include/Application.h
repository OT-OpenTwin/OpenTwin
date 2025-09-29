/*
 * Application.h
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

#pragma once
//Service Header
#include "Circuit.h"
#include "BlockEntityHandler.h"
#include "NGSpice.h"
#include "SubprocessHandler.h"
#include "ConnectionManager.h"

// Open twin header
#include "OTServiceFoundation/ApplicationBase.h"	// Base class
#include "OTServiceFoundation/MenuButtonDescription.h"

// C++ header
#include <string>
#include <map>

// Forward declaration
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
		
	}
}

class QtWrapper;

class Application : public ot::ApplicationBase {
	OT_DECL_ACTION_HANDLER(Application)
public:
	static Application * instance(void);
	static void deleteInstance(void);
	
private:
	Application();
	virtual ~Application();
	NGSpice m_ngSpice;
	BlockEntityHandler m_blockEntityHandler;

	std::string m_serverName;
	bool m_SimulationRunning;
	std::mutex m_mutex;
	std::string m_modelVersion;
	std::list<ot::EntityInformation> m_solverInfo;
	std::map<std::string, EntityBase*> m_solverMap;
	
	
public:

	// ##################################################################################################################################################################################################################

	// Add your custom functions/ members here

	// A handler can be created to handle the the specified action
	// In this example the first parameter is the name of the callback function
	// The second parameter is the class name where the handler is created at
	// The third parameter is a String containing the action name
	// The last parameter are flags describing the allowed message types for this handler
	//OT_HANDLER(myHandleFunctionName, Application, "actionToHandle", ot::SECURE_MESSAGE_TYPES);
	OT_HANDLER(handleExecuteModelAction, Application, OT_ACTION_CMD_MODEL_ExecuteAction, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleNewGraphicsItem, Application, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleRemoveGraphicsItem, Application, OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveItem, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleNewGraphicsItemConnection, Application, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleRemoveGraphicsItemConnection, Application, OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveConnection, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleItemChanged, Application, OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemChanged, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleConnectionToConnection, Application, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnectionToConnection, ot::SECURE_MESSAGE_TYPES)

	void createNewCircuit();
	void createInitialCircuit();

	virtual void modelSelectionChanged() override;

	void addSolver();
	void runCircuitSimulation();
	void runNextSolvers();
	void runSingleSolver(ot::EntityInformation& solver, std::string& modelVersion,  EntityBase* solverEntity);
	void sendNetlistToSubService(std::list<std::string>& _netlist);

	std::string extractStringAfterDelimiter(const std::string& inputString, char delimiter, size_t occurrence);

	void finishSimulation();
	void finishFailedSimulation();

	//Getter
	BlockEntityHandler getBlockEntityHandler() { return m_blockEntityHandler; }
	NGSpice& getNGSpice() { return m_ngSpice; }
	std::string getCircuitRootName(void) { return "Circuits/"; }
	SubprocessHandler* getSubProcessHandler();

	// ##################################################################################################################################################################################################################

	// Required functions

	virtual void initialize() override;

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	//! @brief Will be called when a model service connected to the session and is ready to work
	virtual void modelConnected(ot::components::ModelComponent * _model) override;

private:
	SubprocessHandler* m_subprocessHandler = nullptr;
	QtWrapper* m_qtWrapper = nullptr;
	ot::MenuButtonDescription m_buttonRunSimulation;
	ot::MenuButtonDescription m_buttonAddCircuit;
	ot::MenuButtonDescription m_buttonAddSolver;
	
};
