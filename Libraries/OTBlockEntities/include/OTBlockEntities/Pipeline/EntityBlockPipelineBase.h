// @otlicense

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