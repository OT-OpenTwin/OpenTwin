#pragma once
#include "BlockHandler.h"
#include "OpenTwinFoundation/ModelComponent.h"
#include "EntityBlockPlot1D.h"

class BlockHandlerPlot1D : public BlockHandler
{
public:
	BlockHandlerPlot1D(EntityBlockPlot1D* blockEntity);
	virtual genericDataBlock Execute(genericDataBlock& inputData) override;

private:
	ot::components::ModelComponent* _modelComponent;
	
	std::string _resultFolder = "Results/";
	std::string _plotName = "Plot";
	std::string _curveName = "Curve";

	std::string _xlabel;
	std::string _xunit; 
	std::string _ylabel;
	std::string _yunit;
};
