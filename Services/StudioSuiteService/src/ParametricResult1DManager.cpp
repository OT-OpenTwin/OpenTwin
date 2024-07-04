#include "ParametricResult1DManager.h"
#include "Result1DManager.h"
#include "Application.h"
#include "DataBase.h"

#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTCore/FolderNames.h"

#include "ResultCollectionExtender.h"
#include "MetadataSeries.h"
#include "MetadataParameter.h"
#include "MetadataEntrySingle.h"

#include <boost/algorithm/string.hpp>

ParametricResult1DManager::ParametricResult1DManager(Application *app) :
	m_resultFolderName(ot::FolderNames::DatasetFolder),
	m_application(app)
{
	setModelComponent(m_application->modelComponent());
	setUIComponent(m_application->uiComponent());
}

ParametricResult1DManager::~ParametricResult1DManager()
{

}

void ParametricResult1DManager::clear()
{
	// We delete all previous result data (series), since there was a non-parametric change
	std::list<std::string> resultEntity{ m_resultFolderName + "/1D Results" };

	m_application->modelComponent()->deleteEntitiesFromModel(resultEntity, false);
}

void ParametricResult1DManager::add(Result1DManager& result1DManager)
{
	// We add the data in result1DManager to the parametric storage
	std::list<int> runIDList = result1DManager.getRunIDList();
	if (runIDList.empty()) return;

	std::string runIDLabel = determineRunIDLabel(runIDList);

	std::string collectionName = DataBase::GetDataBase()->getProjectName();

	ResultCollectionExtender resultCollectionExtender(collectionName, *m_application->modelComponent(), &m_application->getClassFactory(), OT_INFO_SERVICE_TYPE_STUDIOSUITE);
	resultCollectionExtender.setSaveModel(false);


	ot::UID seriesID = m_application->modelComponent()->createEntityUID();
	std::string seriesName = CreateNewUniqueTopologyName(m_resultFolderName, m_seriesNameBase);
	MetadataSeries seriesMetadata(seriesName, seriesID);
	
	//First, we assemble all new metadata in a new series metadata entity
	
	// Now we process the different types of data entries
	const std::vector<std::string> categories{ "1D Results/Balance", "1D Results/Energy", "1D Results/Port signals", "1D Results/Power","1D Results/Reference Impedance" };
	
	std::list<DataDescription1D> allDataDescriptions;

	for (int runID : runIDList)
	{
		RunIDContainer* runIDContainer = result1DManager.getContainer(runID);
		assert(runIDContainer != nullptr);

		for (const std::string& category : categories)
		{
			std::list<DataDescription1D>	curveDescriptions = extractDataDescriptionCurve(category, runIDContainer,runID);
			allDataDescriptions.splice(allDataDescriptions.end(), std::move(curveDescriptions));
		}
		std::list<DataDescription1D> sparameterDescriptions = extractDataDescriptionSParameter("1D Results/S-Parameters", runIDContainer,runID);
		allDataDescriptions.splice(allDataDescriptions.end(), std::move(sparameterDescriptions));
	}

	//Sort out the common parameter and Quanity definitions	
	std::map<std::string, MetadataQuantity> quantitiesByName;
	std::map<std::string, MetadataParameter> parametersByName;
	
	extractMetadataFromDescriptions(quantitiesByName, parametersByName, allDataDescriptions);
	for (auto& quantityByName : quantitiesByName)
	{
		seriesMetadata.AddQuantity(std::move(quantityByName.second));
	}
	
	for (auto& parameterByName : parametersByName)
	{
		seriesMetadata.AddParameter(std::move(parameterByName.second));
	}

	resultCollectionExtender.AddSeries(std::move(seriesMetadata));
	for (DataDescription1D& dataDescription : allDataDescriptions)
	{
		processDataPoints(dataDescription, resultCollectionExtender,seriesName);
	}
}

