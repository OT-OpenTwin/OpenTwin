/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

// Service header
#include "Application.h"
#include "ModelNotifier.h"
#include "UiNotifier.h"
#include "DataSourceManager.h"

// Open twin header
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"		// action member and types definition
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityAPI.h"

// Application specific includes
#include "EntityVis2D3D.h"
#include "EntityBinaryData.h"
#include "TemplateDefaultManager.h"
#include "DataBase.h"
#include "VtkDriverFactory.h"
#include "VtkDriver.h"
#include "ClassFactory.h"

#include <thread>

#include <map>

// The name of this service
#define MY_SERVICE_NAME OT_INFO_SERVICE_TYPE_VisualizationService

// The type of this service
#define MY_SERVICE_TYPE OT_INFO_SERVICE_TYPE_VisualizationService

Application::Application()
	: ot::ApplicationBase(MY_SERVICE_NAME, MY_SERVICE_TYPE, new UiNotifier(), new ModelNotifier()), visualizationModelID(0) 
{

}

Application::~Application()
{

}

// ##################################################################################################################################

// Required functions

void Application::run(void)
{
	// This method is called once the service can start its operation
	if (EnsureDataBaseConnection())
	{
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();
	}
	// Add code that should be executed when the service is started and may start its work
}

std::string Application::processAction(const std::string & _action, ot::JsonDocument& _doc)
{
	if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
	{
		std::string action = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
	
		assert(0); // Unhandled button action
	}
	else if (_action == OT_ACTION_CMD_MODEL_PropertyChanged)
	{
		std::list<ot::UID> entityIDs      = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityIDList);
		std::list<ot::UID> entityVersions = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_EntityVersionList);
		bool itemsVisible			      = ot::json::getBool(_doc, OT_ACTION_PARAM_MODEL_ItemsVisible);

		updateEntities(entityIDs, entityVersions, itemsVisible);
	}
	else {
		return OT_ACTION_RETURN_UnknownAction;
	}
	return "";
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument& _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	//enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);
	//_ui->registerForModelEvents();
	//_ui->addMenuPage("Post Processing");

	//_ui->addMenuGroup("Post Processing", "Visualization");

	//ot::flags<ot::ui::lockType> modelWrite;
	//modelWrite.setFlag(ot::ui::lockType::tlModelWrite);

	//_ui->addMenuButton("Post Processing", "Visualization", "Add 2D Plot", "Add 2D Plot", modelWrite, "Add 2D Plot", "Default");
	
	//enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{

}

void Application::modelDisconnected(const ot::components::ModelComponent * _model)
{

}

void Application::serviceConnected(ot::ServiceBase * _service)
{

}

void Application::serviceDisconnected(const ot::ServiceBase * _service)
{

}

void Application::preShutdown(void) {

}

void Application::shuttingDown(void)
{

}

bool Application::startAsRelayService(void) const
{
	return false;	// Do not want the service to start a relay service. Otherwise change to true
}

// ##################################################################################################################################

void Application::EnsureVisualizationModelIDKnown(void)
{
	if (visualizationModelID > 0) return;
	if (m_modelComponent == nullptr) {
		assert(0); throw std::exception("Model not connected");
	}
	
	// The visualization model isnot known yet -> get it from the model
	visualizationModelID = ot::ModelServiceAPI::getCurrentVisualizationModelID();
}

void Application::updateEntities(std::list<ot::UID> &entityIDs, std::list<ot::UID> &entityVersions, bool itemsVisible)
{
	std::list<std::pair<unsigned long long, unsigned long long>> entitiesToUpdate;

	while (!entityIDs.empty())
	{
		ot::UID entityID      = entityIDs.front();
		ot::UID entityVersion = entityVersions.front();

		entityIDs.pop_front();
		entityVersions.pop_front();

		entitiesToUpdate.push_back(std::pair<ot::UID, ot::UID>(entityID, entityVersion));
	}

	DataBase::GetDataBase()->PrefetchDocumentsFromStorage(entitiesToUpdate);

	for (auto entity : entitiesToUpdate)
	{
		updateSingleEntity(entity.first, entity.second, itemsVisible);
	}

}

