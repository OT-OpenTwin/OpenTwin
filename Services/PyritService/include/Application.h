/*
 * Application.h
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

#pragma once

// Open twin header
#include "OTServiceFoundation/ApplicationBase.h"

// std header
#include <string>

class SubprocessManager;

class Application : public ot::ApplicationBase {
public:
	static Application* instance(void);
	static void deleteInstance(void);

private:
	Application();
	virtual ~Application();
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

	//! @brief Will be called when the service was successfully started, the hppt and/or websocket connection is established and the service may start its work
	virtual void run(void) override;

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	//! @brief Will be called when a UI disconnected from the session (is already closed)
	virtual void uiDisconnected(const ot::components::UiComponent * _ui) override;

	//! @brief Will be called when a model service connected to the session and is ready to work
	virtual void modelConnected(ot::components::ModelComponent * _model) override;

	//! @brief Will be called when a mode service disconnected from the session (is already closed)
	virtual void modelDisconnected(const ot::components::ModelComponent * _model) override;

	virtual void logFlagsChanged(const ot::LogFlags& _flags) override;

	// Handler
	void handleExecuteAction(ot::JsonDocument& _doc);

	virtual void modelSelectionChanged() override;

	void addSolver(void);
	void runSolver(void);

	void solverThread(std::list<ot::EntityInformation> solverInfo, std::list<ot::EntityInformation> meshInfo, std::map<std::string, EntityBase*> solverMap);
	void runSingleSolver(ot::EntityInformation& solver, std::list<ot::EntityInformation>& meshInfo, EntityBase* solverEntity);
	void deleteSingleSolverResults(EntityBase* solverEntity);
	std::string problemTypeScript(EntityBase* solverEntity);

	void addScalarResult(const std::string& resultName, char* fileData, int data_length, EntityBase* solverEntity);
	void addVectorResult(const std::string& resultName, char* fileData, int data_length, EntityBase* solverEntity);


private:
	SubprocessManager*		m_subprocessManager;
	ot::UID					visualizationModelID;
};
