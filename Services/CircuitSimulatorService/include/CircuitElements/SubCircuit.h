// @otlicense
// File: CircuitElementSubcircuit.h
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
#include "OTCore/OTClassHelper.h"

// Service header
#include "CircuitElement.h"

class CircuitElementSubcircuit : public CircuitElement
{
	OT_DECL_NOCOPY(CircuitElementSubcircuit)
public:
	CircuitElementSubcircuit() = default;
	CircuitElementSubcircuit(std::string modelName, std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName);
	~CircuitElementSubcircuit() = default;

	std::string type() const override { return "Subcircuit"; }
	std::string getNetlistPrefix() const override { return "X"; } 
	std::string getNetlistValue() const override { return m_modelName; }

	void initFromEntity(const std::shared_ptr<ot::EntityBlock>& _entity, const std::string& _editorName) override;

	// Getter & Setter
	const std::string& getModelName() const { return m_modelName; }
	void setModelName(const std::string& _name) { m_modelName = _name; }

	const std::vector<std::string>& getOrderedTerminalNames() const { return m_orderedTerminalNames; }
	void setOrderedTerminalNames(const std::vector<std::string>& _terminals) { m_orderedTerminalNames = _terminals; }

	bool allowDuplicateNodes() const override { return true; }
	std::vector<std::string> getOrderedPoleNames() const override;

private:
	std::string m_modelName;
	std::vector<std::string> m_orderedTerminalNames;
};