std::pair<ot::UID, ot::UID> Application::createDataItems(EntityVis2D3D *visEntity)
{
	// Update the visualization -> Create a new visualization data object
	VtkDriver *vtkDriver = VtkDriverFactory::createDriver(visEntity);
	if (vtkDriver == nullptr)
	{
		throw std::exception();
	}

	vtkDriver->setProperties(visEntity);

	// Load the data item
	DataSourceManagerItem *dataItem = DataSourceManager::getDataItem(visEntity->getSourceID(), visEntity->getSourceVersion(), visEntity->getMeshID(), visEntity->getMeshVersion(), m_modelComponent, &getClassFactory());

	// Now buld the osg node and convert it to a string
	std::string nodeString = vtkDriver->buildSceneNode(dataItem);

	//if (vtkDriver->GetUpdateTopoEntityID().size() != 0)
	//{
	//	std::list<UID> topologyEntityIDList, topologyEntityVersionList, dataEntityIDList, dataEntityVersionList, dataEntityParentList;
	//	std::list<bool> topologyEntityForceVisible;
	//	topologyEntityIDList = vtkDriver->GetUpdateTopoEntityID();
	//	topologyEntityVersionList = vtkDriver->GetUpdateTopoEntityVersion();
	//	topologyEntityForceVisible = vtkDriver->GetUpdateTopoEntityForceVisible();
	//	m_modelComponent->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible, dataEntityIDList, dataEntityVersionList, dataEntityParentList,"vtk triggered property updates.");
	//}

	// Delete the driver
	delete vtkDriver;
	vtkDriver = nullptr;

	// Now we store the data in binary data items
	return storeBinaryData(nodeString.c_str(), nodeString.size()+1);
}

void Application::updateSingleEntity(ot::UID entityID, ot::UID entityVersion, bool itemsVisible)
{
	EntityVis2D3D *visEntity = dynamic_cast<EntityVis2D3D*>(ot::EntityAPI::readEntityFromEntityIDandVersion(entityID, entityVersion, getClassFactory()));
	if (visEntity == nullptr)
	{
		assert(0); // Wrong entity type of entity id / version
		return;
	}

	// Read all data from the entity
	std::pair<ot::UID, ot::UID> binaryDataItems;
	
	if (itemsVisible)
	{
		try
		{
			binaryDataItems = createDataItems(visEntity);
		}
		catch (std::exception)
		{
			uiComponent()->displayMessage("ERROR: Unable to create the data item.");
			return;
		}
	}

	// Send the information about the new visualization data to the model. The model then updates the EntityVis2D item and sends the update message to the viewer.
	sendNewVisualizationDataToModeler(visEntity, binaryDataItems.first, binaryDataItems.second);
}

std::pair<ot::UID, ot::UID> Application::storeBinaryData(const char *data, size_t dataLength)
{
	EntityBinaryData *dataItem = new EntityBinaryData(m_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, getServiceName());

	dataItem->setData(data, dataLength);
	dataItem->StoreToDataBase();

	return std::pair<ot::UID, ot::UID>(dataItem->getEntityID(), dataItem->getEntityStorageVersion());
}

void Application::sendNewVisualizationDataToModeler(EntityVis2D3D *visEntity, ot::UID binaryDataItemID, ot::UID binaryDataItemVersion)
{
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_UpdateVisualizationEntity, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, visEntity->getEntityID(), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion, visEntity->getEntityStorageVersion(), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataID, binaryDataItemID, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_MODEL_DataVersion, binaryDataItemVersion, requestDoc.GetAllocator());

	std::string tmp;
	sendMessage(false, OT_INFO_SERVICE_TYPE_MODEL, requestDoc, tmp);
}
