#include "BlockHandlerPlot1D.h"
#include "EntityResult1DPlot.h"
#include "EntityResult1DCurve.h"
#include "OTCore/GenericDataStructSingle.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTCore/VariableToStringConverter.h"

BlockHandlerPlot1D::BlockHandlerPlot1D(EntityBlockPlot1D* blockEntity, const HandlerMap& handlerMap)
	:BlockHandler(blockEntity ,handlerMap)
{
	m_xlabel = blockEntity->getXLabel();
	m_ylabel = blockEntity->getYLabel();
	
	m_xunit = blockEntity->getXUnit();
	m_yunit = blockEntity->getYUnit();

	m_xDataConnector = blockEntity->getConnectorXAxis().getConnectorName();
	auto allYAxisConnectors = blockEntity->getConnectorsYAxis();
	for (const auto yAxisConnector : allYAxisConnectors)
	{
		m_yDataConnectors.push_back(yAxisConnector->getConnectorName());
	}

	m_curveNames =	blockEntity->getCurveNames();
	m_curveNames.unique();
	if (m_curveNames.size() != m_yDataConnectors.size())
	{
		throw std::exception("Plot Block detected non unique identifier amongst the curve names.");
	}

	const std::string fullName = blockEntity->getName();
	m_plotName = fullName.substr(fullName.find_last_of("/")+1, fullName.size());
}

bool BlockHandlerPlot1D::executeSpecialized()
{
	bool allSet = (_dataPerPort.find(m_xDataConnector) != _dataPerPort.end());
	for (std::string& yDataConnector : m_yDataConnectors)
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

		std::vector<double> xAxisValues = loadXAxis();
				
		int colourID(0);
		std::list<std::shared_ptr<EntityResult1DCurve>> curves;
		auto currentCurveName = m_curveNames.begin();
		for (std::string& yAxisPortName : m_yDataConnectors)
		{
			const std::string curveName = *currentCurveName;
			currentCurveName++;
			const PipelineData& pipelineData = _dataPerPort.find(yAxisPortName)->second;
			std::list<std::shared_ptr<EntityResult1DCurve>> newCurves = createCurves(pipelineData, xAxisValues, curveName, colourID);
			curves.insert(curves.end(), newCurves.begin(), newCurves.end());
		}
		storeCurves(curves);
	}

	return allSet;
}

std::vector<double> BlockHandlerPlot1D::loadXAxis()
{
	PipelineData& xAxisData = _dataPerPort[m_xDataConnector];
	PipelineDataDocumentList& pipelineData = xAxisData.m_data;
	if (pipelineData.size() == 0)
	{
		throw std::exception("X axis values are empty. Nothing to plot.");
	}
	if (m_xunit == "")
	{
		setUnit(xAxisData, m_xunit);
	}
	if (m_xlabel == "")
	{
		setLabel(xAxisData, m_xlabel);
	}

	bool isFamilyOfCurves =  isAFamilyOfCurves(pipelineData);
	if (isFamilyOfCurves)
	{
		throw std::exception("X axis values are not unambiguously set.");
	}
	return transforParameterToDouble(pipelineData);
}

std::list<std::shared_ptr<EntityResult1DCurve>> BlockHandlerPlot1D::createCurves(const PipelineData& _yAxis, const std::vector<double>& _xAxisValues, const std::string& _curveNameBase, int _colourID)
{
	const PipelineDataDocumentList& pipelineDocuments= _yAxis.m_data;
	if (pipelineDocuments.size() == 0)
	{
		throw std::exception("Y axis values are empty. Nothing to plot.");
	}
	
	std::string groupingParameter("");
	if (isAFamilyOfCurves(pipelineDocuments))
	{
		groupingParameter = getGroupingParameter(_yAxis);
	}
	
	std::list<std::shared_ptr<EntityResult1DCurve>> curves;
	std::string curveNameBase = _curveNameBase;

	if (_yAxis.m_quantity == nullptr)
	{
		assert(_yAxis.m_parameter != nullptr);
		if (_curveNameBase == "")
		{
			curveNameBase = _yAxis.m_parameter->parameterLabel;
		}
		
		std::vector<double> yValues = transforParameterToDouble(pipelineDocuments);
		const std::string fullCurveName = m_curveFolderPath + "/" + curveNameBase;

		EntityResult1DCurve* curve = _modelComponent->addResult1DCurveEntity(fullCurveName, _xAxisValues, yValues, {}, m_xlabel, m_xunit, m_ylabel, m_yunit, _colourID, true);
		_colourID++;
		curves.push_back(std::shared_ptr<EntityResult1DCurve>(curve));
	}
	else
	{
		if (curveNameBase == _curveNameBase)
		{
			curveNameBase = _yAxis.m_quantity->quantityLabel;
			if (_yAxis.m_quantityDescription->quantityValueLabel != "")
			{
				curveNameBase += "_" + _yAxis.m_quantityDescription->quantityValueLabel;
			}
		}
		
		std::map<ot::Variable, std::list<double>> yValuesByGroupValue = transformQuantityToDouble(pipelineDocuments, groupingParameter);
		
		ot::VariableToStringConverter converter;
		for (auto& yValueByGroupValue : yValuesByGroupValue)
		{
			ot::Variable groupValue = yValueByGroupValue.first;
			const std::string fullCurveName = m_curveFolderPath + "/" + curveNameBase + "_" + groupingParameter + "_" + converter(groupValue);

			std::list<double>& yValues = yValueByGroupValue.second;
			std::vector<double> yValuesVect(yValues.begin(), yValues.end());
			EntityResult1DCurve* curve = _modelComponent->addResult1DCurveEntity(fullCurveName, _xAxisValues, yValuesVect, {}, m_xlabel, m_xunit, m_ylabel, m_yunit, _colourID, true);
			_colourID++;
			curves.push_back(std::shared_ptr<EntityResult1DCurve>(curve));
		}
	}

	return curves;
}

