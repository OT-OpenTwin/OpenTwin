// @otlicense
// File: BlockHandler.h
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
#include "OTGuiAPI/GraphicsActionHandler.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"

class BlockHandler : public BusinessLogicHandler, public ot::GraphicsActionHandler {
	OT_DECL_NOCOPY(BlockHandler)
	OT_DECL_NOMOVE(BlockHandler)
public:
	BlockHandler() = default;
	~BlockHandler() = default;
	
	bool addViewBlockRelation(std::string _viewName, ot::UID _blockId, ot::UID _connectionId);

protected:
	virtual ot::ReturnMessage graphicsItemChanged(const ot::GraphicsItemCfg* _item) override;
	virtual ot::ReturnMessage graphicsConnectionChanged(const ot::GraphicsConnectionCfg& _connectionData) override;
	virtual ot::ReturnMessage graphicsItemRequested(const std::string& _viewName, const std::string& _itemName, const ot::Point2DD& _pos) override;
	virtual ot::ReturnMessage graphicsConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData) override;
	
private:
	std::unordered_map<std::string, std::unordered_map<ot::UID, ot::UID>> m_viewBlockConnectionMap;
	const std::string m_connectionsFolder = "Connections";
};