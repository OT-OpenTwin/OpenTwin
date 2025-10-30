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

// Forward declaration
class EntityBase;
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

class EntityVis2D3D;

class Application : public ot::ApplicationBase {
public:
	Application();
	virtual ~Application();

	// ##################################################################################################################################

	// Required functions

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	// ##################################################################################################################################

	// Add your custom functions/ members here

	void propertyChanged(ot::JsonDocument& _doc) override;
private:
	void updateEntities(std::list<ot::UID> &entityIDs, std::list<ot::UID> &entityVersions, bool itemsVisible);
	void updateSingleEntity(ot::UID entityID, ot::UID entityVersion, bool itemsVisible);
	std::pair<ot::UID, ot::UID> storeBinaryData(const char *data, size_t dataLength);
	void sendNewVisualizationDataToModeler(EntityVis2D3D *visEntity, ot::UID binaryDataItemID, ot::UID binaryDataItemVersion);
	std::pair<ot::UID, ot::UID> createDataItems(EntityVis2D3D *visEntity);
};
