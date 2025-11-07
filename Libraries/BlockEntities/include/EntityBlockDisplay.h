// @otlicense
// File: EntityBlockDisplay.h
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

class __declspec(dllexport) EntityBlockDisplay : public EntityBlock
{
public:
	EntityBlockDisplay() : EntityBlockDisplay(0, nullptr, nullptr, nullptr, "") {};
	EntityBlockDisplay(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner);

	static std::string className() { return "EntityBlockDisplay"; }
	virtual std::string getClassName(void) const override { return EntityBlockDisplay::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; }
		
	virtual void createProperties() override;
	const std::string& getDescription();
	virtual ot::GraphicsItemCfg* createBlockCfg() override;
	virtual ot::GraphicsConnectionCfg::ConnectionShape getDefaultConnectionShape() const override { return ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine; };
	const ot::Connector& getConnectorInput() const { return _inputConnector; }

	static const std::string getIconName() { return "Monitor.svg"; }

private:
	ot::Connector _inputConnector;
};
