#include "stdafx.h"
#include "MetadataHandler.h"
#include "Application.h"
#include "OTModelEntities/DataBase.h"
#include "CrossCollectionDatabaseWrapper.h"
#include "OTModelEntities/EntityAPI.h"
#include "OTModelEntities/MetadataEntityInterface.h"
#include "OTResultDataAccess/DataLakeAccessor.h"
#include <chrono>

MetadataCampaign MetadataHandler::getMetadataCampaign(const std::string& _projectName, std::string& _collectionName)
{
	if (m_projectToCollectionConverter == nullptr)
	{
		const std::string sessionServiceURL = Application::instance()->getSessionServiceURL();
		m_projectToCollectionConverter.reset(new ProjectToCollectionConverter(sessionServiceURL));
	}
	std::map<std::string, uint64_t> timings;
	if (!_projectName.empty())
	{
		const std::string classNameSeries = EntityMetadataSeries::className();
		const std::string classNameCampaign = EntityMetadataCampaign::className();

		EntityMetadataCampaign* campaignMetadataEntity = nullptr;
		std::list<EntityMetadataSeries*> measurementMetadataEntity;
		std::list<std::unique_ptr<EntityBase>> garbage;

		Model* model = Application::instance()->getModel();
		auto start = std::chrono::high_resolution_clock::now();
		if (Application::instance()->getProjectName() != _projectName)
		{
			// We need to load another model state. List of projects is already reduced to those which the user has access to, so permission to the collection is guaranteed.
			const std::string databaseUserName = Application::instance()->getDataBaseUserName();
			const std::string databaseUserPwd = Application::instance()->getDataBaseUserPassword();
			
			// Getting information of session service. Potential to reduce communication with buffer.
			_collectionName = m_projectToCollectionConverter->nameCorrespondingCollection(_projectName,databaseUserName, databaseUserPwd);
			std::string actualOpenedProject = DataBase::instance().getCollectionName();
			auto startModelState = std::chrono::high_resolution_clock::now();
			CrossCollectionDatabaseWrapper wrapper(_collectionName);
			ModelState secondary(model->getSessionCount(), static_cast<unsigned int>(model->getServiceID()));
			ModelState::VersionInformation information;
			secondary.getActiveModelState(information);
			secondary.loadModelState(information.version, false);
			auto modelStateLoaded = std::chrono::high_resolution_clock::now();

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
			auto entitiesLoaded = std::chrono::high_resolution_clock::now();

			for (auto& identifier : prefetchIdAndVersion)
			{
				try
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
				catch (std::exception _e)
				{
					OT_LOG_E("Failed to load entity from another project.");
				}
			}
			auto filtered = std::chrono::high_resolution_clock::now();

			uint64_t modelSTateTime = std::chrono::duration_cast<std::chrono::milliseconds>(modelStateLoaded - startModelState).count();
			uint64_t entityLoadedTime = std::chrono::duration_cast<std::chrono::milliseconds>(entitiesLoaded - modelStateLoaded).count();
			uint64_t filteredTime = std::chrono::duration_cast<std::chrono::milliseconds>( filtered - entitiesLoaded ).count();
			timings["ModelStateLoaded"] = modelSTateTime;
			timings["entityLoaded"] = entityLoadedTime;
			timings["filteredTime"] = filteredTime;

		}
		else
		{
			_collectionName = Application::instance()->getCollectionName();
			ot::UIDList entityIDs = model->getIDsOfFolderItems(ot::FolderNames::DatasetFolder, true);
			Model* model =	Application::instance()->getModel();
			
			auto entLoaded = std::chrono::high_resolution_clock::now();

			for (ot::UID entityID : entityIDs)
			{
				EntityBase* baseEnt = model->getEntityByID(entityID);
				
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
			auto filtered = std::chrono::high_resolution_clock::now();
			uint64_t filteredTime = std::chrono::duration_cast<std::chrono::milliseconds>(filtered - entLoaded).count();
			timings["FilterTime"] = filteredTime;

		}
		auto loaded = std::chrono::high_resolution_clock::now();
		if (!campaignMetadataEntity)
		{
			OT_LOG_E("Campaign metadata entity for for project \"" + _projectName + "\" not found");
			return MetadataCampaign();
		}

		MetadataEntityInterface campaignFactory;
		auto startCreated = std::chrono::high_resolution_clock::now();
		MetadataCampaign campaign = campaignFactory.createCampaign(campaignMetadataEntity, measurementMetadataEntity);
		auto created = std::chrono::high_resolution_clock::now();
		
		uint64_t totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(created - start).count();
		uint64_t creationTime = std::chrono::duration_cast<std::chrono::milliseconds>(created - startCreated).count();
		timings["CreateCampaign"] = creationTime;
		timings["total"] = totalTime;
		return campaign;
	}
	else
	{
		return MetadataCampaign();
	}
}

ot::DataLakeAccessCfg MetadataHandler::createConfig(const MetadataCampaign& _campaign, const std::string& _collectionName, const DataLakeQueryCfg& _queryCfg)
{
	DataLakeAccessor dataLakeAccessor(Application::instance());
	dataLakeAccessor.accessPartition(_campaign,_collectionName);
	dataLakeAccessor.createQueryDescriptionQuantity(_queryCfg.getValueDescriptionQuantities());
	dataLakeAccessor.createQueryDescriptionsParameter(_queryCfg.getValueDescriptionParameters());
	dataLakeAccessor.createQueryDescriptionsSeries(_queryCfg.getValueDescriptionSeriesMD(), _queryCfg.getSeriesLabel());
	ot::DataLakeAccessCfg accessConfig = dataLakeAccessor.createConfig();
	return accessConfig;
}
