#pragma once
#include "BlockHandler.h"
#include "EntityBlockPlot1D.h"
#include "OTCore/FolderNames.h"
#include "OTCore/GenericDataStructVector.h"

class BlockHandlerPlot1D : public BlockHandler
{
public:
	BlockHandlerPlot1D(EntityBlockPlot1D* blockEntity, const HandlerMap& handlerMap);
	virtual bool executeSpecialized() override;

private:
	std::string _xDataConnector;
	std::list<std::string> _yDataConnectors;
	std::list<std::string> _curveNames;

	std::string _resultFolder = ot::FolderNames::ResultFolder + "/";
	std::string _plotName;
	std::string _curveName = "Curve";
	const std::string _curveFolderPath = _resultFolder + "1D/Curves";
	std::string _xlabel;
	std::string _xunit; 
	std::string _ylabel;
	std::string _yunit;

	std::vector<double> transformDataToDouble(GenericDataList& data);
};
