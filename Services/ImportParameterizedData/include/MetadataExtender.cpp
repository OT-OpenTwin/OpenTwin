#include "MetadataExtender.h"
#include "Application.h"
#include "OTModelEntities/EntityAPI.h"
#include "OTGui/FileExtension.h"
#include "OTModelEntities/EntityMetadataSeries.h"
#include "OTModelEntities/EntityFileText.h"
#include "OTModelAPI/ModelServiceAPI.h"

void MetadataExtender::extendWithJsonFile()
{
	try
	{
		std::list<ot::EntityInformation> selectedFiles, selectedSeries;
		loadRelevantSelectedEntities(selectedFiles, selectedSeries);
		auto seriesEntities = getSelectedSeriesMetadata(selectedSeries);
		auto jsonFileEntities = getSelectedJsonFiles(selectedFiles);

		if (jsonFileEntities.size() == 0 || seriesEntities.size() == 0)
		{
			throw std::exception("Extending a metadata series requires the selection of a json file and a series metadata");
		}
		std::string seriesNames, jsonFileNames;
		for (auto& seriesEntity : seriesEntities)
		{
			seriesNames +=  seriesEntity->getName() + ", ";
		}
		for(auto& jsonFileEntity : jsonFileEntities)
		{
			jsonFileNames += jsonFileEntity->getName() + ", ";
		}
		seriesNames = seriesNames.substr(0, seriesNames.size() - 2);
		jsonFileNames = jsonFileNames.substr(0, jsonFileNames.size() - 2);
		const std::string message = "Extending metadata series: \"" + seriesNames + "\" with json file(s): \"" + jsonFileNames + "\"\n";
		Application::instance()->getUiComponent()->displayMessage(message);

		ot::JsonDocument newMetadata;
		std::string currentJson;
		try
		{
			for (auto& jsonFile : jsonFileEntities)
			{
				currentJson = jsonFile->getName();
				const std::string& content = jsonFile->getText();
				ot::JsonDocument subDoc, cleanedSubDoc;
				subDoc.fromJson(content);

				sanitiseKeys(subDoc, cleanedSubDoc, cleanedSubDoc.GetAllocator());
				ot::json::mergeObjects(newMetadata, cleanedSubDoc, newMetadata.GetAllocator());
			}
		}
		catch (std::exception& _e)
		{
			const std::string message = "Failed to merge selected json files with "+ currentJson + ": " + std::string(_e.what());
			throw std::exception(message.c_str());
		}
		
		std::string currentSeriesName;
		try
		{

			for (auto& series : seriesEntities)
			{
				currentSeriesName = series->getName();
				ot::JsonDocument individualMetadata;
				individualMetadata.CopyFrom(series->getMetadata(), individualMetadata.GetAllocator());
				ot::json::mergeObjects(individualMetadata, newMetadata, individualMetadata.GetAllocator());
				series->setMetadata(individualMetadata);
			}
		}
		catch (std::exception& _e)
		{
			const std::string message = "Failed to merge new metadata with existing metadata in series "+currentSeriesName+": "  +std::string(_e.what());
			throw std::exception(message.c_str());
		}
		ot::NewModelStateInfo newEntities;
		for (auto& series : seriesEntities)
		{
			series->storeToDataBase();
			newEntities.addTopologyEntity(*series.get());
		}
		ot::ModelServiceAPI::addEntitiesToModel(newEntities, "Extended metadata of series metadata");
		Application::instance()->getUiComponent()->displayMessage("Successfully extended series metadata.\n");
	}
	catch (std::exception& _e)
	{
		OT_LOG_E(_e.what());
	}
}


void MetadataExtender::loadRelevantSelectedEntities(std::list<ot::EntityInformation>& _selectedFiles, std::list<ot::EntityInformation>& _selectedSeries)
{

	std::list<ot::EntityInformation> entityInfos;// = Application::instance()->getSelectedEntityInfos();
	ot::ModelServiceAPI::getSelectedEntityInformation(entityInfos);

	EntityFileText textEntity;
	EntityMetadataSeries series;

	for (const ot::EntityInformation& info : entityInfos)
	{
		if (info.getEntityType() == textEntity.getClassName())
		{
			_selectedFiles.push_back(info);
		}
		if (info.getEntityType() == series.getClassName())
		{
			_selectedSeries.push_back(info);
		}
	}
	std::list<ot::EntityInformation> allRelevantEntities;
	allRelevantEntities.insert(allRelevantEntities.begin(), _selectedFiles.begin(), _selectedFiles.end());
	allRelevantEntities.insert(allRelevantEntities.begin(), _selectedSeries.begin(), _selectedSeries.end());

	Application::instance()->prefetchDocumentsFromStorage(allRelevantEntities);
}

std::list<std::unique_ptr<EntityMetadataSeries>> MetadataExtender::getSelectedSeriesMetadata(std::list<ot::EntityInformation>& _selectedSeries)
{

	std::list<std::unique_ptr<EntityMetadataSeries>> allSelectedSeries;
	for (ot::EntityInformation& seriesInfo : _selectedSeries)
	{
		std::unique_ptr<EntityBase> baseEntity(ot::EntityAPI::readEntityFromEntityIDandVersion(seriesInfo));
		allSelectedSeries.push_back(std::unique_ptr<EntityMetadataSeries>(dynamic_cast<EntityMetadataSeries*>(baseEntity.release())));
	}
	return allSelectedSeries;
}

std::list<std::unique_ptr<EntityFileText>> MetadataExtender::getSelectedJsonFiles(std::list<ot::EntityInformation>& _selectedFiles)
{
	std::list<std::unique_ptr<EntityFileText>> jsonFiles;
	for (ot::EntityInformation& fileInfo : _selectedFiles)
	{
		std::unique_ptr<EntityBase> baseEntity(ot::EntityAPI::readEntityFromEntityIDandVersion(fileInfo));
		EntityFileText* fileEntity = dynamic_cast<EntityFileText*>(baseEntity.get());
		auto fileExtension = ot::FileExtension::stringToFileExtension(fileEntity->getFileType());
		if (fileExtension == ot::FileExtension::JSON)
		{
			jsonFiles.push_back(std::unique_ptr<EntityFileText>(fileEntity));
			baseEntity.release();
		}
	}
	return jsonFiles;
}

void MetadataExtender::sanitiseKeys(const ot::JsonValue& _src, ot::JsonValue& _dest, ot::JsonAllocator& _alloc)
{
	if (_src.IsObject())
	{
		_dest.SetObject();
		for (auto& member : _src.GetObject())
		{
			// Copy and sanitise the key
			std::string key = member.name.GetString();
			ot::String::removeControlCharacters(key);

			// Recursively sanitise the value
			ot::JsonValue sanitisedValue;
			sanitiseKeys(member.value, sanitisedValue, _alloc);

			// Add the sanitised key/value pair to the destination object
			ot::JsonString keyJson(key, _alloc);
			_dest.AddMember(keyJson, sanitisedValue, _alloc);
		}
	}
	else if (_src.IsArray())
	{
		_dest.SetArray();
		for (auto& element : _src.GetArray())
		{
			ot::JsonValue sanitisedElement;
			sanitiseKeys(element, sanitisedElement, _alloc);
			_dest.PushBack(sanitisedElement, _alloc);
		}
	}
	else
	{
		// Leaf value — deep copy as-is (strings, numbers, bools, null)
		_dest.CopyFrom(_src, _alloc);
	}
}