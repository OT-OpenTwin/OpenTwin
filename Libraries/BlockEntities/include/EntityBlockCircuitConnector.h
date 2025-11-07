// @otlicense
// File: EntityBlockCircuitConnector.h
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
#include "EntityBlock.h"
class __declspec(dllexport) EntityBlockCircuitConnector : public EntityBlock
{
public:
	EntityBlockCircuitConnector() : EntityBlockCircuitConnector(0, nullptr, nullptr, nullptr) {};
	EntityBlockCircuitConnector(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);
	static std::string className() { return "EntityBlockCircuitConnector"; }
	virtual std::string getClassName(void) const override { return EntityBlockCircuitConnector::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	virtual ot::GraphicsConnectionCfg::ConnectionShape getDefaultConnectionShape() const override { return ot::GraphicsConnectionCfg::ConnectionShape::AutoXYLine; };
	virtual std::string getBlockFolderName() const override { return ""; };
	
private:
	ot::Connector m_LeftConnector;

	void addStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};