std::string ParametricResult1DManager::determineRunIDLabel(std::list<int> &runIDList)
{
	assert(!runIDList.empty());

	int minRunID = runIDList.front();
	int maxRunID = runIDList.back();

	if (minRunID == maxRunID)
	{
		return "Run " + std::to_string(minRunID);
	}

	return "Run " + std::to_string(minRunID) + "-" + std::to_string(maxRunID);
}

std::list<DataDescription1D>  ParametricResult1DManager::extractDataDescriptionCurve(const std::string& _category, RunIDContainer* _runIDContainer, int _runID)
{
	std::list<DataDescription1D> allCurveDescriptions;
	std::map<std::string, Result1DData*> categoryResults = _runIDContainer->getResultsForCategory(_category);
	if (!categoryResults.empty())
	{
	
		//We determine the parameter, which are shared by all curves.
		auto allParameter = extractParameterDescription(_category, _runIDContainer, _runID);

		//Now we create the individual curve descriptions
		for (auto& curve : categoryResults)
		{
			DataDescription1D newCurveDescription;
			auto parameter = allParameter.begin();
			newCurveDescription.setXAxisParameterDescription(*parameter);
			parameter++;
			newCurveDescription.addParameterDescriptions(parameter, allParameter.end());

			Result1DData* curveData = curve.second;
			std::unique_ptr<QuantityDescriptionCurve> quantityDescription(std::make_unique<QuantityDescriptionCurve>());
			std::string prefix = curve.first.substr(_category.size() + 1);
			const bool hasRealValues = !curveData->getYreValues().empty();
			if (hasRealValues)
			{
				quantityDescription->quantityName = prefix + " (Re)";
				quantityDescription->quantityValues.reserve(curveData->getYreValues().size());
				for (auto realValue : curveData->getYreValues())
				{
					quantityDescription->quantityValues.push_back(ot::Variable(realValue));
				}
				newCurveDescription.setQuantityDescription(quantityDescription.release());
				allCurveDescriptions.push_back(std::move(newCurveDescription));
			}
			const bool hasImValue = !curveData->getYimValues().empty();
			if (hasImValue)
			{
				std::unique_ptr<QuantityDescriptionCurve> quantityDescription(std::make_unique<QuantityDescriptionCurve>());
				quantityDescription->quantityName = prefix + " (Im)";
				quantityDescription->quantityValues.reserve(curveData->getYimValues().size());
				for (auto imValue : curveData->getYimValues())
				{
					quantityDescription->quantityValues.push_back(ot::Variable(imValue));
				}

				//In case that we already created a curve description for the real component
				if (hasRealValues)
				{
					DataDescription1D secondCurveDescription;
					auto parameter = allParameter.begin();
					secondCurveDescription.setXAxisParameterDescription(*parameter);
					parameter++;
					secondCurveDescription.addParameterDescriptions(parameter, allParameter.end());
					
					secondCurveDescription.setQuantityDescription(quantityDescription.release());
					allCurveDescriptions.push_back(std::move(secondCurveDescription));
				}
				else
				{
					newCurveDescription.setQuantityDescription(quantityDescription.release());
					allCurveDescriptions.push_back(std::move(newCurveDescription));
				}
			}
		}
	}
	return allCurveDescriptions;
}

