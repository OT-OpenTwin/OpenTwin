#include "MeasurementCampaignHandler.h"
#include "Application.h"
#include "ClassFactory.h"

void MeasurementCampaignHandler::ConnectToCollection(const std::string& collectionName, const std::string& projectName)
{
	EntityMeasurementMetadata temp(0, nullptr, nullptr, nullptr, nullptr, "");

	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_TEMP);
	ot::rJSON::add(doc, OT_ACTION_PARAM_PROJECT_NAME, projectName);
	ot::rJSON::add(doc, OT_ACTION_PARAM_COLLECTION_NAME, collectionName);
	ot::rJSON::add(doc, OT_ACTION_PARAM_Folder, "Dataset");
	ot::rJSON::add(doc, OT_ACTION_PARAM_Type, "EntityMeasurementMetadata");
	ot::rJSON::add(doc, OT_ACTION_PARAM_Recursive, true);

	std::string returnValue = Application::instance()->sendMessage(false, OT_INFO_SERVICE_TYPE_MODEL, doc);
	// neuen ModelState erschaffen
	// in undoRedo steht nichts weiteres als ein ModelState .reset
	// Model dahingehend �ndern, dass der Modelstate tempor�r gewechselt werden kann (Achtung problem mit parallelen Zugriff!)
	ot::UIDList allMetadata = _modelComponent->getIDsOfFolderItemsOfType("/", temp.getClassName(),true);
	Application::instance()->prefetchDocumentsFromStorage(allMetadata);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(allMetadata, entityInfos);
	_measurementMetadata.clear();
	_measurementMetadata.reserve(entityInfos.size());

	ClassFactory classFactory;
	for (auto& entityInfo : entityInfos)
	{
		auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
		auto metadata = dynamic_cast<EntityMeasurementMetadata*>(baseEnt);
		assert(metadata != nullptr);
		_measurementMetadata.push_back(std::shared_ptr<EntityMeasurementMetadata>(metadata));
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
