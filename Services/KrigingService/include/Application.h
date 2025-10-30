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
#include "OTServiceFoundation/ApplicationBase.h"		// Base class
#include "EntityInformation.h"

// C++ header
#include <string>
#include <list>
#include <map>
#include <array>

#include <Kriging.h>
#include <KrigingParams.h>

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
	Application();
	virtual ~Application();

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

	virtual void modelSelectionChanged() override;

	void loadDataset(void);
	void train(void);
	void predict(void);
	void saveKrigingModel(void);
	void addParameters(void);
	void importData(void);
	std::list<std::string> getListOfSelectedKrigingSolvers(void);

	KrigingParams* getParams(void);

private:
	std::list<ot::UID>		selectedEntities;
	Kriging krig;
};