std::list<DataDescription1D> ParametricResult1DManager::extractDataDescriptionSParameter(const std::string& _category, RunIDContainer* _runIDContainer, int _runID)
{
	std::list<DataDescription1D> newSParameterDescriptions;

	std::map<std::string, Result1DData*> categoryResults = _runIDContainer->getResultsForCategory(_category);
	if (!categoryResults.empty())
	{
		//We determine the characteristics that are shared by all curves.
		auto allParameter = extractParameterDescription(_category, _runIDContainer, _runID);
		DataDescription1D realValuesDescription, imValuesDescription;

		auto parameter = allParameter.begin();
		realValuesDescription.setXAxisParameterDescription(*parameter);
		imValuesDescription.setXAxisParameterDescription(*parameter);

		parameter++;
		realValuesDescription.addParameterDescriptions(parameter, allParameter.end());
		imValuesDescription.addParameterDescriptions(parameter, allParameter.end());

		//Now we set the s-parameter matrices
		std::vector<Result1DData*> sParameterValues;
		std::unique_ptr<QuantityDescriptionSParameter> realQuantityValues(std::make_unique<QuantityDescriptionSParameter>()), imQuantityValues(std::make_unique<QuantityDescriptionSParameter>());
		int numberOfPorts =	determineNumberOfPorts(_category, categoryResults, sParameterValues);
		realQuantityValues->quantityDataNumberOfColumns= static_cast<uint32_t>(numberOfPorts);
		realQuantityValues->quantityDataNumberOfRows= static_cast<uint32_t>(numberOfPorts);
		
		//Dangerous?
		size_t numberOfFrequencyPoints = sParameterValues[0]->getYreValues().size();
		realQuantityValues->quantityValues.reserve(numberOfFrequencyPoints);
		for (size_t i = 0; i < numberOfFrequencyPoints; i++)
		{
			realQuantityValues->quantityValues.push_back(SParameterMatrixHelper(realQuantityValues->quantityDataNumberOfRows));
		}
		
		*imQuantityValues = *realQuantityValues;
		realQuantityValues->quantityName = "S-Parameter (Re)";
		imQuantityValues->quantityName = "S-Parameter (Im)";
		for (uint32_t row = 0; row < realQuantityValues->quantityDataNumberOfRows; row++)
		{
			uint32_t index = row * realQuantityValues->quantityDataNumberOfColumns;
			for (uint32_t column = 0; column < realQuantityValues->quantityDataNumberOfColumns; column++)
			{
				if (sParameterValues[index] != nullptr)
				{
					std::vector<double>& realValuesOverAllFrequencies =	sParameterValues[index]->getYreValues();
					std::vector<double>& imValuesOverAllFrequencies =	sParameterValues[index]->getYimValues();
					for (size_t frequencyIndex = 0; frequencyIndex < numberOfFrequencyPoints; frequencyIndex++)
					{
						ot::Variable realValue = ot::Variable(realValuesOverAllFrequencies[frequencyIndex]);
						ot::Variable imagValue = ot::Variable(imValuesOverAllFrequencies[frequencyIndex]);

						realQuantityValues->quantityValues[frequencyIndex].setValue(row, column, std::move(realValue));
						imQuantityValues->quantityValues[frequencyIndex].setValue(row, column, std::move(imagValue));
					}
				}
				index++;
			}
		}
		realValuesDescription.setQuantityDescription(realQuantityValues.release());
		imValuesDescription.setQuantityDescription(imQuantityValues.release());
		newSParameterDescriptions.push_back(std::move(realValuesDescription));
		newSParameterDescriptions.push_back(std::move(imValuesDescription));
	}
	return newSParameterDescriptions;
}

std::list<std::shared_ptr<ParameterDescription>> ParametricResult1DManager::extractParameterDescription(const std::string& _category, RunIDContainer* _runIDContainer, int _runID)
{
	std::list<std::shared_ptr<ParameterDescription>> allParameter;
	std::map<std::string, Result1DData*> categoryResults = _runIDContainer->getResultsForCategory(_category);

	//First is the parameter of the X-Axis. Important to stay like this!
	auto firstCurveData = categoryResults.begin()->second;
	std::string xLabel, xUnit;
	parseAxisLabel(firstCurveData->getXLabel(), xLabel, xUnit);
	std::vector<ot::Variable> parameterValuesXAxis;
	parameterValuesXAxis.reserve(firstCurveData->getXValues().size());
	for (const double& xValue : firstCurveData->getXValues())
	{
		parameterValuesXAxis.push_back(ot::Variable(xValue));
	}
	std::shared_ptr<ParameterDescription> parameterXAxis(std::make_shared<ParameterDescription>());
	parameterXAxis->parameterName = xLabel;
	parameterXAxis->parameterValues = std::move(parameterValuesXAxis);
	parameterXAxis->unitName = xUnit;
	parameterXAxis->valueDataType = ot::TypeNames::getDoubleTypeName();
	allParameter.push_back(std::move(parameterXAxis));

	//Second the run ID
	std::shared_ptr<ParameterDescription> parameterRunID(std::make_shared<ParameterDescription>());
	parameterRunID->parameterValues.push_back(ot::Variable(_runID));
	parameterRunID->parameterName = m_parameterNameRunID;
	parameterRunID->valueDataType = ot::TypeNames::getInt32TypeName();
	allParameter.push_back(std::move(parameterRunID));

	// Third all structure parameters
	for (auto param : _runIDContainer->getParameters())
	{
		std::shared_ptr<ParameterDescription> parameterStructure(std::make_shared<ParameterDescription>());
		parameterStructure->parameterName = param.first;
		parameterStructure->parameterValues = { ot::Variable(param.second) };
		parameterStructure->valueDataType = ot::TypeNames::getDoubleTypeName();
		allParameter.push_back(std::move(parameterStructure));
	}

	return allParameter;
}

