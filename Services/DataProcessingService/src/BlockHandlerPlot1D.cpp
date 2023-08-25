#pragma once
#include "BlockHandlerPlot1D.h"
#include "EntityPlot1D.h"
#include "EntityResult1D.h"
#include "OpenTwinCommunication/ActionTypes.h"

BlockHandlerPlot1D::BlockHandlerPlot1D(EntityBlockPlot1D* blockEntity)
{
	_xlabel = blockEntity->getXLabel();
	_ylabel = blockEntity->getYLabel();
	
	_xunit = blockEntity->getXUnit();
	_yunit = blockEntity->getYUnit();

}

BlockHandler::genericDataBlock BlockHandlerPlot1D::Execute(BlockHandler::genericDataBlock& inputData)
{

	std::vector<double> resultCurve;
	resultCurve.reserve(inputData.size());
	
	std::vector<double> xValues;
	xValues.reserve(inputData.size());
	double count(0.);
	
	for (auto& data : inputData)
	{
		if (std::holds_alternative<int32_t>(data))
		{
			resultCurve.push_back(static_cast<double>(std::get<int32_t>(data)));
		}
		else if (std::holds_alternative<int64_t>(data))
		{
			resultCurve.push_back(static_cast<double>(std::get<int64_t>(data)));
		}
		else if (std::holds_alternative<float>(data))
		{
			resultCurve.push_back(static_cast<double>(std::get<float>(data)));
		}
		else if (std::holds_alternative<double>(data))
		{
			resultCurve.push_back(std::get<double>(data));
		}
		else
		{
			throw std::exception("Block of type Plot1D can only handle input data of type float, double, int32 or int64");
		}

		xValues.push_back(count);
		count += 1;
	}
	 
	
	int colorID(0);
	
	EntityResult1D* curve = _modelComponent->addResult1DEntity(_resultFolder + _curveName, xValues, resultCurve, {}, _xlabel, _xunit, _ylabel, _yunit, colorID, true);
	std::list<std::pair<ot::UID, std::string>> curves{ std::pair<ot::UID, std::string>(curve->getEntityID(),_curveName)};
	EntityPlot1D* plotID = _modelComponent->addPlot1DEntity(_resultFolder + _plotName, "SomeEntries", curves);


	ot::UIDList topoEnt{curve->getEntityID(), plotID->getEntityID()}, 
		topoVers{curve->getEntityStorageVersion(), plotID->getEntityStorageVersion()},
		dataEntID{ (ot::UID)curve->getCurveDataStorageId() }, dataEntVers{ (ot::UID)curve->getCurveDataStorageVersion()},
		dataEntParent{curve->getEntityID()};
	std::list<bool> forceVis{ false,false };
	_modelComponent->addEntitiesToModel(topoEnt,topoVers,forceVis,dataEntID,dataEntVers,dataEntParent,"Created plot");


    return genericDataBlock();
}
