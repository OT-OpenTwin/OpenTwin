#include "stdafx.h"
#include "MetadataHandler.h"
#include "Application.h"
#include "OTModelEntities/DataBase.h"
#include "CrossCollectionDatabaseWrapper.h"
#include "OTModelEntities/EntityAPI.h"
#include "OTModelEntities/MetadataEntityInterface.h"

MetadataCampaign MetadataHandler::getMetadataCampaign(const std::string& _projectName, std::string& _collectionName)
{
	if (m_projectToCollectionConverter == nullptr)
	{
		const std::string sessionServiceURL = Application::instance()->getSessionServiceURL();
		m_projectToCollectionConverter.reset(new ProjectToCollectionConverter(sessionServiceURL));
	}

	if (!_projectName.empty())
	{
		const std::string classNameSeries = EntityMetadataSeries::className();
		const std::string classNameCampaign = EntityMetadataCampaign::className();

		EntityMetadataCampaign* campaignMetadataEntity = nullptr;
		std::list<EntityMetadataSeries*> measurementMetadataEntity;
		std::list<std::unique_ptr<EntityBase>> garbage;

		Model* model = Application::instance()->getModel();
		if (Application::instance()->getProjectName() != _projectName)
		{
			// We need to load another model state. List of projects is already reduced to those which the user has access to, so permission to the collection is guaranteed.
			const std::string userName = Application::instance()->getLogInUserName();
			const std::string pswd = Application::instance()->getLogInUserPassword();
			
			// Getting information of session service. Potential to reduce communication with buffer.
			_collectionName = m_projectToCollectionConverter->nameCorrespondingCollection(_projectName,userName,pswd);
			std::string actualOpenedProject = DataBase::instance().getCollectionName();
			CrossCollectionDatabaseWrapper wrapper(_collectionName);
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
				
				if (baseEnt == nullptr)
				{
					continue;
				}

				if (baseEnt->getClassName() == classNameCampaign)
				{
					EntityMetadataCampaign* campaign = dynamic_cast<EntityMetadataCampaign*>(baseEnt.get());
					OTAssertNullptr(campaign);
					campaignMetadataEntity = campaign;
				}
				else if (baseEnt->getClassName() == classNameSeries)
				{
					EntityMetadataSeries* series = dynamic_cast<EntityMetadataSeries*>(baseEnt.get());
					OTAssertNullptr(series);
					measurementMetadataEntity.push_back(series);
				}

				garbage.push_back(std::move(baseEnt));
			}
		}
		else
		{
			_collectionName = Application::instance()->getCollectionName();
			ot::UIDList entityIDs = model->getIDsOfFolderItems(ot::FolderNames::DatasetFolder, true);
			model->prefetchDocumentsFromStorage(entityIDs);
			for (ot::UID entityID : entityIDs)
			{
				EntityBase* baseEnt = model->readEntityFromEntityID(nullptr, entityID, model->getAllEntitiesByUID());
				
				assert(baseEnt != nullptr);
				if (baseEnt->getClassName() == classNameSeries)
				{
					EntityMetadataSeries* series = dynamic_cast<EntityMetadataSeries*>(baseEnt);
					measurementMetadataEntity.push_back(series);
				}
				else if(baseEnt->getClassName() == classNameCampaign)
				{
					EntityMetadataCampaign* campaign = dynamic_cast<EntityMetadataCampaign*>(baseEnt);
					campaignMetadataEntity = campaign;
				}
			}
		}

		if (!campaignMetadataEntity)
		{
			OT_LOG_E("Campaign metadata entity for for project \"" + _projectName + "\" not found");
			return MetadataCampaign();
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
