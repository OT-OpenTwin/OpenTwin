// @otlicense
// File: EntityBlockCircuitSubCircuit.h
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
#include "OTBlockEntities/Circuit/EntityBlockCircuitElement.h"


class OT_BLOCKENTITIES_API_EXPORT EntityBlockCircuitSubcircuit : public EntityBlockCircuitElement
{
	OT_DECL_NOCOPY(EntityBlockCircuitSubcircuit)
public:
	EntityBlockCircuitSubcircuit() : EntityBlockCircuitSubcircuit(0, nullptr, nullptr, nullptr) {};
	EntityBlockCircuitSubcircuit(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);
	static std::string className() { return "EntityBlockCircuitSubcircuit"; }
	virtual std::string getClassName(void) const override { return EntityBlockCircuitSubcircuit::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void createProperties() override;
	virtual bool updateFromProperties(void) override;
	virtual std::string getTypeAbbreviation() override { return "X"; };
	virtual std::string getFolderName() override { return "Subcircuits"; };
	virtual ot::GraphicsItemCfg* createBlockCfg() override;

	// Subcircuit Name
	std::string getSubcircuitName() const;
	void setSubcircuitName(const std::string& _name);

	double getRotation() const;
	bool getFlipHorizontal() const;
	bool getFlipVertical() const;

	const std::vector<std::string>& getOrderedTerminalNames() const { return m_orderedTerminalNames; }
protected:
	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap) override;
private:
	std::vector<std::string> m_orderedTerminalNames;
};