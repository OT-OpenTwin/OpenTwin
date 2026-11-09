// @otlicense
// File: EntityBlockPipelineBase.cpp
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

// OpenTwin header
#include "OTBlockEntities/Pipeline/EntityBlockPipelineBase.h"

ot::EntityBlockPipelineBase::EntityBlockPipelineBase(ot::UID _id, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBlock(_id, _parent, _obs, _ms)
{

}

ot::EntityBlockPipelineBase::~EntityBlockPipelineBase()
{
}

void ot::EntityBlockPipelineBase::addConnectors(ot::GraphicsFlowItemBuilder& _flowBlockBuilder) const
{
	for (const auto& connectorByName : this->getAllConnectors())
	{
		const ot::Connector& connector = connectorByName.second;
		ot::ConnectorType connectorType = connector.getConnectorType();
		if (connectorType == ot::ConnectorType::UNKNOWN) {
			OT_LOG_E("Unknown connector type");
		}
		const std::string connectorName = connector.getConnectorName();
		const std::string connectorTitle = connector.getConnectorTitle();
		if (connectorType == ot::ConnectorType::In)
		{
			_flowBlockBuilder.addLeft(connectorName, connectorTitle, ot::GraphicsFlowItemConnector::TriangleRight);
		}
		else if (connectorType == ot::ConnectorType::InOptional)
		{
			_flowBlockBuilder.addLeft(connectorName, connectorTitle, ot::GraphicsFlowItemConnector::Circle);
		}
		else if (connectorType == ot::ConnectorType::Out)
		{
			_flowBlockBuilder.addRight(connectorName, connectorTitle, ot::GraphicsFlowItemConnector::TriangleRight);
		}
	}
}
