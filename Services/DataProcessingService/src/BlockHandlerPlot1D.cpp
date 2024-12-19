#include "BlockHandlerPlot1D.h"
#include "EntityResult1DPlot.h"
#include "EntityResult1DCurve.h"
#include "GraphNode.h"
#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/GenericDataStructMatrix.h"

BlockHandlerPlot1D::BlockHandlerPlot1D(EntityBlockPlot1D* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(blockEntity ,handlerMap)
{
	_xlabel = blockEntity->getXLabel();
	_ylabel = blockEntity->getYLabel();
	
	_xunit = blockEntity->getXUnit();
	_yunit = blockEntity->getYUnit();

	_xDataConnector = blockEntity->getConnectorXAxis().getConnectorName();
	auto allYAxisConnectors = blockEntity->getConnectorsYAxis();
	for (const auto yAxisConnector : allYAxisConnectors)
	{
		_yDataConnectors.push_back(yAxisConnector->getConnectorName());
	}

	_curveNames =	blockEntity->getCurveNames();
	_curveNames.unique();
	if (_curveNames.size() != _yDataConnectors.size())
	{
		throw std::exception("Plot Block detected non unique identifier amongst the curve names.");
	}
	for (const std::string& curveName : _curveNames)
	{
		if (curveName == "")
		{
			throw std::exception("Plot Block detected an empty curve name.");
		}
	}

	const std::string fullName = blockEntity->getName();
	_plotName = fullName.substr(fullName.find_last_of("/")+1, fullName.size());
}
bool BlockHandlerPlot1D::executeSpecialized()
{
	bool allSet = (_dataPerPort.find(_xDataConnector) != _dataPerPort.end());
	for (std::string& yDataConnector : _yDataConnectors)
	{
		if (_dataPerPort.find(yDataConnector) == _dataPerPort.end())
		{
			allSet = false;
			break;
		}
	}
	
	if (allSet)
	{
		_uiComponent->displayMessage("Executing Plot 1D Block: " + _blockName);

		PipelineData& xAxisData = _dataPerPort[_xDataConnector];
		if (_xunit == "")
		{
			setUnit(xAxisData, _xunit);
		}
		if (_xlabel == "")
		{
			setLabel(xAxisData, _xlabel);
		}
		
		GenericDataList& genericXValues = xAxisData.m_data;
		if (genericXValues.size() == 0)
		{
			throw std::exception("X axis values are empty. Nothing to plot.");
		}
		std::vector<double> xValues = transformDataToDouble(genericXValues);
		const int colorID(0);

		auto currentCurveName = _curveNames.begin();
		
		std::list<std::pair<ot::UID, std::string>> curves;
		ot::UIDList topoEntID, topoEntVers, dataEntID, dataEntVers, dataEntParent;
		std::list<bool> forceVis;
		const std::string plotFolder = _resultFolder + "1D/Plots/";
		const std::string fullPlotName = plotFolder + _plotName;

		bool noUnitWasSet(_yunit == ""), noLabelWasSet(_ylabel == "");
		for (std::string& yAxisPortName : _yDataConnectors)
		{
			PipelineData& yAxisData = _dataPerPort[yAxisPortName];

			if (_ylabel == "")
			{
				setLabel(yAxisData, _ylabel);
			}
			else
			{
				if (noLabelWasSet)
				{
					//Here we deal with multiple parameter with different units and labels
				}
				else
				{
					//Here we ignore the transported labels, since a label was set explicitly
				}
			}

			if (_yunit == "")
			{
				setUnit(yAxisData, _yunit);
			}
			else
			{
				if (noUnitWasSet)
				{
					//Here we deal with multiple parameter with different units and labels
				}
				else
				{
					//Here we ignore the transported labels, since a label was set explicitly
				}
			}
			GenericDataList& genericYValues = yAxisData.m_data;
			if (genericYValues.size() == 0)
			{
				throw std::exception("Y axis values are empty. Nothing to plot.");
			}

			std::vector<double> yValues = transformDataToDouble(genericYValues);
			const std::string curveName = *currentCurveName;
			currentCurveName++;
			
			const std::string fullCurveName = _curveFolderPath + "/" + curveName;
			EntityResult1DCurve* curve = _modelComponent->addResult1DCurveEntity(fullCurveName, xValues, yValues, {}, _xlabel, _xunit, _ylabel, _yunit, colorID, true);
			
			curves.push_back(std::pair<ot::UID, std::string>(curve->getEntityID(),curveName));
			
			topoEntID.push_back(curve->getEntityID());
			topoEntVers.push_back(curve->getEntityStorageVersion());
			dataEntID.push_back((ot::UID)curve->getCurveDataStorageId());
			dataEntVers.push_back((ot::UID)curve->getCurveDataStorageId());
			dataEntParent.push_back(curve->getEntityID());
			forceVis.push_back(false);
		}

		EntityResult1DPlot* plotID = _modelComponent->addResult1DPlotEntity(fullPlotName, "Result Plot", curves);
		topoEntID.push_back(plotID->getEntityID());
		topoEntVers.push_back(plotID->getEntityStorageVersion());
		forceVis.push_back(false);
		_modelComponent->addEntitiesToModel(topoEntID, topoEntVers, forceVis, dataEntID, dataEntVers, dataEntParent, "Created plot");
	}
	return allSet;
}

void BlockHandlerPlot1D::setLabel(PipelineData& _pipelineData, std::string& _label)
{
	if (_pipelineData.m_quantityDescription != nullptr)
	{
		assert(_pipelineData.m_quantity != nullptr);
		_label = _pipelineData.m_quantity->quantityLabel + " " + _pipelineData.m_quantityDescription->quantityValueLabel;
	}
	else if(_pipelineData.m_parameter != nullptr)
	{
		_label = _pipelineData.m_parameter->parameterLabel;
	}
}

void BlockHandlerPlot1D::setUnit(PipelineData& _pipelineData, std::string& _unit)
{
	if (_pipelineData.m_quantityDescription != nullptr)
	{
		_unit = _pipelineData.m_quantityDescription->unit;
	}
	else if(_pipelineData.m_parameter != nullptr)
	{
		_unit = _pipelineData.m_parameter->unit;
	}
}

std::vector<double> BlockHandlerPlot1D::transformDataToDouble(GenericDataList& genericDataBlocks)
{
	std::vector<double> doubleValues;
	if (genericDataBlocks.size() > 1)
	{
		doubleValues.reserve(genericDataBlocks.size());
		for (auto& genericDataBlock : genericDataBlocks)
		{
			ot::GenericDataStructSingle* entry = dynamic_cast<ot::GenericDataStructSingle*>(genericDataBlock.get());
			if (entry == nullptr)
			{
				throw std::exception("Plot 1D block detected incompatible data format in data input. Only one-dimensional data structures are supported");
			}

			const ot::Variable& value = entry->getValue();
			doubleValues.push_back(VariableToDouble(value));
		}
	}
	else
	{
		ot::GenericDataStruct* genericDataStruc = genericDataBlocks.begin()->get();

		ot::GenericDataStructMatrix* matrixStruct = dynamic_cast<ot::GenericDataStructMatrix*>(genericDataStruc);
		if (matrixStruct != nullptr)
		{
			uint32_t numberOfRows = matrixStruct->getNumberOfRows();
			uint32_t numberOfColumns = matrixStruct->getNumberOfColumns();
			if (numberOfRows > 1 && numberOfColumns > 1)
			{
				throw std::exception("Plot 1D block detected incompatible data format in data input. Only one-dimensional data structures are supported.");
			}
			uint32_t numberOfEntries = matrixStruct->getNumberOfEntries();
			doubleValues.reserve(numberOfEntries);
			ot::MatrixEntryPointer matrixEntry;
			
			for (matrixEntry.m_row = 0; matrixEntry.m_row < numberOfRows; matrixEntry.m_row++)
			{
				for (matrixEntry.m_column = 0; matrixEntry.m_column < numberOfColumns; matrixEntry.m_column++)
				{
					const ot::Variable& value = matrixStruct->getValue(matrixEntry);
					doubleValues.push_back(VariableToDouble(value));
				}
			}
		}
		else
		{
			ot::GenericDataStructVector* entry = dynamic_cast<ot::GenericDataStructVector*>(genericDataStruc);
			if (entry == nullptr)
			{
				throw std::exception("Plot 1D block detected incompatible data format in data input. Only one-dimensional data structures are supported.");
			}
			doubleValues.reserve(entry->getNumberOfEntries());
			const std::vector<ot::Variable> values = entry->getValues();
			for (const ot::Variable& value : values)
			{
				doubleValues.push_back(VariableToDouble(value));
			}
		}
	}
	return doubleValues;
}

double BlockHandlerPlot1D::VariableToDouble(const ot::Variable& value)
{
	if (value.isInt32())
	{
		return (static_cast<double>(value.getInt32()));
	}
	else if (value.isInt64())
	{
		return (static_cast<double>(value.getInt64()));
	}
	else if (value.isFloat())
	{
		return (static_cast<double>(value.getFloat()));
	}
	else if (value.isDouble())
	{
		return (value.getDouble());
	}
	else
	{
		throw std::exception("Block of type Plot1D can only handle input data of type float, double, int32 or int64");
	}
}
