// @otlicense
// File: BlockEntityHandler.h
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
//Service Header
#include "NGSpice.h"

// Open twin header
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "EntityBlock.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/FolderNames.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTGui/GraphicsPackage.h"
#include "EntityBlockConnection.h"
#include "EntityFileText.h"

// C++ header
#include <string>
#include <memory>

class BlockEntityHandler : public BusinessLogicHandler
{
public:
	void createBlockPicker();
	std::map<ot::UID, std::shared_ptr<EntityBlock>> findAllBlockEntitiesByBlockID(const std::string& _folderName);
	std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> findAllEntityBlockConnections(const std::string& _folderName);
	std::shared_ptr<EntityFileText> getCircuitModel(const std::string& _folderName, std::string _modelName);

	// Function for resultCurves of Simulation
	void createResultCurves(std::string solverName,std::string simulationType,std::string circuitName);

	// Setter
	void setPackageName(const std::string& name) { m_packageName = name; };
	
	// Getter
	const std::string& getPackageName() const { return m_packageName; };
	const std::string getInitialCircuitName() const;
private:
	const std::string m_blockFolder = "Circuits";
	std::string m_packageName = "Circuit Simulator";
	const std::string m_initialCircuitName = "Circuit 1";

	const std::string m_connectionsFolder = "Connections";

	ot::GraphicsPickerCollectionPackage buildUpBlockPicker();
};


