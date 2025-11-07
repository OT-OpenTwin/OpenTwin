// @otlicense
// File: EntityBlockCircuitTransmissionLine.h
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
class __declspec(dllexport) EntityBlockCircuitTransmissionLine : public EntityBlockCircuitElement {
public:
	EntityBlockCircuitTransmissionLine() : EntityBlockCircuitTransmissionLine(0, nullptr, nullptr, nullptr) {};
	EntityBlockCircuitTransmissionLine(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);

	static std::string className() { return "EntityBlockCircuitTransmissionLine"; }
	virtual std::string getClassName(void) const override { return EntityBlockCircuitTransmissionLine::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void createProperties() override;
	std::string getImpedance();
	std::string getTransmissionDelay();
	virtual std::string getTypeAbbreviation() override;
	virtual std::string getFolderName() override;

	virtual ot::GraphicsItemCfg* createBlockCfg() override;


	double getRotation();
	std::string getFlip();

	const ot::Connector getLeftConnectorPos1() const { return m_LeftConnectorPos1; }
	const ot::Connector getLeftConnectorPos2() const { return m_LeftConnectorPos2; }
	const ot::Connector getRightConnectorNeg1() const { return m_RightConnectorNeg1; }
	const ot::Connector getRightConnectorNeg2() const { return m_RightConnectorNeg2; }

	virtual bool updateFromProperties(void) override;
private:

	ot::Connector m_LeftConnectorPos1;
	ot::Connector m_LeftConnectorPos2;
	ot::Connector m_RightConnectorNeg1;
	ot::Connector m_RightConnectorNeg2;
	

	void addStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;


};