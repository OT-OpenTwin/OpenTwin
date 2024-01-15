#include "TouchstoneToResultdata.h"

#include "OTCore/FolderNames.h"
#include "Application.h"
#include "ResultCollectionExtender.h"

#include "DataBase.h"

#include "MetadataCampaign.h"
#include "MetadataEntrySingle.h"

#include "OptionsParameterHandlerFormat.h"
#include "OptionsParameterHandlerFrequency.h"
#include "OptionsParameterHandlerParameter.h"

#include <base64.h>
#include <zlib.h>

TouchstoneToResultdata::TouchstoneToResultdata()
{
}

TouchstoneToResultdata::~TouchstoneToResultdata()
{
}

void TouchstoneToResultdata::CreateResultdata(const std::string& fileName, const std::string& fileContent, uint64_t uncompressedLength)
{
	
	if (_collectionName == "")
	{
		_collectionName = DataBase::GetDataBase()->getProjectName();
	}
	const std::string seriesName = CreateSeriesName(fileName);
	const bool seriesAlreadyExists = SeriesAlreadyExists(seriesName);
	if (!seriesAlreadyExists)
	{
		TouchstoneHandler handler = std::move(ImportTouchstoneFile(fileName, fileContent, uncompressedLength));
		ot::UID seriesID = _modelComponent->createEntityUID();
		MetadataSeries newSeriesMetadata(seriesName, seriesID);
		BuildSeriesMetadataFromTouchstone(handler, newSeriesMetadata);
		ResultCollectionExtender resultCollectionExtender(_collectionName, *_modelComponent, &Application::instance()->getClassFactory(), OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
		resultCollectionExtender.AddSeries(std::move(newSeriesMetadata));

		uint32_t numberOfPorts = handler.getNumberOfPorts();
		resultCollectionExtender.setBucketSize(numberOfPorts * numberOfPorts);
		const std::list<ts::PortData>& allPortData = handler.getPortData();

		const auto seriesMetadata = resultCollectionExtender.FindMetadataSeries(seriesName);
		const auto& allParameter = seriesMetadata->getParameter();
		assert(allParameter.size() == 1);
		std::list<std::string> parameterAbbrev{ allParameter.front().parameterAbbreviation };

		const auto allQuantities = seriesMetadata->getQuantities();
		assert(allQuantities.size() == 2);

		for (const ts::PortData& portData : allPortData)
		{
			auto currentQuantity = allQuantities.begin();
			const ot::Variable& frequency = portData.getFrequency();

			for (auto& portDataEntry : portData.getPortDataEntries())
			{
				resultCollectionExtender.AddQuantityContainer(seriesID, parameterAbbrev, { frequency }, currentQuantity->quantityIndex,std::get<0>(portDataEntry));
			}

			currentQuantity++;
			for (auto& portDataEntry : portData.getPortDataEntries())
			{
				resultCollectionExtender.AddQuantityContainer(seriesID, parameterAbbrev, { frequency }, currentQuantity->quantityIndex, std::get<1>(portDataEntry));
			}
		}
	}
	else
	{
		_uiComponent->displayMessage("Series metadata with the name: " + seriesName + " already exists.");
	}
}

const std::string TouchstoneToResultdata::CreateSeriesName(const std::string& fileName)
{
	auto start = fileName.find_last_of("/")+1;
	auto end = fileName.find_last_of(".")-1;
	const std::string seriesName = ot::FolderNames::DatasetFolder + "/" + fileName.substr(start, end - start);
	return seriesName;
}

bool TouchstoneToResultdata::SeriesAlreadyExists(const std::string& seriesName)
{
	std::list<std::string> folderContent =	_modelComponent->getListOfFolderItems(ot::FolderNames::DatasetFolder,true);
	for (const std::string& entityName : folderContent)
	{
		if (entityName == seriesName)
		{
			return true;
		}
	}
	return false;
}

TouchstoneHandler TouchstoneToResultdata::ImportTouchstoneFile(const std::string& fileName, const std::string& fileContent, uint64_t uncompressedLength)
{
	// Decode the encoded string into binary data
	int decoded_compressed_data_length = Base64decode_len(fileContent.c_str());
	char* decodedCompressedString = new char[decoded_compressed_data_length];

	Base64decode(decodedCompressedString, fileContent.c_str());

	// Decompress the data
	char* decodedString = new char[uncompressedLength];
	uLongf destLen = (uLongf)uncompressedLength;
	uLong  sourceLen = decoded_compressed_data_length;
	uncompress((Bytef*)decodedString, &destLen, (Bytef*)decodedCompressedString, sourceLen);

	delete[] decodedCompressedString;
	decodedCompressedString = nullptr;

	std::string unpackedFileContent(decodedString, uncompressedLength);
	TouchstoneHandler handler(fileName);
	try
	{
		handler.AnalyseFile(unpackedFileContent);
	}
	catch (const std::exception& e)
	{
		_uiComponent->displayMessage("Failed to anaylse touchstone file due to following issue: " + std::string(e.what()));
	}
	//ToDo: Display summary of touchstone data import
	return handler;
}

void TouchstoneToResultdata::BuildSeriesMetadataFromTouchstone(TouchstoneHandler& touchstoneHandler, MetadataSeries& series)
{
	ts::OptionSettings optionSettings =	touchstoneHandler.getOptionSettings();
	
	std::string tsParameter = OptionsParameterHandlerParameter::ToString(optionSettings.getParameter());
	std::shared_ptr<MetadataEntry>mdParameter(new MetadataEntrySingle("Touchstone Parameter",tsParameter));
	series.AddMetadata(mdParameter);
	
	std::shared_ptr<MetadataEntry>mdResistance(new MetadataEntrySingle("Reference Resistance", optionSettings.getReferenceResistance()));
	series.AddMetadata(mdResistance);

	MetadataParameter parameter;
	std::string frequencySetting = OptionsParameterHandlerFrequency::ToString(optionSettings.getFrequency());
	std::shared_ptr<MetadataEntry> mdFrequencySetting(new MetadataEntrySingle("Unit", frequencySetting));
	parameter.metaData.insert(std::make_pair<>("Unit",mdFrequencySetting));
	parameter.parameterName = "Frequency";

	MetadataQuantity quantity1;
	MetadataQuantity quantity2;

	const ts::option::Format& selectedFormat = optionSettings.getFormat();
	if (selectedFormat == ts::option::Format::Decibel_angle)
	{
		quantity1.quantityName = "Decibel";
		quantity2.quantityName = "Angle";

	}
	else if (selectedFormat == ts::option::Format::magnitude_angle)
	{
		quantity1.quantityName = "Magnitude";
		quantity2.quantityName = "Angle";
	}
	else
	{
		quantity1.quantityName = "Real";
		quantity2.quantityName = "Imaginary";
	}

	const auto& allPortData = touchstoneHandler.getPortData();
	std::list<ot::Variable> allFrequencies;
	
	//std::list<std::string> allTypeNamesFrequencies;

	for (const auto& portData : allPortData)
	{
		allFrequencies.push_back(portData.getFrequency());
	/*	
	* Maybe it is necessary to guarantee that all values have the same type ?!
	* 
		allTypeNamesFrequencies.push_back(portData.getFrequency().getTypeName());
		const auto& allEntries = portData.getPortDataEntries();
		std::list<std::string> typeNames1;
		std::list<std::string> typeNames2;
		
		for(const auto& entry : allEntries)
		{
			typeNames1.push_back(std::get<0>(entry).getTypeName());
			typeNames1.push_back(std::get<1>(entry).getTypeName());
		}
		typeNames1.unique();
		if (typeNames1.size() != 1)
		{
			std::string typeName = DetermineDominantType();
		}
		typeNames2.unique();
		if (typeNames2.size() != 1)
		{

		}*/
	}
	allFrequencies.unique();
	parameter.values = allFrequencies;
	
	parameter.typeName = "Numerical";
	quantity1.typeName = "Numerical";
	quantity1.dataRows = 4;
	quantity1.dataColumns = 4;

	quantity2.typeName = "Numerical";
	quantity2.dataRows = 4;
	quantity2.dataColumns = 4;

	series.AddParameter(std::move(parameter));
	series.AddQuantity(std::move(quantity1));
	series.AddQuantity(std::move(quantity2));
}