void BlockHandlerPlot1D::storeCurves(std::list<std::shared_ptr<EntityResult1DCurve>>& _curves)
{
	ot::UIDList topoEntID, topoEntVers, dataEntID, dataEntVers, dataEntParent;
	std::list<bool> forceVis;

	const std::string plotFolder = m_resultFolder + "1D/Plots/";
	const std::string fullPlotName = plotFolder + m_plotName;

	std::list<std::pair<ot::UID, std::string>> curveIdentifiers;
	for(auto& curve : _curves)
	{
		topoEntID.push_back(curve->getEntityID());
		topoEntVers.push_back(curve->getEntityStorageVersion());

		dataEntID.push_back((ot::UID)curve->getCurveDataStorageId());
		dataEntVers.push_back((ot::UID)curve->getCurveDataStorageVersion());
		dataEntParent.push_back(curve->getEntityID());
		forceVis.push_back(false);
		const std::string fullName = curve->getName();
		curveIdentifiers.push_back(std::make_pair<>(curve->getEntityID(), fullName.substr(fullName.find_last_of("/")+1)));
	}

	EntityResult1DPlot* plotID = _modelComponent->addResult1DPlotEntity(fullPlotName, "Result Plot", curveIdentifiers);
	topoEntID.push_back(plotID->getEntityID());
	topoEntVers.push_back(plotID->getEntityStorageVersion());
	forceVis.push_back(false);
	ot::ModelServiceAPI::addEntitiesToModel(topoEntID, topoEntVers, forceVis, dataEntID, dataEntVers, dataEntParent, "Created plot");

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

std::vector<double> BlockHandlerPlot1D::transforParameterToDouble(const PipelineDataDocumentList& _documents)
{
	std::vector<double> doubleValues;
	doubleValues.reserve(_documents.size());

	//Data are parameter.
	for (auto& document : _documents)
	{
		assert(document.m_parameter.size() == 1);
		const auto& parameter = *document.m_parameter.begin();
		const ot::Variable& value = parameter.second;
		doubleValues.push_back(variableToDouble(value));
	}
	return doubleValues;
}

std::map<ot::Variable, std::list<double>> BlockHandlerPlot1D::transformQuantityToDouble(const PipelineDataDocumentList& _documents, const std::string& _group)
{
	std::map<ot::Variable,std::list<double>> doubleValuesByGroup;
	//Data are quantities

	ot::Variable groupValue;
	if (_group == "")
	{
		groupValue = ot::Variable("");
	}

	if (_documents.size() > 1)
	{

		for (auto& document : _documents)
		{
			ot::GenericDataStructSingle* entry = dynamic_cast<ot::GenericDataStructSingle*>(document.m_quantity.get());
			if (entry == nullptr)
			{
				throw std::exception("Plot 1D block detected incompatible data format in data input. Only one-dimensional data structures are supported");
			}
			const ot::Variable& value = entry->getValue();

			if (_group != "")
			{
				groupValue = document.m_parameter.find(_group)->second;
			}
			
			doubleValuesByGroup[groupValue].push_back(variableToDouble(value));
		}
	}
	else
	{
		const PipelineDataDocument& document = *_documents.begin();
		ot::GenericDataStructMatrix* matrixStruct = dynamic_cast<ot::GenericDataStructMatrix*>(document.m_quantity.get());
		if (matrixStruct != nullptr)
		{
			uint32_t numberOfRows = matrixStruct->getNumberOfRows();
			uint32_t numberOfColumns = matrixStruct->getNumberOfColumns();
			if (numberOfRows > 1 && numberOfColumns > 1)
			{
				throw std::exception("Plot 1D block detected incompatible data format in data input. Only one-dimensional data structures are supported.");
			}
			uint32_t numberOfEntries = matrixStruct->getNumberOfEntries();
			
			ot::MatrixEntryPointer matrixEntry;
			for (matrixEntry.m_row = 0; matrixEntry.m_row < numberOfRows; matrixEntry.m_row++)
			{
				for (matrixEntry.m_column = 0; matrixEntry.m_column < numberOfColumns; matrixEntry.m_column++)
				{
					const ot::Variable& value = matrixStruct->getValue(matrixEntry);
					doubleValuesByGroup[groupValue].push_back(variableToDouble(value));
				}
			}
		}
		else
		{
			ot::GenericDataStructVector* entry = dynamic_cast<ot::GenericDataStructVector*>(document.m_quantity.get());
			if (entry == nullptr)
			{
				throw std::exception("Plot 1D block detected incompatible data format in data input. Only one-dimensional data structures are supported.");
			}
			const std::vector<ot::Variable> values = entry->getValues();
			for (const ot::Variable& value : values)
			{
				doubleValuesByGroup[groupValue].push_back(variableToDouble(value));
			}
		}
	}

	return doubleValuesByGroup;
}

double BlockHandlerPlot1D::variableToDouble(const ot::Variable& value)
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

bool BlockHandlerPlot1D::isAFamilyOfCurves(const PipelineDataDocumentList& _documents)
{
	const PipelineDataDocument& document = *_documents.begin();
	return document.m_quantity != nullptr && !document.m_parameter.empty();
}

const std::string BlockHandlerPlot1D::getGroupingParameter(const PipelineData& _pipelineData)
{
	const auto& documents =	_pipelineData.m_data;
	auto parameter =  documents.begin()->m_parameter;
	const std::string parameterName = parameter.begin()->first;
	return parameterName;
}