void ParametricResult1DManager::extractMetadataFromDescriptions(std::map<std::string, MetadataQuantity>& _quantitiesByName, std::map<std::string, MetadataParameter>& _parametersByName, std::list<DataDescription1D>& _allDataDescriptions)
{

	for (auto& dataDescription : _allDataDescriptions)
	{
		//First, we create a corresponding Quantity 
		QuantityDescription* quantityDescription = dataDescription.getQuantity();
		auto quantityMetadataByName = _quantitiesByName.find(quantityDescription->quantityName);
		if (quantityMetadataByName != _quantitiesByName.end())
		{
			//The Quantity was already added. So we need to check if the information about this quantity are consistent.
			MetadataQuantity& quantityMetadata = quantityMetadataByName->second;
			if (quantityMetadata.dataColumns != quantityDescription->quantityDataNumberOfColumns ||
				quantityMetadata.dataRows != quantityDescription->quantityDataNumberOfRows ||
				quantityMetadata.typeName != ot::TypeNames::getDoubleTypeName())
			{
				//Quantity exists already by name, but holds different characteristics
				//Display: Warning regarding changed name.
				quantityDescription->quantityLabel = quantityDescription->quantityName;
				int i = 0;
				do
				{
					i++;
					quantityDescription->quantityName = quantityDescription->quantityName + "_" + std::to_string(i);
					quantityMetadataByName = _quantitiesByName.find(quantityDescription->quantityName);
				} while (quantityMetadataByName != _quantitiesByName.end());

				//Name eventuell mit _ erweitern
				//Originalen Namen behalten, ansonsten 
				//dataDescription.quantityName = newInternalName
				//Einmal einen UniqueName und dann ein Label. UniqueName = Label, außer UniqueName existiert bereits. Dann wird nämlich hochgezählt
				//Das löst aber nicht das Problem bei Verteilung. Es kann sein, dass zwei Datensätze parallel hinzugefügt werden. Die UID unterscheidet sich. Der Unique Name ist aber identisch.
				_quantitiesByName[quantityDescription->quantityName] = createNewQuantity(quantityDescription);
			}
		}
		else
		{
			_quantitiesByName[quantityDescription->quantityName] = createNewQuantity(quantityDescription);
		}

		//Now we create the parameter
		addParameterMetadata(_parametersByName, dataDescription.getXAxisParameter());

		for (auto& parameter : dataDescription.getParameters())
		{
			addParameterMetadata(_parametersByName, parameter);
		}

	}
}

void ParametricResult1DManager::addParameterMetadata(std::map<std::string, MetadataParameter>& _parametersByName, std::shared_ptr<ParameterDescription> _parameter)
{
	auto parameterByName = _parametersByName.find(_parameter->parameterName);
	if (parameterByName != _parametersByName.end())
	{

		if (parameterByName->second.unit != _parameter->unitName ||
			parameterByName->second.typeName != _parameter->valueDataType)
		{

			//Needs a notification
			//There is already a parameter with the same name but differences in its other criterias.
			_parameter->parameterLabel = _parameter->parameterName;
			int i = 0;
			do
			{
				i++;
				_parameter->parameterName = _parameter->parameterName + "_" + std::to_string(i);
				parameterByName = _parametersByName.find(_parameter->parameterName);
			} while (parameterByName != _parametersByName.end());

			_parametersByName[_parameter->parameterName] = createNewParameter(_parameter.get());
		}
		else
		{

			std::list<ot::Variable>* existingValues = &(parameterByName->second.values);
			std::list<ot::Variable> newValues = { _parameter->parameterValues.begin(), _parameter->parameterValues.end() };
			existingValues->insert(existingValues->end(), newValues.begin(), newValues.end());
			existingValues->sort();
			existingValues->unique();
		}
	}
	else
	{
		_parametersByName[_parameter->parameterName] = createNewParameter(_parameter.get());
	}
}

