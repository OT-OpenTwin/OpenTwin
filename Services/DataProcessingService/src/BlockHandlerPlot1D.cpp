#pragma once
#include "BlockHandlerPlot1D.h"
#include "EntityPlot1D.h"
#include "EntityResult1D.h"
#include "OpenTwinCommunication/ActionTypes.h"

BlockHandlerPlot1D::BlockHandlerPlot1D(ot::components::ModelComponent* modelComponent)
    : _modelComponent(modelComponent)
{
}

BlockHandler::genericDataBlock BlockHandlerPlot1D::Execute(BlockHandler::genericDataBlock& inputData)
{
	std::vector<double> xValues;
	xValues.reserve(inputData.size());

	std::vector<double> resultCurve;
	resultCurve.reserve(inputData.size());
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
		count = +1;
	}
	
	std::string xlabel("X-Label"), xunit("X-Unit"), ylabel("Y-Label"), yunit("Y-Unit");
	int colorID(0);
	std::string plotName = "Plots/firstPlot";
	
	EntityResult1D* curve = _modelComponent->addResult1DEntity(plotName, xValues, resultCurve, {}, xlabel, xunit, ylabel, yunit, colorID, true);
	curve->StoreToDataBase();

	std::list<std::pair<ot::UID, std::string>> curves{ std::pair<ot::UID, std::string>(curve->getEntityID(),curve->getName())};
	EntityPlot1D* plotID = _modelComponent->addPlot1DEntity("Plots/firstPlot1", "SomeEntries", curves);
	plotID->StoreToDataBase();


	ot::UIDList topoEnt, topoVers, dataEnt;
	std::list<bool> forceVis{ false,false };

	_modelComponent->addEntitiesToModel(topoEnt,topoVers,forceVis,dataEnt,dataEnt,dataEnt,"Created plot");


    return genericDataBlock();
}
