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
	
	std::list<CurveDescription>	allCurveDescriptions;
	std::list<SParameterDescription> allSParamterDescriptions;

	for (int runID : runIDList)
	{
		RunIDContainer* runIDContainer = result1DManager.getContainer(runID);
		assert(runIDContainer != nullptr);

		for (const std::string& category : categories)
		{
			std::list<CurveDescription>	curveDescriptions = extractCurveDescriptions(category, runIDContainer,runID);
			allCurveDescriptions.splice(allCurveDescriptions.end(), std::move(curveDescriptions));
		}
		std::list<SParameterDescription> sparameterDescriptions = extractSParameterDescription("1D Results/S-Parameters", runIDContainer,runID);
		allSParamterDescriptions.splice(allSParamterDescriptions.end(), std::move(sparameterDescriptions));
	}

	//Sort out the common parameter and Quanity definitions	
	std::map<std::string, MetadataQuantity> quantitiesByName;
	std::map<std::string, MetadataParameter> parametersByName;

	std::list<DataDescription*> allDataDescriptions;
	for (CurveDescription& curveDescription : allCurveDescriptions)
	{
		allDataDescriptions.push_back(&curveDescription);
	}
	for (SParameterDescription& sParameterDescription : allSParamterDescriptions)
	{
		allDataDescriptions.push_back(&sParameterDescription);
	}
	
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
	for (DataDescription* dataDescription : allDataDescriptions)
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

std::list<CurveDescription>  ParametricResult1DManager::extractCurveDescriptions(const std::string& _category, RunIDContainer* _runIDContainer, int _runID)
{
	std::list<CurveDescription> allCurveDescriptions;
	std::map<std::string, Result1DData*> categoryResults = _runIDContainer->getResultsForCategory(_category);
	if (!categoryResults.empty())
	{
	
		//We determine the parameter, which are shared by all curves.
		auto allParameter =	extractCurveParameters(_category, _runIDContainer, _runID);

		//Now we create the individual curve descriptions
		for (auto& curve : categoryResults)
		{
			CurveDescription newCurveDescription;
			newCurveDescription.parameters = allParameter;

			Result1DData* curveData = curve.second;
			std::string prefix = curve.first.substr(_category.size() + 1);
			if (!curveData->getYreValues().empty())
			{
				newCurveDescription.quantityName = prefix + " (Re)";
				newCurveDescription.quantityValues.reserve(curveData->getYreValues().size());
				for (auto realValue : curveData->getYreValues())
				{
					newCurveDescription.quantityValues.push_back(ot::Variable(realValue));
				}
				allCurveDescriptions.push_back(std::move(newCurveDescription));
				newCurveDescription = CurveDescription();
			}
			if (!curveData->getYimValues().empty())
			{
				//In case that we already created a curve description for the real component
				if (newCurveDescription.parameters == nullptr)
				{
					newCurveDescription.parameters = allParameter;
				}
				newCurveDescription.quantityName = prefix + " (Im)";
				newCurveDescription.quantityValues.reserve(curveData->getYimValues().size());
				for (auto imValue : curveData->getYimValues())
				{
					newCurveDescription.quantityValues.push_back(ot::Variable(imValue));
				}
				allCurveDescriptions.push_back(std::move(newCurveDescription));
			}
		}
	}
	return allCurveDescriptions;
}

std::list<SParameterDescription> ParametricResult1DManager::extractSParameterDescription(const std::string& _category, RunIDContainer* _runIDContainer, int _runID)
{
	std::list<SParameterDescription> newSParameterDescriptions;

	std::map<std::string, Result1DData*> categoryResults = _runIDContainer->getResultsForCategory(_category);
	if (!categoryResults.empty())
	{
		//We determine the characteristics that are shared by all curves.
		auto allParameter = extractCurveParameters(_category, _runIDContainer, _runID);

		SParameterDescription realValuesDescription, imValuesDescription;
		realValuesDescription.parameters = allParameter;

		//Now we set the s-parameter matrices
		std::vector<Result1DData*> sParameterValues;
		int numberOfPorts =	determineNumberOfPorts(_category, categoryResults, sParameterValues);
		realValuesDescription.quantityDataNumberOfColumns= static_cast<uint32_t>(numberOfPorts);
		realValuesDescription.quantityDataNumberOfRows= static_cast<uint32_t>(numberOfPorts);
		
		//Dangerous?
		size_t numberOfFrequencyPoints = sParameterValues[0]->getYreValues().size();
		realValuesDescription.quantityValues.reserve(numberOfFrequencyPoints);
		for (size_t i = 0; i < numberOfFrequencyPoints; i++)
		{
			realValuesDescription.quantityValues.push_back(SParameterMatrixHelper(realValuesDescription.quantityDataNumberOfRows));
		}
		
		imValuesDescription = realValuesDescription;
		realValuesDescription.quantityName = "S-Parameter (Re)";
		imValuesDescription.quantityName = "S-Parameter (Im)";
		for (uint32_t row = 0; row < realValuesDescription.quantityDataNumberOfRows; row++)
		{
			uint32_t index = row * realValuesDescription.quantityDataNumberOfColumns;
			for (uint32_t column = 0; column < realValuesDescription.quantityDataNumberOfColumns; column++)
			{
				if (sParameterValues[index] != nullptr)
				{
					std::vector<double>& realValuesOverAllFrequencies =	sParameterValues[index]->getYreValues();
					std::vector<double>& imValuesOverAllFrequencies =	sParameterValues[index]->getYimValues();
					for (size_t frequencyIndex = 0; frequencyIndex < numberOfFrequencyPoints; frequencyIndex++)
					{
						ot::Variable realValue = ot::Variable(realValuesOverAllFrequencies[frequencyIndex]);
						ot::Variable imagValue = ot::Variable(imValuesOverAllFrequencies[frequencyIndex]);

						realValuesDescription.quantityValues[frequencyIndex].setValue(row, column, std::move(realValue));
						imValuesDescription.quantityValues[frequencyIndex].setValue(row, column, std::move(imagValue));
					}
				}
				index++;
			}
		}
		newSParameterDescriptions.push_back(std::move(realValuesDescription));
		newSParameterDescriptions.push_back(std::move(imValuesDescription));
	}
	return newSParameterDescriptions;
}