void ParametricResult1DManager::processDataPoints(DataDescription1D& dataDescription, ResultCollectionExtender& _resultCollectionExtender, std::string& seriesName)
{
	const auto seriesMetadata = _resultCollectionExtender.FindMetadataSeries(seriesName);
	std::list<std::string> parameterAbbreviations;
	std::list<ot::Variable> sharedParameterValues;
	
	//In the extraction of parameters, the first is the x-axis. This is the only varying parameter for a curve
	assert(dataDescription.getXAxisParameter()->parameterValues.size()>1);

	//Add the first parameter abbreviation, which is corresponding to the x-axis parameter
	const std::string xAxisParameterName = dataDescription.getXAxisParameter()->parameterName;
	const MetadataParameter* metadataXAxisParameter = _resultCollectionExtender.FindMetadataParameter(xAxisParameterName);
	parameterAbbreviations.push_back(metadataXAxisParameter->parameterAbbreviation);

	//Add all the other parameters
	for (auto& parameter : dataDescription.getParameters())
	{
		const MetadataParameter* metadataParameter = _resultCollectionExtender.FindMetadataParameter(parameter->parameterName);
		
		//Those parameter which are fixed for one curve, so they are suppose to have only one entry.
		if (parameter->parameterValues.size() != 1)
		{
			//Too strickt ?
			std::string message = "Cannot import data. Appart from the x-axis parameter, there is the parameter " + parameter->parameterName + ", which has more then one value within a single curve.";
			throw std::exception(message.c_str());
		}
		else
		{
			parameterAbbreviations.push_back(metadataParameter->parameterAbbreviation);
			sharedParameterValues.push_back(parameter->parameterValues.front());
		}
	}
	QuantityDescription* currentQuantityDescription = dataDescription.getQuantity();
	const MetadataQuantity* quantityMetadata = _resultCollectionExtender.FindMetadataQuantity(currentQuantityDescription->quantityName);

	auto xParameter =	dataDescription.getXAxisParameter();
	auto xParameterValue = xParameter->parameterValues.begin();
	 dataDescription.getQuantity();
	QuantityDescriptionCurve* curveDescription = dynamic_cast<QuantityDescriptionCurve*>(currentQuantityDescription);

	if (curveDescription != nullptr)
	{
		assert(xParameter->parameterValues.size() == curveDescription->quantityValues.size());
		for (auto& quantityValue : curveDescription->quantityValues)
		{
			std::list<ot::Variable> currentParameterValues{ *xParameterValue };
			currentParameterValues.insert(currentParameterValues.end(), sharedParameterValues.begin(), sharedParameterValues.end());
			_resultCollectionExtender.AddQuantityContainer(seriesMetadata->getSeriesIndex(), parameterAbbreviations, currentParameterValues, quantityMetadata->quantityIndex,quantityValue);
			if (xParameterValue != xParameter->parameterValues.end())
			{
				xParameterValue++;
			}
		}
	}
	else
	{
		QuantityDescriptionSParameter* sParameterDescription = dynamic_cast<QuantityDescriptionSParameter*>(currentQuantityDescription);
		assert(sParameterDescription != nullptr);
		assert(xParameter->parameterValues.size() == sParameterDescription->quantityValues.size());
		_resultCollectionExtender.FlushQuantityContainer();
		_resultCollectionExtender.setBucketSize(sParameterDescription->quantityDataNumberOfColumns * sParameterDescription->quantityDataNumberOfRows);
		for (auto& quantityValue : sParameterDescription->quantityValues)
		{
			std::list<ot::Variable> currentParameterValues{ *xParameterValue };
			currentParameterValues.insert(currentParameterValues.end(), sharedParameterValues.begin(), sharedParameterValues.end());
			for (uint32_t row = 0; row < quantityValue.getPortNumber(); row++)
			{
				for (uint32_t column = 0; column < quantityValue.getPortNumber(); column++)
				{
					const ot::Variable& quantityValueEntry = quantityValue.getValue(row,column);
					_resultCollectionExtender.AddQuantityContainer(seriesMetadata->getSeriesIndex(), parameterAbbreviations, currentParameterValues, quantityMetadata->quantityIndex, quantityValueEntry);
				}
			}

			if (xParameterValue != xParameter->parameterValues.end())
			{
				xParameterValue++;
			}
		}
	}

	
}

