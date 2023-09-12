#include "MeasurementCampaignHandler.h"
#include "Application.h"
#include "ClassFactory.h"
#include "OpenTwinCore/FolderNames.h"
#include "DataBase.h"

void MeasurementCampaignHandler::ConnectToCollection(const std::string& collectionName, const std::string& projectName)
{
	std::list<ot::EntityInformation> allMeasurementMetadata = getMSMDEntityInformation(collectionName, projectName);

	if(allMeasurementMetadata.size() > 0)
	{

		std::string oldProjectName = DataBase::GetDataBase()->getProjectName();
		DataBase::GetDataBase()->setProjectName(collectionName);
		Application::instance()->prefetchDocumentsFromStorage(allMeasurementMetadata);

		std::vector<std::shared_ptr<EntityMeasurementMetadata>> measurementMetadata;
		measurementMetadata.reserve(allMeasurementMetadata.size());

		ClassFactory classFactory;
		for (auto& entityInfo : allMeasurementMetadata)
		{
			auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
			auto metadata = dynamic_cast<EntityMeasurementMetadata*>(baseEnt);
			assert(metadata != nullptr);
			measurementMetadata.push_back(std::shared_ptr<EntityMeasurementMetadata>(metadata));
		}

		DataBase::GetDataBase()->setProjectName(oldProjectName);

		CollectMetaInformation(measurementMetadata);
	}
	else
	{
		_uiComponent->displayDebugMessage("No measurement metadata detected in project: " + projectName);
	}
}

std::list<ot::EntityInformation> MeasurementCampaignHandler::getMSMDEntityInformation(const std::string& collectionName, const std::string& projectName)
{
	EntityMeasurementMetadata temp(0, nullptr, nullptr, nullptr, nullptr, "");

	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_GET_ENTITIES_FROM_ANOTHER_COLLECTION);
	ot::rJSON::add(doc, OT_ACTION_PARAM_PROJECT_NAME, projectName);
	ot::rJSON::add(doc, OT_ACTION_PARAM_COLLECTION_NAME, collectionName);
	ot::rJSON::add(doc, OT_ACTION_PARAM_Folder, ot::FolderNames::DatasetFolder);
	ot::rJSON::add(doc, OT_ACTION_PARAM_Type, temp.getClassName());
	ot::rJSON::add(doc, OT_ACTION_PARAM_Recursive, true);

	std::string response = Application::instance()->sendMessage(false, OT_INFO_SERVICE_TYPE_MODEL, doc);
	OT_rJSON_parseDOC(responseDoc, response.c_str());
	ot::UIDList entityIDs = ot::rJSON::getULongLongList(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
	ot::UIDList entityVersions = ot::rJSON::getULongLongList(responseDoc, OT_ACTION_PARAM_MODEL_EntityVersionList);

	auto version = entityVersions.begin();
	std::list<ot::EntityInformation> entityInfos;
	for (ot::UID id : entityIDs)
	{
		ot::EntityInformation entityInfo;
		entityInfo.setID(id);
		entityInfo.setVersion(*version);
		version++;
	}

	return entityInfos;
}

void MeasurementCampaignHandler::CollectMetaInformation(std::vector<std::shared_ptr<EntityMeasurementMetadata>>& measurementMetadata)
{
	for (auto& metaData : measurementMetadata)
	{
		metaData->getAllQuantityDocumentNames();
	}
}

std::vector<std::string> MeasurementCampaignHandler::GetParameterList()
{
	

	return std::vector<std::string>();
}

std::vector<std::string> MeasurementCampaignHandler::GetQuantityList()
{
	return std::vector<std::string>();
}