std::shared_ptr<std::list<CurveParameter>> ParametricResult1DManager::extractCurveParameters(const std::string& _category, RunIDContainer* _runIDContainer, int _runID)
{
	std::shared_ptr<std::list<CurveParameter>> allParameter(new std::list<CurveParameter>());
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
	CurveParameter parameterXAxis;
	parameterXAxis.parameterName = xLabel;
	parameterXAxis.parameterValues = std::move(parameterValuesXAxis);
	parameterXAxis.unitName = xUnit;
	parameterXAxis.valueDataType = ot::TypeNames::getDoubleTypeName();
	allParameter->push_back(std::move(parameterXAxis));

	//Second the run ID
	CurveParameter parameterRunID;
	parameterRunID.parameterValues.push_back(ot::Variable(_runID));
	parameterRunID.parameterName = m_parameterNameRunID;
	parameterRunID.valueDataType = ot::TypeNames::getInt32TypeName();
	allParameter->push_back(std::move(parameterRunID));

	// Third all structure parameters
	for (auto param : _runIDContainer->getParameters())
	{
		CurveParameter parameterStructure;
		parameterStructure.parameterName = param.first;
		parameterStructure.parameterValues = { ot::Variable(param.second) };
		parameterStructure.valueDataType = ot::TypeNames::getDoubleTypeName();
		allParameter->push_back(std::move(parameterStructure));
	}

	return allParameter;
}

void ParametricResult1DManager::extractMetadataFromDescriptions(std::map<std::string, MetadataQuantity>& _quantitiesByName, std::map<std::string, MetadataParameter>& _parametersByName, std::list<DataDescription*> _allDataDescriptions)
{

	for (auto dataDescription : _allDataDescriptions)
	{
		//First, we create a corresponding Quantity 
		auto quantityMetadataByName = _quantitiesByName.find(dataDescription->quantityName);
		if (quantityMetadataByName != _quantitiesByName.end())
		{
			//The Quantity was already added. So we need to check if the information about this quantity are consistent.
			MetadataQuantity& quantityMetadata = quantityMetadataByName->second;
			if (quantityMetadata.dataColumns != dataDescription->quantityDataNumberOfColumns ||
				quantityMetadata.dataRows != dataDescription->quantityDataNumberOfRows ||
				quantityMetadata.typeName != ot::TypeNames::getDoubleTypeName())
			{
				//Quantity exists already by name, but holds different characteristics
				//Display: Warning regarding changed name.
				dataDescription->quantityLabel = dataDescription->quantityName;
				int i = 0;
				do
				{
					i++;
					dataDescription->quantityName = dataDescription->quantityName + "_" + std::to_string(i);
					quantityMetadataByName = _quantitiesByName.find(dataDescription->quantityName);
				} while (quantityMetadataByName != _quantitiesByName.end());

				//Name eventuell mit _ erweitern
				//Originalen Namen behalten, ansonsten 
				//dataDescription.quantityName = newInternalName
				//Einmal einen UniqueName und dann ein Label. UniqueName = Label, außer UniqueName existiert bereits. Dann wird nämlich hochgezählt
				//Das löst aber nicht das Problem bei Verteilung. Es kann sein, dass zwei Datensätze parallel hinzugefügt werden. Die UID unterscheidet sich. Der Unique Name ist aber identisch.
				_quantitiesByName[dataDescription->quantityName] = createNewQuantity(dataDescription);
			}
		}
		else
		{
			_quantitiesByName[dataDescription->quantityName] = createNewQuantity(dataDescription);
		}

		//Now we create the parameter

		for (auto& parameter : *dataDescription->parameters)
		{
			auto parameterByName = _parametersByName.find(parameter.parameterName);
			if (parameterByName != _parametersByName.end())
			{

				if (parameterByName->second.unit != parameter.unitName ||
					parameterByName->second.typeName != parameter.valueDataType)
				{

					//There is already a parameter with the same name but differences in its other criterias.
					parameter.parameterLabel = parameter.parameterName;
					int i = 0;
					do
					{
						i++;
						parameter.parameterName = parameter.parameterName + "_" + std::to_string(i);
						parameterByName = _parametersByName.find(parameter.parameterName);
					} while (parameterByName != _parametersByName.end());
					
					_parametersByName[parameter.parameterName] = createNewParameter(parameter);
				}
				else
				{

					std::list<ot::Variable>* existingValues = &(parameterByName->second.values);
					std::list<ot::Variable> newValues = { parameter.parameterValues.begin(), parameter.parameterValues.end() };
					existingValues->insert(existingValues->end(), newValues.begin(), newValues.end());
					existingValues->sort();
					existingValues->unique();
				}
			}
			else
			{
				_parametersByName[parameter.parameterName] = createNewParameter(parameter);
			}
		}
	}
}

