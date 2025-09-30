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
#include "OTServiceFoundation/MenuButtonDescription.h"

// C++ header
#include <string>
#include <list>
#include <map>

//Application specific includes
#include "EntityMeshCartesianData.h"
#include "MicroServiceInterfaceFITTDSolver.h"

// Forward declaration
class EntityBase;
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

class Application : public ot::ApplicationBase {
public:
	virtual ~Application();

	static Application& instance(void);

	// ##################################################################################################################################

	// Required functions

	//! @brief Will be called whenever a action should be processed. Core actions will be processed in the base and will not be forwarded to this function (see documentation)
	//! @param _action The action that should be processed
	//! @param _doc The document containing all the information
	virtual std::string processAction(const std::string & _action, ot::JsonDocument& _doc) override;

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	// ##################################################################################################################################

	// Add your custom functions/ members here

	virtual void modelSelectionChanged(void) override;

	void addSolver(void);
	void runSolver(void);

	void addMonitor(void);
	void addPort(void);
	void addSignalType(void);

	void EnsureVisualizationModelIDKnown(void);

private:
	ot::UID					visualizationModelID = 1;

	ot::MenuButtonDescription _buttonCreateSolver;
	ot::MenuButtonDescription _buttonRunSolver;
	ot::MenuButtonDescription _buttonAddPort;
	ot::MenuButtonDescription _buttonAddMonitor;
	ot::MenuButtonDescription _buttonCreateSignal;

	Application();

	void SolverThread(std::list<std::string> solverRunList);
	void SetupSolverService(MicroServiceInterfaceFITTDSolver & newSolver);
};
