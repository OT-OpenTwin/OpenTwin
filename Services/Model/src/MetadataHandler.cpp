#include "stdafx.h"
#include "MetadataHandler.h"
#include "Application.h"
#include "OTModelEntities/DataBase.h"
#include "CrossCollectionDatabaseWrapper.h"
#include "OTModelEntities/EntityAPI.h"
#include "OTResultDataAccess/MetadataHandle/MetadataEntityInterface.h"

MetadataCampaign MetadataHandler::getMetadataCampaign(const std::string& _projectName)
{
	if (m_projectToCollectionConverter == nullptr)
	{
		const std::string sessionServiceURL = Application::instance()->getSessionServiceURL();
		m_projectToCollectionConverter.reset(new ProjectToCollectionConverter(sessionServiceURL));
	}

	if (!_projectName.empty())
	{
		EntityMetadataSeries tempS;
		const std::string classNameSeries = tempS.getClassName();
		EntityMetadataCampaign tempC;
		const std::string classNameCampaign = tempC.getClassName();
		std::shared_ptr<EntityMetadataCampaign> campaignMetadataEntity;
		std::list<std::shared_ptr<EntityMetadataSeries>> measurementMetadataEntity;

		Model* model = Application::instance()->getModel();
		if (Application::instance()->getProjectName() != _projectName)
		{
			// We need to load another model state. List of projects is already reduced to those which the user has access to, so permission to the collection is guaranteed.
			const std::string userName = Application::instance()->getLogInUserName();
			const std::string pswd = Application::instance()->getLogInUserPassword();
			
			// Getting information of session service. Potential to reduce communication with buffer.
			const std::string collectionName =	m_projectToCollectionConverter->nameCorrespondingCollection(_projectName,userName,pswd);
			std::string actualOpenedProject = DataBase::instance().getCollectionName();
			CrossCollectionDatabaseWrapper wrapper(collectionName);
			ModelState secondary(model->getSessionCount(), static_cast<unsigned int>(model->getServiceID()));
			secondary.openProject();

			//Load entire modelState
			std::list<ot::UID> prefetchIds;
			secondary.getListOfTopologyEntites(prefetchIds);
			std::list<std::pair<ot::UID, ot::UID>> prefetchIdAndVersion;
			for (auto id : prefetchIds)
			{
				ot::UID entityID = id;
				ot::UID entityVersion = secondary.getCurrentEntityVersion(entityID);
				prefetchIdAndVersion.push_back(std::pair<ot::UID, ot::UID>(entityID, entityVersion));
			}
			DataBase::instance().prefetchDocumentsFromStorage(prefetchIdAndVersion);
		
			for (auto& identifier : prefetchIdAndVersion)
			{
				std::unique_ptr<EntityBase> baseEnt;
				baseEnt.reset(ot::EntityAPI::readEntityFromEntityIDandVersion(identifier.first, identifier.second));

				if (baseEnt != nullptr)
				{
					if (baseEnt->getClassName() == classNameCampaign)
					{
						EntityMetadataCampaign* campaign = dynamic_cast<EntityMetadataCampaign*>(baseEnt.release());
						campaignMetadataEntity.reset(campaign);
					}
					else if (baseEnt->getClassName() == classNameSeries)
					{
						EntityMetadataSeries* series = dynamic_cast<EntityMetadataSeries*>(baseEnt.release());
						measurementMetadataEntity.push_back(std::shared_ptr<EntityMetadataSeries>(series));
					}
					else
					{
						assert(false);
					}
				}
			}
		}
		else
		{
			ot::UIDList entityIDs = model->getIDsOfFolderItems(ot::FolderNames::DatasetFolder,true);
			model->prefetchDocumentsFromStorage(entityIDs);
			for (ot::UID entityID : entityIDs)
			{
				std::unique_ptr<EntityBase> baseEnt;
				baseEnt.reset(model->readEntityFromEntityID(nullptr, entityID, model->getAllEntitiesByUID()));
				
				assert(baseEnt != nullptr);
				if (baseEnt->getClassName() == classNameSeries)
				{
					EntityMetadataSeries* series = dynamic_cast<EntityMetadataSeries*>(baseEnt.release());
					measurementMetadataEntity.push_back(std::shared_ptr<EntityMetadataSeries>(series));
				}
				else if(baseEnt->getClassName() == classNameCampaign)
				{
					EntityMetadataCampaign* campaign = dynamic_cast<EntityMetadataCampaign*>(baseEnt.release());
					campaignMetadataEntity.reset(campaign);
				}
				else
				{
					assert(false);
				}
			}
			
		}

		MetadataEntityInterface campaignFactory;
		MetadataCampaign campaign = campaignFactory.createCampaign(campaignMetadataEntity, measurementMetadataEntity);
		return campaign;
	}
	else
	{
		return MetadataCampaign();
	}
}
