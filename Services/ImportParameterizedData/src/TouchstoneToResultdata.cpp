#include "TouchstoneToResultdata.h"

#include "OTCore/FolderNames.h"
#include "Application.h"

#include "OTModelAPI/ModelServiceAPI.h"

#include "DataBase.h"

#include "ResultCollectionExtender.h"
#include "MetadataCampaign.h"
#include "MetadataEntrySingle.h"
#include "ParameterDescription.h"
#include "QuantityDescription.h"
#include "QuantityDescriptionSParameter.h"
#include "ValueFormatSetter.h"

#include "OptionsParameterHandlerFormat.h"
#include "OptionsParameterHandlerFrequency.h"
#include "OptionsParameterHandlerParameter.h"

#include <base64.h>
#include <zlib.h>


int TouchstoneToResultdata::getAssumptionOfPortNumber(const std::string& fileName)
{
	return TouchstoneHandler::deriveNumberOfPorts(fileName);
}

void TouchstoneToResultdata::setResultdata(const std::string& _fileName, const std::string& _fileContent, uint64_t _uncompressedLength)
{
	m_fileName = _fileName;
	m_fileContent = _fileContent;
	m_uncompressedLength = _uncompressedLength;
}

void TouchstoneToResultdata::createResultdata(int _numberOfPorts)
{	
	if (m_collectionName == "")
	{
		m_collectionName = DataBase::GetDataBase()->getProjectName();
	}
	const std::string seriesName = createSeriesName(m_fileName);
	const bool seriesExists = seriesAlreadyExists(seriesName);
	if (!seriesExists)
	{
		ResultCollectionExtender resultCollectionExtender(m_collectionName, *_modelComponent, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
		
		std::list< std::shared_ptr<MetadataEntry>> seriesMetadata;
		
		TouchstoneHandler handler(m_fileName);
		importTouchstoneFile(m_fileName, m_fileContent, m_uncompressedLength, _numberOfPorts, handler);
		DatasetDescription dataset = std::move(extractDatasetDescription(handler));
			
		auto& optionSettings = handler.getOptionSettings();
		std::string tsParameter = OptionsParameterHandlerParameter::ToString(optionSettings.getParameter());
		seriesMetadata.push_back(std::make_shared<MetadataEntrySingle>("Touchstone Parameter", tsParameter));
		seriesMetadata.push_back(std::make_shared<MetadataEntrySingle>("Reference Resistance", optionSettings.getReferenceResistance()));
		
		std::list<DatasetDescription>datasets;
		datasets.push_back(std::move(dataset));
		const ot::UID seriesID = resultCollectionExtender.buildSeriesMetadata(datasets, seriesName, seriesMetadata);
			
		resultCollectionExtender.processDataPoints(&(*datasets.begin()), seriesID);
		resultCollectionExtender.storeCampaignChanges();
	}
	else
	{
		_uiComponent->displayMessage("Series metadata with the name: " + seriesName + " already exists.");
	}
	
	m_fileContent = "";
	m_fileName = "";
	m_uncompressedLength = 0;
	m_collectionName = "";
}

const std::string TouchstoneToResultdata::createSeriesName(const std::string& _fileName)
{
	auto start = _fileName.find_last_of("/")+1;
	auto end = _fileName.find_last_of(".")-1;
	const std::string seriesName = ot::FolderNames::DatasetFolder + "/" + _fileName.substr(start, end - start);
	return seriesName;
}

bool TouchstoneToResultdata::seriesAlreadyExists(const std::string& _seriesName)
{
	std::list<std::string> folderContent = ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::DatasetFolder,true);
	for (const std::string& entityName : folderContent)
	{
		if (entityName == _seriesName)
		{
			return true;
		}
	}
	return false;
}

void TouchstoneToResultdata::importTouchstoneFile(const std::string& _fileName, const std::string& _fileContent, uint64_t _uncompressedLength, int _numberOfPorts, TouchstoneHandler& _handler)
{
	// Decode the encoded string into binary data
	int decoded_compressed_data_length = Base64decode_len(_fileContent.c_str());
	char* decodedCompressedString = new char[decoded_compressed_data_length];

	Base64decode(decodedCompressedString, _fileContent.c_str());

	// Decompress the data
	char* decodedString = new char[_uncompressedLength];
	uLongf destLen = (uLongf)_uncompressedLength;
	uLong  sourceLen = decoded_compressed_data_length;
	uncompress((Bytef*)decodedString, &destLen, (Bytef*)decodedCompressedString, sourceLen);

	delete[] decodedCompressedString;
	decodedCompressedString = nullptr;

	std::string unpackedFileContent(decodedString, _uncompressedLength);
	try
	{
		_handler.analyseFile(unpackedFileContent, _numberOfPorts);
	}
	catch (const std::exception& e)
	{
		_uiComponent->displayMessage("Failed to anaylse touchstone file due to following issue: " + std::string(e.what()));
	}
}

DatasetDescription TouchstoneToResultdata::extractDatasetDescription(TouchstoneHandler& _touchstoneHandler)
{
	DatasetDescription datasetDescription;

	MetadataParameter& metadataParameter = _touchstoneHandler.getMetadataFrequencyParameter();
	auto frequencyParameterDescription(std::make_shared<ParameterDescription>(metadataParameter, false));
	auto& frequencyParameter = frequencyParameterDescription->getMetadataParameter();

	////Maybe it is necessary to guarantee that all values have the same type ? !
	ts::OptionSettings optionSettings = _touchstoneHandler.getOptionSettings();
	frequencyParameter.unit = OptionsParameterHandlerFrequency::ToString(optionSettings.getFrequency());
	frequencyParameter.parameterName = "Frequency";
	frequencyParameter.typeName = ot::TypeNames::getDoubleTypeName();
	datasetDescription.addParameterDescription(frequencyParameterDescription);

	auto quantityDescription = _touchstoneHandler.handOverQuantityDescription();
	quantityDescription->setName("S-Parameter");
	const ts::option::Format& selectedFormat = optionSettings.getFormat();
	ValueFormatSetter valueFormatSetter;
	if (selectedFormat == ts::option::Format::Decibel_angle)
	{
		valueFormatSetter.setValueFormatDecibelPhase(*quantityDescription);
	}
	else if (selectedFormat == ts::option::Format::magnitude_angle)
	{
		valueFormatSetter.setValueFormatMagnitudePhase(*quantityDescription);
	}
	else
	{
		valueFormatSetter.setValueFormatRealImaginary(*quantityDescription,"");
	}
	datasetDescription.setQuantityDescription(quantityDescription);
	return datasetDescription;
}