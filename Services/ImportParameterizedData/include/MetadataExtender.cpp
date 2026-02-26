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
		
		ot::JsonDocument newMetadata;
		std::string currentJson;
		try
		{
			for (auto& jsonFile : jsonFileEntities)
			{
				currentJson = jsonFile->getName();
				const std::string& content = jsonFile->getText();
				ot::JsonDocument subDoc;
				subDoc.fromJson(content);
				mergeObjects(newMetadata, subDoc, newMetadata.GetAllocator());
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
				mergeObjects(individualMetadata, newMetadata, individualMetadata.GetAllocator());
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

void MetadataExtender::mergeObjects(rapidjson::Value& _dstObject, rapidjson::Value& _srcObject, rapidjson::Document::AllocatorType& _allocator, bool _secureMerge)
{
	for (auto srcIt = _srcObject.MemberBegin(); srcIt != _srcObject.MemberEnd(); ++srcIt)
	{
		auto dstIt = _dstObject.FindMember(srcIt->name);
		if (dstIt != _dstObject.MemberEnd())
		{
			if (srcIt->value.GetType() != dstIt->value.GetType())
			{
				if (_secureMerge) {
					throw std::exception(("Type conflict in the field: " + std::string(srcIt->name.GetString())).c_str());
				}
				else
				{
					continue;
				}
			}
			if (srcIt->value.IsArray())
			{
				for (auto arrayIt = srcIt->value.Begin(); arrayIt != srcIt->value.End(); ++arrayIt)
				{
					dstIt->value.PushBack(*arrayIt, _allocator);
				}
			}
			else if (srcIt->value.IsObject())
			{
				mergeObjects(dstIt->value, srcIt->value, _allocator);
			}
			else
			{
				dstIt->value.CopyFrom(srcIt->value, _allocator);
				dstIt->value = srcIt->value;
			}
		}
		else
		{
			ot::JsonValue value;
			value.CopyFrom(srcIt->value, _allocator);
			_dstObject.AddMember(srcIt->name, value,_allocator);
		}
	}
}
