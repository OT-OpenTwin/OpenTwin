// @otlicense
// File: EntityBlockStorage.h
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

struct __declspec(dllexport) ParameterProperties
{
	bool m_propertyConstParameter = false;
	std::string m_propertyName = "";
	std::string m_propertyUnit = "";
};

struct __declspec(dllexport) QuantityProperties
{
	std::string m_propertyType = "";
	std::string m_propertyName = "";
	std::string m_propertyUnit = "";
};

class __declspec(dllexport) EntityBlockStorage : public EntityBlock
{
public:
	EntityBlockStorage() : EntityBlockStorage(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockStorage(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);
	virtual ot::GraphicsItemCfg* createBlockCfg() override;

	static std::string className() { return "EntityBlockStorage"; }
	virtual std::string getClassName(void) const override { return EntityBlockStorage::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }
	void createProperties();
	virtual bool updateFromProperties() override;

	uint32_t getNumberOfInputs();
	static const std::string getIconName() { return "Database_store.svg"; }

	bool getCreatePlot();
	std::string getPlotName();

	const std::list<std::string> getInputNames();
	const std::string getSeriesConnectorName() { return "SeriesMetadata"; }
private:	
	uint32_t m_maxNbOfInputs = 40;

	void createParameterProperties(const std::string& _groupName);

	void createConnectors();
};
