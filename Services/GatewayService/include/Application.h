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

// OpenTwin header
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/Handler/ActionHandler.h"
#include "OTServiceFoundation/ApplicationBase.h"

// std header
#include <list>
#include <string>

class Application : public ot::ApplicationBase, public ot::ActionHandler
{
public:
	static Application& instance();
	~Application();

	// ################################################################################################################################################################################################################################################################

	// Required functions

	virtual void initialize() override;

private:

	// ################################################################################################################################################################################################################################################################

	// Private: Action handler

	ot::ReturnMessage handleCheckReady(ot::JsonDocument& _document);
	ot::ReturnMessage handleRunGateway(ot::JsonDocument& _document);

	// ################################################################################################################################################################################################################################################################

	// Private: Constructor

	Application();

};