void ParametricResult1DManager::processDataPoints(DataDescription* dataDescription, ResultCollectionExtender& _resultCollectionExtender, std::string& seriesName)
{
	const auto seriesMetadata = _resultCollectionExtender.FindMetadataSeries(seriesName);
	std::list<std::string> parameterAbbreviations;
	std::list<ot::Variable> parameterValues;
	//In the extraction of parameters, the first is the x-axis. This is the only varying parameter for a curve
	assert(dataDescription->parameters->begin()->parameterValues.size() > 1);
	for (auto& parameter : *dataDescription->parameters)
	{
		const MetadataParameter* metadataParameter = _resultCollectionExtender.FindMetadataParameter(parameter.parameterName);
		parameterAbbreviations.push_back(metadataParameter->parameterAbbreviation);
		
		//Those parameter which are fixed for one curve can now be added here.
		if (parameter.parameterValues.size() == 1)
		{
			parameterValues.push_back(parameter.parameterValues.front());
		}
	}

	const MetadataQuantity* quantityMetadata = _resultCollectionExtender.FindMetadataQuantity(dataDescription->quantityName);

	auto xParameter =	dataDescription->parameters->begin();
	auto xParameterValue = xParameter->parameterValues.begin();
	CurveDescription* curveDescription = dynamic_cast<CurveDescription*>(dataDescription);
	if (curveDescription != nullptr)
	{
		assert(xParameter->parameterValues.size() == curveDescription->quantityValues.size());
		for (auto& quantityValue : curveDescription->quantityValues)
		{
			std::list<ot::Variable> currentParameterValues{ *xParameterValue };
			currentParameterValues.insert(currentParameterValues.end(), parameterValues.begin(), parameterValues.end());
			_resultCollectionExtender.AddQuantityContainer(seriesMetadata->getSeriesIndex(), parameterAbbreviations, currentParameterValues, quantityMetadata->quantityIndex,quantityValue);
			if (xParameterValue != xParameter->parameterValues.end())
			{
				xParameterValue++;
			}
		}
	}
	else
	{
		SParameterDescription* sParameterDescription = dynamic_cast<SParameterDescription*>(dataDescription);
		assert(sParameterDescription != nullptr);
		assert(xParameter->parameterValues.size() == sParameterDescription->quantityValues.size());
		_resultCollectionExtender.FlushQuantityContainer();
		_resultCollectionExtender.setBucketSize(sParameterDescription->quantityDataNumberOfColumns * sParameterDescription->quantityDataNumberOfRows);
		for (auto& quantityValue : sParameterDescription->quantityValues)
		{
			std::list<ot::Variable> currentParameterValues{ *xParameterValue };
			currentParameterValues.insert(currentParameterValues.end(), parameterValues.begin(), parameterValues.end());
			for (uint32_t row = 0; row < quantityValue.getPortNumber(); row++)
			{
				for (uint32_t column = 0; column < quantityValue.getPortNumber(); column++)
				{
					ot::Variable& quantityValueEntry = quantityValue.getValue(row,column);
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

MetadataQuantity ParametricResult1DManager::createNewQuantity(DataDescription* _dataDescription)
{
	MetadataQuantity newQuantity;
	newQuantity.dataColumns = _dataDescription->quantityDataNumberOfColumns;
	newQuantity.dataRows = _dataDescription->quantityDataNumberOfRows;
	newQuantity.quantityName = _dataDescription->quantityName;
	newQuantity.quantityLabel = _dataDescription->quantityLabel;
	newQuantity.unit = _dataDescription->quantityUnit;
	newQuantity.typeName = ot::TypeNames::getDoubleTypeName();
	return newQuantity;
}

MetadataParameter ParametricResult1DManager::createNewParameter(CurveParameter& _curveParameter)
{
	MetadataParameter newParameterMetadata;
	newParameterMetadata.parameterName = _curveParameter.parameterName;
	newParameterMetadata.parameterLabel = _curveParameter.parameterLabel;
	newParameterMetadata.unit = _curveParameter.unitName;
	newParameterMetadata.typeName = _curveParameter.valueDataType;
	newParameterMetadata.values = { _curveParameter.parameterValues.begin(),_curveParameter.parameterValues.end() };
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

