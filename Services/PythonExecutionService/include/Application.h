// @otlicense
// File: Application.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// Open twin header
#include "OTServiceFoundation/ApplicationBase.h"

// std header
#include <string>

class SubprocessManager;

class Application : public ot::ApplicationBase, public ot::ActionHandler {
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

private:
	std::string handleForwardToSubprocess(ot::JsonDocument& _doc);

	SubprocessManager* m_subprocessManager;
};
