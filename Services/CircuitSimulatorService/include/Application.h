// @otlicense

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
#include "OTGuiAPI/ButtonHandler.h"
#include "OTGuiAPI/GraphicsActionHandler.h"
#include "OTServiceFoundation/ApplicationBase.h"	// Base class

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

class Application : public ot::ApplicationBase, public ot::ButtonHandler, public ot::GraphicsActionHandler {
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

	// ##################################################################################################################################################################################################################

	// Protected: Callback functions

protected:
	virtual ot::ReturnMessage graphicsItemRequested(const std::string& _viewName, const std::string& _itemName, const ot::Point2DD& _pos) override;
	virtual ot::ReturnMessage graphicsConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData) override;
	virtual ot::ReturnMessage graphicsConnectionToConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData, const ot::Point2DD& _pos) override;

private:
	SubprocessHandler* m_subprocessHandler = nullptr;
	QtWrapper* m_qtWrapper = nullptr;
	ot::ToolBarButtonCfg m_buttonRunSimulation;
	ot::ToolBarButtonCfg m_buttonAddCircuit;
	ot::ToolBarButtonCfg m_buttonAddSolver;
	
};
