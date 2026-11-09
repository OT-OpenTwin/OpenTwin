// @otlicense
// File: EntityBlockPipelineBase.h
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
#include "OTGui/Graphics/Builder/GraphicsFlowItemBuilder.h"
#include "OTBlockEntities/EntityBlock.h"

namespace ot {

	class OT_BLOCKENTITIES_API_EXPORT EntityBlockPipelineBase : public EntityBlock
	{
		OT_DECL_DEFCOPY(EntityBlockPipelineBase)
		OT_DECL_DEFMOVE(EntityBlockPipelineBase)
	public:
		EntityBlockPipelineBase(ot::UID _id, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);
		virtual ~EntityBlockPipelineBase();

	protected:
		void addConnectors(ot::GraphicsFlowItemBuilder& _flowBlockBuilder) const;
	};
}