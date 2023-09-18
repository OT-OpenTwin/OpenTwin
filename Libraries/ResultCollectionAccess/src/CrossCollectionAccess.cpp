#pragma once
#include "CrossCollectionAccess.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCommunication/Msg.h"
#include "DataBase.h"
#include "OpenTwinCore/FolderNames.h"
#include "ClassFactory.h"
#include "OpenTwinCore/ReturnMessage.h"


CrossCollectionAccess::CrossCollectionAccess(const std::string& projectName, const std::string& sessionServiceURL, const std::string& modelServiceURL)
	:_projectName(projectName), _modelServiceURL(modelServiceURL)
{
	std::string authorisationURL =	InquireAuthorisationURL(sessionServiceURL);
	InquireProjectCollection(authorisationURL);
}

std::list<std::shared_ptr<EntityMeasurementMetadata>> CrossCollectionAccess::getMeasurementMetadata(ot::components::ModelComponent* modelComponent)
{
	EntityMeasurementMetadata temp(0, nullptr, nullptr, nullptr, nullptr, "");
	std::pair<ot::UIDList, ot::UIDList> entityIdentifier = InquireMetadataEntityIdentifier(temp.getClassName());
	
	ot::UIDList& entityIDs = entityIdentifier.first;
	ot::UIDList& entityVersions = entityIdentifier.second;

	std::list<std::shared_ptr<EntityMeasurementMetadata>> measurementMetadata;
	ClassFactory classFactory;
	auto entityVersion = entityVersions.begin();
	DataBaseWrapper wrapper(_collectionName);
	
	for(auto entityID= entityIDs.begin(); entityID != entityIDs.end(); entityID++)
	{
		auto baseEnt = modelComponent->readEntityFromEntityIDandVersion(*entityID, *entityVersion, classFactory);
		const std::shared_ptr<EntityMeasurementMetadata> metadata(dynamic_cast<EntityMeasurementMetadata*>(baseEnt));
		assert(metadata != nullptr);
		measurementMetadata.push_back(metadata);
		
		entityVersion++;
	}
	return measurementMetadata;
}

std::shared_ptr<EntityResearchMetadata>  CrossCollectionAccess::getMeasurementCampaignMetadata(ot::components::ModelComponent* modelComponent)
{
	EntityResearchMetadata temp(0, nullptr, nullptr, nullptr, nullptr, "");
	std::pair<ot::UIDList, ot::UIDList> entityIdentifier = InquireMetadataEntityIdentifier(temp.getClassName());
	if (entityIdentifier.first.size() == 1)
	{
		DataBaseWrapper wrapper(_collectionName);
		ot::UID& entityID = *entityIdentifier.first.begin();
		ot::UID& entityVersion = *entityIdentifier.second.begin();
		ClassFactory classFactory;

		auto baseEnt = modelComponent->readEntityFromEntityIDandVersion(entityID, entityVersion, classFactory);
		const std::shared_ptr<EntityResearchMetadata> metadata(dynamic_cast<EntityResearchMetadata*>(baseEnt));
		assert(metadata != nullptr);
		return metadata;
	}
	else
	{
		return std::shared_ptr<EntityResearchMetadata>(nullptr);
	}
}

std::string CrossCollectionAccess::InquireAuthorisationURL(const std::string& sessionServiceURL)
{
	OT_rJSON_createDOC(docLSS);
	ot::rJSON::add(docLSS, OT_ACTION_MEMBER, OT_ACTION_CMD_GetAuthorisationServerUrl);
	std::string autorisationURL;
	if (!ot::msg::send("", sessionServiceURL, ot::EXECUTE, ot::rJSON::toJSON(docLSS), autorisationURL))
	{
		throw std::exception("Timeout for requesting the session service.");
	}
	return autorisationURL;
}

void CrossCollectionAccess::InquireProjectCollection(const std::string& authorisationURL)
{
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_GET_PROJECT_DATA);
	ot::rJSON::add(doc, OT_PARAM_AUTH_LOGGED_IN_USERNAME, DataBase::GetDataBase()->getUserName());
	ot::rJSON::add(doc, OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, DataBase::GetDataBase()->getUserPassword());
	ot::rJSON::add(doc, OT_PARAM_AUTH_PROJECT_NAME, _projectName);

	std::string response;
	if (!ot::msg::send("", authorisationURL, ot::EXECUTE, ot::rJSON::toJSON(doc), response))
	{
		throw std::exception("Timeout for requesting the authorisation service.");
	}
	
	ot::ReturnMessage responseMessage(response);
	if (responseMessage.getStatus() == ot::ReturnStatus::Failed())
	{
		_collectionName = "";
	}
	else
	{
		OT_rJSON_parseDOC(responseDoc, responseMessage.getWhat().c_str());
		_collectionName = responseDoc[OT_PARAM_AUTH_PROJECT_COLLECTION].GetString();
	}
}

std::pair<ot::UIDList, ot::UIDList> CrossCollectionAccess::InquireMetadataEntityIdentifier(const std::string& className)
{
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_GET_ENTITIES_FROM_ANOTHER_COLLECTION);
	ot::rJSON::add(doc, OT_ACTION_PARAM_PROJECT_NAME, _projectName);
	ot::rJSON::add(doc, OT_ACTION_PARAM_COLLECTION_NAME, _collectionName);
	ot::rJSON::add(doc, OT_ACTION_PARAM_Folder, ot::FolderNames::DatasetFolder);
	ot::rJSON::add(doc, OT_ACTION_PARAM_Type, className);
	ot::rJSON::add(doc, OT_ACTION_PARAM_Recursive, true);
	
	std::string response;
	if (!ot::msg::send("", _modelServiceURL, ot::EXECUTE, ot::rJSON::toJSON(doc), response))
	{
		throw std::exception("Timeout for requesting the model service.");
	}

	if (response == "")
	{
		return std::pair<ot::UIDList, ot::UIDList>();
	}
	
	auto responseDoc = ot::rJSON::fromJSON(response);
	ot::UIDList entityIDs = ot::rJSON::getULongLongList(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
	ot::UIDList entityVersions = ot::rJSON::getULongLongList(responseDoc, OT_ACTION_PARAM_MODEL_EntityVersionList);
	return std::make_pair<>(entityIDs, entityVersions);
}

DataBaseWrapper::DataBaseWrapper(const std::string& collectionName)
	:_oldCollectionName(DataBase::GetDataBase()->getProjectName())
{
	DataBase::GetDataBase()->setProjectName(collectionName);
}

DataBaseWrapper::~DataBaseWrapper()
{
	DataBase::GetDataBase()->setProjectName(_oldCollectionName);
}