MetadataQuantity ParametricResult1DManager::createNewQuantity(const QuantityDescription* const _quantityDescription)
{
	MetadataQuantity newQuantity;
	newQuantity.dataColumns = _quantityDescription->quantityDataNumberOfColumns;
	newQuantity.dataRows = _quantityDescription->quantityDataNumberOfRows;
	newQuantity.quantityName = _quantityDescription->quantityName;
	newQuantity.quantityLabel = _quantityDescription->quantityLabel;
	newQuantity.unit = _quantityDescription->quantityUnit;
	newQuantity.typeName = ot::TypeNames::getDoubleTypeName();
	return newQuantity;
}

MetadataParameter ParametricResult1DManager::createNewParameter(const ParameterDescription* const _paramParameter)
{
	MetadataParameter newParameterMetadata;
	newParameterMetadata.parameterName = _paramParameter->parameterName;
	newParameterMetadata.parameterLabel = _paramParameter->parameterLabel;
	newParameterMetadata.unit = _paramParameter->unitName;
	newParameterMetadata.typeName = _paramParameter->valueDataType;
	newParameterMetadata.values = { _paramParameter->parameterValues.begin(),_paramParameter->parameterValues.end() };
	newParameterMetadata.values.sort();
	newParameterMetadata.values.unique();
	return newParameterMetadata;
}

int ParametricResult1DManager::determineNumberOfPorts(const std::string& category, std::map<std::string, Result1DData*>& categoryResults, std::vector<Result1DData*>& sources)
{
	int maxPort = 0;

	for (auto item : categoryResults)
	{
		std::string itemName = item.first.substr(category.size() + 2); // We skip the path and the leading S in front of the port identifier i,j

		size_t index = itemName.find(',');
		assert(index != std::string::npos);

		std::string port1 = itemName.substr(0, index);
		std::string port2 = itemName.substr(index + 1);

		int nPort1 = atoi(port1.c_str());
		int nPort2 = atoi(port2.c_str());

		maxPort = std::max(maxPort, nPort1);
		maxPort = std::max(maxPort, nPort2);
	}

	sources.resize(maxPort * maxPort);

	for (auto item : categoryResults)
	{
		std::string itemName = item.first.substr(category.size() + 2); // We skip the path and the leading S in front of the port identifier i,j

		size_t index = itemName.find(',');
		assert(index != std::string::npos);

		std::string port1 = itemName.substr(0, index);
		std::string port2 = itemName.substr(index + 1);

		int nPort1 = atoi(port1.c_str());
		int nPort2 = atoi(port2.c_str());

		sources[(nPort1 - 1) * maxPort + (nPort2 - 1)] = item.second;
	}

	return maxPort;
}

void ParametricResult1DManager::parseAxisLabel(const std::string& value, std::string& label, std::string& unit)
{
	// Here we separate the value into the actual axis type and the unit. We assume that both are separated by a / character
	size_t separatorIndex = value.find('/');

	label = value.substr(0, separatorIndex);
	unit = value.substr(separatorIndex + 1);

	boost::trim(label);
	boost::trim(unit);
}

