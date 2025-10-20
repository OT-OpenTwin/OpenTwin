/*
 * Application.h
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

#pragma once

// Open twin header
#include "OTServiceFoundation/ApplicationBase.h" // Base class
#include "OTGui/ToolBarButtonCfg.h"
// C++ header
#include <string>

#include "BlockEntityHandler.h"
#include "PipelineHandler.h"
#include "PropertyHandlerDatabaseAccessBlock.h"
#include "GraphHandler.h"
#include "InvalidUID.h"
#include "OTGuiAPI/ButtonHandler.h"
#include "OTGuiAPI/GraphicsHandler.h"

// Forward declaration
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

class Application : public ot::ApplicationBase, public ot::ActionHandler, public ot::ButtonHandler, public ot::GraphicsHandler {
public:
	static Application * instance(void);
	static void deleteInstance(void);

public:

	// ##################################################################################################################################################################################################################

	// Add your custom functions/ members here

	// A handler can be created to handle the the specified action
	// In this example the first parameter is the name of the callback function
	// The second parameter is the class name where the handler is created at
	// The third parameter is a String containing the action name
	// The last parameter are flags describing the allowed message types for this handler
	//OT_HANDLER(myHandleFunctionName, Application, "actionToHandle", ot::SECURE_MESSAGE_TYPES);

	// ##################################################################################################################################################################################################################

	// Required functions

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	//! @brief Will be called when a model service connected to the session and is ready to work
	virtual void modelConnected(ot::components::ModelComponent * _model) override;

	virtual void propertyChanged(ot::JsonDocument& _doc) override;

protected:
	virtual ot::ReturnMessage graphicsItemRequested(const std::string& _viewName, const std::string& _itemName, const ot::Point2DD& _pos) override;
	virtual ot::ReturnMessage graphicsConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData) override;


private:
	Application();
	virtual ~Application();

	ot::ToolBarButtonCfg m_buttonRunPipeline;
	ot::ToolBarButtonCfg m_buttonCreateSolver;
	ot::ToolBarButtonCfg m_buttonGraphicsScene;
	
	PipelineHandler _pipelineHandler;
	BlockEntityHandler _blockEntityHandler;
	GraphHandler _graphHandler;
	PropertyHandlerDatabaseAccessBlock m_propertyHandlerDatabaseAccessBlock;
	
	void runPipelineWorker(ot::UIDList _selectedSolverIDs);

	ot::UID m_dataProcessingFolderID = ot::getInvalidUID();

	void createPipeline();
	void createSolver();
	void runPipeline();
};
