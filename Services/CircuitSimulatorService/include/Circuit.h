// @otlicense
// File: Circuit.h
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

// Service Header
#include "CircuitElement.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTBlockEntities/EntityBlock.h"

// std Header
#include <map>
#include <memory>

class Circuit
{
	OT_DECL_NOCOPY(Circuit)
public:
	// Move constructor & Move assignment operator
	Circuit(Circuit&& other) noexcept = default;
	Circuit& operator=(Circuit&& other) noexcept = default;

	Circuit();
	~Circuit();

	//Getter
	std::string getEditorName();
	std::string getId();
	std::map<ot::UID, std::unique_ptr<CircuitElement>>& getMapOfElements();
	std::map <std::string, std::vector<std::shared_ptr<ot::EntityBlock>>>& getMapOfEntityBlcks();

	//Setter
	void addElement(ot::UID key, std::unique_ptr<CircuitElement> obj);
	void addBlockEntity(std::string block, const std::shared_ptr<ot::EntityBlock> obj);
	void setEditorName(std::string name);
	void setId(const std::string id);
	
	//additionalFunctions
	void addConnection(std::string connactable,const ot::UID& key, const Connection& obj);
	std::string findElement(const ot::UID& key);
		
private:
	std::map <ot::UID, std::unique_ptr<CircuitElement>> mapOfElements;
	std::map <std::string, std::vector<std::shared_ptr<ot::EntityBlock>>> mapOfEntityBlocks;
	std::string editorName;
	std::string id;
};