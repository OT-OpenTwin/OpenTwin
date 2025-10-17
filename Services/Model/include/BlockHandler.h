#pragma once

// OpenTwin header
#include "OTGuiAPI/GraphicsHandler.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"

class BlockHandler : public BusinessLogicHandler, public ot::GraphicsHandler {
	OT_DECL_NOCOPY(BlockHandler)
	OT_DECL_NOMOVE(BlockHandler)
public:
	BlockHandler() = default;
	~BlockHandler() = default;

protected:
	virtual ot::ReturnMessage graphicsItemChanged(const ot::GraphicsItemCfg* _item) override;
	virtual ot::ReturnMessage graphicsConnectionChanged(const ot::GraphicsConnectionCfg& _connectionData) override;
};