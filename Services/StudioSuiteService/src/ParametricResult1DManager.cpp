#include "ParametricResult1DManager.h"
#include "Result1DManager.h"
#include "Application.h"
#include "DataBase.h"

#include "OTServiceFoundation/ModelComponent.h"
#include "OTCore/FolderNames.h"

#include "ResultCollectionExtender.h"
#include "MetadataSeries.h"
#include "MetadataParameter.h"
#include "MetadataEntrySingle.h"

#include <boost/algorithm/string.hpp>

ParametricResult1DManager::ParametricResult1DManager(Application *app) :
	resultFolderName("Results"),
	application(app)
{

}

ParametricResult1DManager::~ParametricResult1DManager()
{

}

void ParametricResult1DManager::clear()
{
	// We delete all previous result data (series), since there was a non-parametric change
	std::list<std::string> resultFolderContent = application->modelComponent()->getListOfFolderItems(resultFolderName, false);

	application->modelComponent()->deleteEntitiesFromModel(resultFolderContent, false);
}

void ParametricResult1DManager::add(Result1DManager& result1DManager)
{
	// We add the data in result1DManager to the parametric storage
	std::list<int> runIDList = result1DManager.getRunIDList();
	if (runIDList.empty()) return;

	std::string runIDLabel = determineRunIDLabel(runIDList);

	std::string collectionName = DataBase::GetDataBase()->getProjectName();

	ResultCollectionExtender resultCollectionExtender(collectionName, *application->modelComponent(), &application->getClassFactory(), OT_INFO_SERVICE_TYPE_STUDIOSUITE);
	resultCollectionExtender.setSaveModel(false);

	// Now we process the different types of data entries
	processCurves("1D Results/Balance", runIDLabel, runIDList, result1DManager, resultCollectionExtender);
	//processCurves("1D Results/Energy", runIDLabel, runIDList, result1DManager, resultCollectionExtender);
	processCurves("1D Results/Port signals", runIDLabel, runIDList, result1DManager, resultCollectionExtender);
	processCurves("1D Results/Power", runIDLabel, runIDList, result1DManager, resultCollectionExtender);
	processCurves("1D Results/Reference Impedance", runIDLabel, runIDList, result1DManager, resultCollectionExtender);
//	processSparameters("1D Results/S-Parameters", runIDLabel, runIDList,result1DManager, resultCollectionExtender);
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

void ParametricResult1DManager::processCurves(const std::string& category, const std::string &runIDLabel, std::list<int> &runIDList, Result1DManager& result1DManager, ResultCollectionExtender &resultCollectionExtender)
{

	for (auto runID : runIDList)
	{
		RunIDContainer* container = result1DManager.getContainer(runID);
		assert(container != nullptr);

		std::map<std::string, Result1DData*> categoryResults = container->getResultsForCategory(category);
		if (categoryResults.empty()) continue;

		size_t numberOfXValues = categoryResults.begin()->second->getXValues().size();

		bool hasRealPart    = !(categoryResults.begin()->second->getYreValues().empty());
		bool hasImagPart    = !(categoryResults.begin()->second->getYimValues().empty());
		int numberOfVectors = (hasRealPart ? 1 : 0) + (hasImagPart ? 1 : 0);

		size_t numberOfQuantities = categoryResults.size() * numberOfVectors;

		std::string seriesName = ot::FolderNames::DatasetFolder + "/" + category + "/" + runIDLabel;
		ot::UID seriesID       = application->modelComponent()->createEntityUID();
		
		std::list<ot::Variable> xValues;

		// Add the meta data
		addCurveSeriesMetadata(resultCollectionExtender, category, seriesName, seriesID, container, categoryResults, xValues, hasRealPart, hasImagPart, numberOfQuantities);

		// Now we add the quantity data
		addCurveSeriesQuantityData(resultCollectionExtender, seriesName, seriesID, numberOfXValues, categoryResults, hasRealPart, hasImagPart);
	}
}

void ParametricResult1DManager::addCurveSeriesMetadata(ResultCollectionExtender &resultCollectionExtender, const std::string& category, const std::string &seriesName, ot::UID seriesID, RunIDContainer* container, 
													   std::map<std::string, Result1DData*> &categoryResults, std::list<ot::Variable> &xValues,
													   bool hasRealPart, bool hasImagPart, size_t numberOfQuantities)
{
	MetadataSeries seriesMetadata(seriesName, seriesID);

	// Add all structure parameters
	for (auto param : container->getParameters())
	{
		std::shared_ptr<MetadataEntry>mdParameter(new MetadataEntrySingle(param.first, ot::Variable(param.second)));
		seriesMetadata.AddMetadata(mdParameter);
	}

	// Determine x-axis type and unit
	std::string xLabel, xUnit;
	parseAxisLabel(categoryResults.begin()->second->getXLabel(), xLabel, xUnit);

	// Determine the xvalues
	for (auto x : categoryResults.begin()->second->getXValues())
	{
		xValues.push_back(x);
	}

	// Create the axis parameter
	MetadataParameter parameter;
	std::shared_ptr<MetadataEntry> mdXaxisSetting(new MetadataEntrySingle("Unit", xUnit));
	parameter.metaData.insert(std::make_pair<>("Unit", mdXaxisSetting));
	parameter.parameterName = xLabel;
	parameter.values = xValues;
	parameter.typeName = "double";

	seriesMetadata.AddParameter(std::move(parameter));

	// Add the quantities (each curve will be added as a separate quantity)
	std::vector<MetadataQuantity> quantities;
	quantities.resize(numberOfQuantities);

	int quantityIndex = 0;
	for (auto curve : categoryResults)
	{
		std::string prefix = curve.first.substr(category.size() + 1);

		if (hasRealPart && hasImagPart)
		{
			quantities[quantityIndex].quantityName = prefix + " (Re)";
			quantityIndex++;
			quantities[quantityIndex].quantityName = prefix + " (Im)";
			quantityIndex++;
		}
		else if (hasRealPart)
		{
			quantities[quantityIndex].quantityName = prefix;
			quantityIndex++;
		}
		else if (hasImagPart)
		{
			quantities[quantityIndex].quantityName = prefix;
			quantityIndex++;
		}
		else
		{
			assert(0);
		}
	}

	for (auto quantity : quantities)
	{
		quantity.typeName = "double";
		quantity.dataRows = 1;
		quantity.dataColumns = 1;

		seriesMetadata.AddQuantity(std::move(quantity));
	}

	resultCollectionExtender.AddSeries(std::move(seriesMetadata));
	resultCollectionExtender.setBucketSize(1);
}

void ParametricResult1DManager::addCurveSeriesQuantityData(ResultCollectionExtender& resultCollectionExtender, const std::string& seriesName, ot::UID seriesID, size_t numberOfXValues, 
														   std::map<std::string, Result1DData*>& categoryResults, bool hasRealPart, bool hasImagPart)
{
	const auto seriesMetadata = resultCollectionExtender.FindMetadataSeries(seriesName);
	const auto& allParameter = seriesMetadata->getParameter();
	assert(allParameter.size() == 1);

	std::list<std::string> parameterAbbrev{ allParameter.front().parameterAbbreviation };

	const auto allQuantities = seriesMetadata->getQuantities();

	size_t xIndex = 0;
	for (size_t xIndex = 0; xIndex < numberOfXValues; xIndex++)
	{
		auto currentQuantity = allQuantities.begin();

		for (auto curve : categoryResults)
		{
			Result1DData* data = curve.second;

			double xValue = data->getXValues()[xIndex];

			if (hasRealPart && hasImagPart)
			{
				resultCollectionExtender.AddQuantityContainer(seriesID, parameterAbbrev, { xValue }, currentQuantity->quantityIndex, data->getYreValues()[xIndex]);
				currentQuantity++;
				resultCollectionExtender.AddQuantityContainer(seriesID, parameterAbbrev, { xValue }, currentQuantity->quantityIndex, data->getYimValues()[xIndex]);
				currentQuantity++;
			}
			else if (hasRealPart)
			{
				resultCollectionExtender.AddQuantityContainer(seriesID, parameterAbbrev, { xValue }, currentQuantity->quantityIndex, data->getYreValues()[xIndex]);
				currentQuantity++;
			}
			else if (hasImagPart)
			{
				resultCollectionExtender.AddQuantityContainer(seriesID, parameterAbbrev, { xValue }, currentQuantity->quantityIndex, data->getYimValues()[xIndex]);
				currentQuantity++;
			}
		}
	}
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

void ParametricResult1DManager::processSparameters(const std::string& category, const std::string& runIDLabel, std::list<int>& runIDList, Result1DManager& result1DManager, ResultCollectionExtender& resultCollectionExtender)
{
	assert(0); // Implementation is missing
}

