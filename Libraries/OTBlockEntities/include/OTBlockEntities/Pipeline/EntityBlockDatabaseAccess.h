// @otlicense
// File: EntityBlockDatabaseAccess.h
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
#include "OTCore/QueryDescription/ValueComparisonDescription.h"
#include "OTBlockEntities/EntityBlock.h"
#include "OTModelEntities/Properties/PropertyBundleDataLakeQuery.h"
#include "OTCore/QueryDescription/DataLakeAccessCfg.h"
class OT_BLOCKENTITIES_API_EXPORT  EntityBlockDatabaseAccess : public EntityBlock
{
public:
	EntityBlockDatabaseAccess() : EntityBlockDatabaseAccess(0, nullptr, nullptr, nullptr) {};
	EntityBlockDatabaseAccess(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);

	static std::string className() { return "EntityBlockDatabaseAccess"; }
	virtual std::string getClassName(void) const override { return EntityBlockDatabaseAccess::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }

	ot::DataLakeAccessCfg getDataLakeAccessCfg() const { return m_dataLakeAccessCfg; }

	virtual void createProperties() override;
	
	std::string getSelectedProjectName();
	
	ot::Connector getConnectorOutput() const { return m_connectorOutput; }	

	virtual bool updateFromProperties() override;

	virtual ot::GraphicsItemCfg* createBlockCfg() override;

	virtual ot::GraphicsConnectionCfg::ConnectionShape getDefaultConnectionShape() const override { return ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine; };

	bool getReproducibleOrder() const;
	std::pair<bool,uint32_t> getResultLimit() const;
	static const std::string getIconName() {return "Database_access.svg"; }

protected:
	void addStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

private:
	ot::Connector m_connectorOutput;
	PropertyBundleDataLakeQuery m_queryProperties;
	ot::DataLakeAccessCfg m_dataLakeAccessCfg;
	std::string m_propertyNameLimitNb = "Result limit";
	std::string m_propertyNameLimit = "Limit results";

	void updateBlockConfig();
};
