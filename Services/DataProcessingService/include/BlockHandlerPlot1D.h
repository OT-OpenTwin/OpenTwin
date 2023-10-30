#pragma once
#include "BlockHandler.h"
#include "EntityBlockPlot1D.h"

class BlockHandlerPlot1D : public BlockHandler
{
public:
	BlockHandlerPlot1D(EntityBlockPlot1D* blockEntity, const HandlerMap& handlerMap);
	virtual bool executeSpecialized() override;

private:
	ot::components::ModelComponent* _modelComponent;
	
	std::vector<double> _xValues;
	std::vector<double> _yValues;
	std::string _xDataConnector;
	std::string _yDataConnector;

	std::string _resultFolder = "Results/";
	std::string _plotName = "Plot";
	std::string _curveName = "Curve";

	std::string _xlabel;
	std::string _xunit; 
	std::string _ylabel;
	std::string _yunit;

	bool _done = false;
};
