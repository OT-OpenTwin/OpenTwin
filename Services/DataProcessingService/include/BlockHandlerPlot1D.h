#pragma once
#include "BlockHandler.h"
#include "OpenTwinFoundation/ModelComponent.h"

class BlockHandlerPlot1D : public BlockHandler
{
public:
	BlockHandlerPlot1D(ot::components::ModelComponent* modelComponent);
	virtual genericDataBlock Execute(genericDataBlock& inputData) override;

private:
	ot::components::ModelComponent* _modelComponent;
};
