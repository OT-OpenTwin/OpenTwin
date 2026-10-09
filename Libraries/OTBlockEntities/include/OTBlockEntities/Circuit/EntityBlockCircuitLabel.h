// @otlicense
// File: EntityBlockCircuitLabel.h
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
#include "OTBlockEntities/Circuit/EntityBlockCircuitElement.h"

class OT_BLOCKENTITIES_API_EXPORT EntityBlockCircuitLabel : public EntityBlockCircuitElement
{
public:
	EntityBlockCircuitLabel() : EntityBlockCircuitLabel(0, nullptr, nullptr, nullptr) {};
	EntityBlockCircuitLabel(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);

	static std::string className() { return "EntityBlockCircuitLabel"; }
	virtual std::string getClassName(void) const override { return EntityBlockCircuitLabel::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void createProperties() override;
	virtual std::string getTypeAbbreviation() override;
	virtual std::string getFolderName() override;
	virtual std::string getBlockFolderName() const override { return "Labels"; };
	std::string getLabelName() const;

	virtual ot::GraphicsItemCfg* createBlockCfg() override;

	double getRotation() const;
	bool getFlipHorizontal() const;
	bool getFlipVertical() const;
	const ot::Connector getLeftConnector() const { return m_LeftConnector; }
	virtual bool updateFromProperties(void) override;
private:

	ot::Connector m_LeftConnector;

	void addStorageData(bsoncxx::builder::basic::document& storage) override;
	void readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
};
