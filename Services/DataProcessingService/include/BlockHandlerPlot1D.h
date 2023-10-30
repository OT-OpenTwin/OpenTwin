#pragma once
#include "BlockHandler.h"
#include "EntityBlockPlot1D.h"
#include "OpenTwinCore/FolderNames.h"

class BlockHandlerPlot1D : public BlockHandler
{
public:
	BlockHandlerPlot1D(EntityBlockPlot1D* blockEntity, const HandlerMap& handlerMap);
	virtual bool executeSpecialized() override;

private:
	std::string _xDataConnector;
	std::string _yDataConnector;

	std::string _resultFolder = ot::FolderNames::ResultFolder + "/";
	std::string _plotName;
	std::string _curveName = "Curve";

	std::string _xlabel;
	std::string _xunit; 
	std::string _ylabel;
	std::string _yunit;

	std::vector<double> transformDataToDouble(genericDataBlock& data);
};
