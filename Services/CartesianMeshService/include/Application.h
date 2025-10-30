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
#include "OTCore/CoreTypes.h"					// open twins rapid json layer
#include "OTGuiAPI/ButtonHandler.h"
#include "OTServiceFoundation/ApplicationBase.h"		// Base class
#include "OldTreeIcon.h"
#include "EntityInformation.h"

// C++ header
#include <string>

class EntityBase;

// Forward declaration
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

class Application : public ot::ApplicationBase, public ot::ButtonHandler {
	OT_DECL_NOCOPY(Application)
public:
	static Application * instance(void);
	static void deleteInstance(void);

private:
	Application();
	virtual ~Application();
public:

	// ##################################################################################################################################

	// Required functions

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	virtual void modelSelectionChanged() override;

	// ##################################################################################################################################

	// Add your custom functions/ members here
	void createMesh(void);
	void updateMesh(void);

	void mesherThread(std::list<ot::EntityInformation> mesherInfo, std::map<std::string, EntityBase *> mesherMap);
	void runSingleMesher(ot::EntityInformation &mesher, EntityBase *meshEntity);

private:
	static std::string materialsFolder;
	static ot::UID materialsFolderID;
};
