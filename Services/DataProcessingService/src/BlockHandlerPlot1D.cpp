#pragma once
#include "BlockHandlerPlot1D.h"
#include "EntityPlot1D.h"
#include "OpenTwinCommunication/ActionTypes.h"

BlockHandlerPlot1D::BlockHandlerPlot1D(ot::components::ModelComponent* modelComponent)
    : _modelComponent(modelComponent)
{
}

BlockHandler::genericDataBlock BlockHandlerPlot1D::Execute(BlockHandler::genericDataBlock& inputData)
{

	std::vector<double> resultCurve;
	resultCurve.reserve(inputData.size());

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
	}
	/*std::string plotName = "Plots/firstplot";
	{
		EntityResult1D* curve = _modelComponent->addResult1DEntity(plotName, resultCurve, {}, {, xlabel, xunit, ylabel, yunit, colorID, visualize);
	}

	topologyEntityIDList.push_back(curve->getEntityID());
	topologyEntityVersionList.push_back(curve->getEntityStorageVersion());
	topologyEntityForceVisibleList.push_back(false);

	dataEntityIDList.push_back(curve->getCurveDataStorageId());
	dataEntityVersionList.push_back(curve->getCurveDataStorageVersion());
	dataEntityParentList.push_back(curve->getEntityID());

    _modelComponent->addPlot1DEntity()
    std::unique_ptr<EntityPlot1D> resultPlot(new EntityPlot1D(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_DataProcessingService));
    resultPlot->*/
    return genericDataBlock();
}
