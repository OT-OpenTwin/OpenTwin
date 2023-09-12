#pragma once
#include "BlockHandlerPlot1D.h"
#include "EntityPlot1D.h"
#include "EntityResult1D.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "Application.h"

BlockHandlerPlot1D::BlockHandlerPlot1D(EntityBlockPlot1D* blockEntity)
{
	_xlabel = blockEntity->getXLabel();
	_ylabel = blockEntity->getYLabel();
	
	_xunit = blockEntity->getXUnit();
	_yunit = blockEntity->getYUnit();
	_modelComponent = Application::instance()->modelComponent();
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
		if (data.isInt32())
		{
			resultCurve.push_back(static_cast<double>(data.getInt32()));
		}
		else if (data.isInt64())
		{
			resultCurve.push_back(static_cast<double>(data.getInt64()));
		}
		else if (data.isFloat())
		{
			resultCurve.push_back(static_cast<double>(data.getFloat()));
		}
		else if (data.isDouble())
		{
			resultCurve.push_back(data.getDouble());
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


	ot::UIDList topoEnt{plotID->getEntityID()}, 
		topoVers{plotID->getEntityStorageVersion()},
		dataEntID{ (ot::UID)curve->getCurveDataStorageId(),curve->getEntityID() }, dataEntVers{ (ot::UID)curve->getCurveDataStorageVersion(),curve->getEntityStorageVersion() },
		dataEntParent{ plotID->getEntityID() ,plotID->getEntityID() };
	std::list<bool> forceVis{ false,false };
	_modelComponent->addEntitiesToModel(topoEnt,topoVers,forceVis,dataEntID,dataEntVers,dataEntParent,"Created plot");


    return genericDataBlock();
}
