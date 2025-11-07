// @otlicense
// File: EntityBlockCircuitVoltageMeter.h
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
#include "EntityBlockCircuitElement.h"
class __declspec(dllexport) EntityBlockCircuitVoltageMeter : public EntityBlockCircuitElement
{
public:
	EntityBlockCircuitVoltageMeter() : EntityBlockCircuitVoltageMeter(0, nullptr, nullptr, nullptr) {};
	EntityBlockCircuitVoltageMeter(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);

	static std::string className() { return "EntityBlockCircuitVoltageMeter"; }
	virtual std::string getClassName(void) const override { return "EntityBlockCircuitVoltageMeter"; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void createProperties() override;
	virtual std::string getTypeAbbreviation() override;
	virtual std::string getFolderName() override;

	double getRotation();
	std::string getFlip();
	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	virtual bool updateFromProperties(void) override;
	const ot::Connector getLeftConnector() const { return m_LeftConnector; }
	const ot::Connector getRightConnector() const { return m_RightConnector; }

private:

	ot::Connector m_LeftConnector;
	ot::Connector m_RightConnector;
	//std::list<Connection> listOfConnections

	void addStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;


};