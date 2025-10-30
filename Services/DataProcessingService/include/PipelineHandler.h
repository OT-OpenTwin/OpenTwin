// @otlicense
// File: PipelineHandler.h
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
#include <list>

#include "EntityBlock.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "GraphNode.h"
#include "BlockHandler.h"
#include "SolverReport.h"


class PipelineHandler : public BusinessLogicHandler
{
public:
	void runAll(const std::list < std::shared_ptr<GraphNode>>& _rootNodes, const std::map<ot::UID, std::shared_ptr<GraphNode>>& _graphNodesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID);
	void setSolverName(const std::string& _solverName)
	{
		SolverReport::instance().setSolverName(_solverName);
	}

private:
	std::map<std::shared_ptr<GraphNode>, std::shared_ptr<BlockHandler>> m_blockHandlerByGraphNode;

	void initiate(const std::map<ot::UID, std::shared_ptr<GraphNode>>& _graphNodesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID);
	std::shared_ptr<BlockHandler> createBlockHandler(std::shared_ptr<EntityBlock> _blockEntity);
};
