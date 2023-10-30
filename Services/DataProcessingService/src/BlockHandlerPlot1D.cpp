#include "BlockHandlerPlot1D.h"
//#include "EntityPlot1D.h"
//#include "EntityResult1D.h"
//#include "OpenTwinCommunication/ActionTypes.h"
//#include "Application.h"
#include "GraphNode.h"

BlockHandlerPlot1D::BlockHandlerPlot1D(EntityBlockPlot1D* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(handlerMap)
{
	_xlabel = blockEntity->getXLabel();
	_ylabel = blockEntity->getYLabel();
	
	_xunit = blockEntity->getXUnit();
	_yunit = blockEntity->getYUnit();

	_xDataConnector = blockEntity->getConnectorXAxis().getConnectorName();
	_yDataConnector = blockEntity->getConnectorYAxis().getConnectorName();
}
bool BlockHandlerPlot1D::executeSpecialized()
{
	bool allSet = (_dataPerPort.find(_xDataConnector) != _dataPerPort.end()) && (_dataPerPort.find(_yDataConnector) != _dataPerPort.end());
	if (allSet)
	{

	}
	return allSet;
}

//
//BlockHandler::genericDataBlock BlockHandlerPlot1D::Execute(BlockHandler::genericDataBlock& inputData)
//{
//	if (_yValues.size() == 0)
//	{
//		std::string connectorName = _ownToOtherConnector[_yDataConnector];
//		_yValues.reserve(inputData[connectorName].size());
//		for (auto& data : inputData[connectorName])
//		{
//			if (data.isInt32())
//			{
//				_yValues.push_back(static_cast<double>(data.getInt32()));
//			}
//			else if (data.isInt64())
//			{
//				_yValues.push_back(static_cast<double>(data.getInt64()));
//			}
//			else if (data.isFloat())
//			{
//				_yValues.push_back(static_cast<double>(data.getFloat()));
//			}
//			else if (data.isDouble())
//			{
//				_yValues.push_back(data.getDouble());
//			}
//			else
//			{
//				throw std::exception("Block of type Plot1D can only handle input data of type float, double, int32 or int64");
//			}
//		}
//	}
//	else if(_xValues.size() == 0)
//	{
//		std::string connectorName = _ownToOtherConnector[_xDataConnector];
//		_xValues.reserve(inputData[connectorName].size());
//		for (auto& data : inputData[connectorName])
//		{
//			if (data.isInt32())
//			{
//				_xValues.push_back(static_cast<double>(data.getInt32()));
//			}
//			else if (data.isInt64())
//			{
//				_xValues.push_back(static_cast<double>(data.getInt64()));
//			}
//			else if (data.isFloat())
//			{
//				_xValues.push_back(static_cast<double>(data.getFloat()));
//			}
//			else if (data.isDouble())
//			{
//				_xValues.push_back(data.getDouble());
//			}
//			else
//			{
//				throw std::exception("Block of type Plot1D can only handle input data of type float, double, int32 or int64");
//			}
//		}
//	}
//	else 
//	{
//		if (!_done)
//		{
//			int colorID(0);
//
//			EntityResult1D* curve = _modelComponent->addResult1DEntity(_resultFolder + _curveName, _xValues, _yValues, {}, _xlabel, _xunit, _ylabel, _yunit, colorID, true);
//			std::list<std::pair<ot::UID, std::string>> curves{ std::pair<ot::UID, std::string>(curve->getEntityID(),_curveName) };
//			EntityPlot1D* plotID = _modelComponent->addPlot1DEntity(_resultFolder + _plotName, "SomeEntries", curves);
//
//
//			ot::UIDList topoEnt{ plotID->getEntityID() },
//				topoVers{ plotID->getEntityStorageVersion() },
//				dataEntID{ (ot::UID)curve->getCurveDataStorageId(),curve->getEntityID() }, dataEntVers{ (ot::UID)curve->getCurveDataStorageVersion(),curve->getEntityStorageVersion() },
//				dataEntParent{ plotID->getEntityID() ,plotID->getEntityID() };
//			std::list<bool> forceVis{ false,false };
//			_modelComponent->addEntitiesToModel(topoEnt, topoVers, forceVis, dataEntID, dataEntVers, dataEntParent, "Created plot");
//			_done = true;
//		}
//	}
//	
//
//    return genericDataBlock();
//}
