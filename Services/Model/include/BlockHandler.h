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

protected:
	virtual ot::ReturnMessage graphicsItemChanged(const ot::GraphicsItemCfg* _item) override;
	virtual ot::ReturnMessage graphicsConnectionChanged(const ot::GraphicsConnectionCfg& _connectionData) override;
};