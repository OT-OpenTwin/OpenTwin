#pragma once
#include "CrossCollectionAccess.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "DataBase.h"
#include "OTCore/FolderNames.h"
#include "ClassFactory.h"
#include "OTCore/ReturnMessage.h"

class ClassFactory;


CrossCollectionAccess::CrossCollectionAccess(const std::string& projectName, const std::string& sessionServiceURL, const std::string& modelServiceURL)
	:_projectName(projectName), _modelServiceURL(modelServiceURL)
{
	std::string authorisationURL =	InquireAuthorisationURL(sessionServiceURL);
	InquireProjectCollection(authorisationURL);
}

std::list<std::shared_ptr<EntityMetadataSeries>> CrossCollectionAccess::getMeasurementMetadata(ot::components::ModelComponent& modelComponent, ClassFactory* classFactory)
{
	EntityMetadataSeries temp(0, nullptr, nullptr, nullptr, nullptr, "");
	std::pair<ot::UIDList, ot::UIDList> entityIdentifier = InquireMetadataEntityIdentifier(temp.getClassName());
	
	ot::UIDList& entityIDs = entityIdentifier.first;
	ot::UIDList& entityVersions = entityIdentifier.second;

	std::list<std::shared_ptr<EntityMetadataSeries>> measurementMetadata;
	auto entityVersion = entityVersions.begin();
	DataBaseWrapper wrapper(_collectionName);
	
	for(auto entityID= entityIDs.begin(); entityID != entityIDs.end(); entityID++)
	{
		auto baseEnt = modelComponent.readEntityFromEntityIDandVersion(*entityID, *entityVersion, *classFactory);
		const std::shared_ptr<EntityMetadataSeries> metadata(dynamic_cast<EntityMetadataSeries*>(baseEnt));
		assert(metadata != nullptr);
		measurementMetadata.push_back(metadata);
		
		entityVersion++;
	}
	return measurementMetadata;
}

std::shared_ptr<EntityMetadataCampaign>  CrossCollectionAccess::getMeasurementCampaignMetadata(ot::components::ModelComponent& modelComponent, ClassFactory* classFactory)
{
	EntityMetadataCampaign temp(0, nullptr, nullptr, nullptr, nullptr, "");
	std::pair<ot::UIDList, ot::UIDList> entityIdentifier = InquireMetadataEntityIdentifier(temp.getClassName());
	if (entityIdentifier.first.size() == 1)
	{
		DataBaseWrapper wrapper(_collectionName);
		ot::UID& entityID = *entityIdentifier.first.begin();
		ot::UID& entityVersion = *entityIdentifier.second.begin();

		auto baseEnt = modelComponent.readEntityFromEntityIDandVersion(entityID, entityVersion, *classFactory);
		const std::shared_ptr<EntityMetadataCampaign> metadata(dynamic_cast<EntityMetadataCampaign*>(baseEnt));
		assert(metadata != nullptr);
		return metadata;
	}
	else
	{
		return std::shared_ptr<EntityMetadataCampaign>(nullptr);
	}
}

std::string CrossCollectionAccess::InquireAuthorisationURL(const std::string& sessionServiceURL)
{
	ot::JsonDocument docLSS;
	docLSS.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetAuthorisationServerUrl, docLSS.GetAllocator()), docLSS.GetAllocator());
	std::string autorisationURL;
	if (!ot::msg::send("", sessionServiceURL, ot::EXECUTE, docLSS.toJson(), autorisationURL))
	{
		throw std::exception("Timeout for requesting the session service.");
	}
	return autorisationURL;
}

void CrossCollectionAccess::InquireProjectCollection(const std::string& authorisationURL)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_PROJECT_DATA, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(DataBase::GetDataBase()->getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(DataBase::GetDataBase()->getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(_projectName, doc.GetAllocator()), doc.GetAllocator());
	
	std::string response;
	if (!ot::msg::send("", authorisationURL, ot::EXECUTE, doc.toJson(), response))
	{
		throw std::exception("Timeout for requesting the authorisation service.");
	}
	
	ot::ReturnMessage responseMessage = ot::ReturnMessage::fromJson(response);
	if (responseMessage == ot::ReturnMessage::Failed)
	{
		_collectionName = "";
	}
	else
	{
		ot::JsonDocument responseDoc;
		responseDoc.fromJson(responseMessage.getWhat());
		_collectionName = ot::json::getString(responseDoc, OT_PARAM_AUTH_PROJECT_COLLECTION);
	}
}

std::pair<ot::UIDList, ot::UIDList> CrossCollectionAccess::InquireMetadataEntityIdentifier(const std::string& className)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_GET_ENTITIES_FROM_ANOTHER_COLLECTION, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(_projectName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(_collectionName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Folder, ot::JsonString(ot::FolderNames::DatasetFolder, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Type, ot::JsonString(className, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Recursive, true, doc.GetAllocator());
		
	std::string response;
	if (!ot::msg::send("", _modelServiceURL, ot::EXECUTE, doc.toJson(), response))
	{
		throw std::exception("Timeout for requesting the model service.");
	}

	if (response == "")
	{
		return std::pair<ot::UIDList, ot::UIDList>();
	}
	
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);
	
	ot::UIDList entityIDs = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
	ot::UIDList entityVersions = ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityVersionList);
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
