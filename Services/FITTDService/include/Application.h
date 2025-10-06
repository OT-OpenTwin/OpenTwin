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
#include "OTGui/ToolBarButtonCfg.h"
#include "OTGuiAPI/ButtonHandler.h"

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

class Application : public ot::ApplicationBase, public ot::ButtonHandler {
public:
	virtual ~Application();

	static Application& instance(void);

	// ##################################################################################################################################

	// Required functions

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

	ot::ToolBarButtonCfg _buttonCreateSolver;
	ot::ToolBarButtonCfg _buttonRunSolver;
	ot::ToolBarButtonCfg _buttonAddPort;
	ot::ToolBarButtonCfg _buttonAddMonitor;
	ot::ToolBarButtonCfg _buttonCreateSignal;

	Application();

	void SolverThread(std::list<std::string> solverRunList);
	void SetupSolverService(MicroServiceInterfaceFITTDSolver & newSolver);
};
