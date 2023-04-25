//#include "CollectionAssembler.h"
//#include "Application.h"
//
//#include "ClassFactory.h"
//
//
//CollectionAssembler::CollectionAssembler(ot::components::ModelComponent * modelComponent, std::string tableFolder)
//	:_modelComponent(modelComponent), _tableFolder(tableFolder)
//{}
//
//void CollectionAssembler::CreateCollection(Categorization categorizationRMD, CategorizationVector categorizationMSMDs, CategorizationVector categorizationParameters, CategorizationVector categorizationQuantities, std::list<std::pair<UID, UID>> allExistingRanges)
//{
//	LoadAllRangeEntitiesByName(allExistingRanges);
//
//	std::list<std::string> allRangesNames;
//	for (auto rangeByName : _allRangeEntitiesByName)
//	{
//		allRangesNames.push_back(rangeByName.first);
//	}
//
//		
//	std::unique_ptr<EntityParameterizedDataMetadata> rmdEntity(new EntityParameterizedDataMetadata(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
//	rmdEntity->setMetadataCategory(EntityParameterizedDataMetadata::MetadataCategory::RMD);
//
//	//ToDo: Loading tables for each of the metadata entities after another
//	//Fill RMD first
//	EntityParameterizedDataMetadata* currentMetaData;
//	currentMetaData = rmdEntity.get();
//	std::map<std::string, std::pair<std::string,std::list<std::string>>> allFields;
//	for (std::string rangeName : allRangesNames)
//	{
//		std::string::difference_type n = std::count(rangeName.begin(), rangeName.end(), '/');
//		if (n == 2)
//		{
//			std::map<std::string, std::list<std::string>> extractedFields = ExtractFieldsFromRange(rangeName);
//			
//			allRangesNames.remove(rangeName);
//		}
//	}
//
//	//Fill each msmd after another
//	std::map<std::string, std::unique_ptr<EntityParameterizedDataMetadata>> msmdEntities;
//	allFields.clear();
//	currentMetaData = nullptr;
//	bool foundMSMDAssociatedRange = false;
//	/*while ()*/
//	for (std::string rangeName : allRangesNames)
//	{
//		std::string::difference_type n = std::count(rangeName.begin(), rangeName.end(), '/');
//		if (n == 3)
//		{
//			std::string msmdName = rangeName.substr(0, rangeName.find_last_of("/"));
//			if (msmdEntities.find(msmdName) == msmdEntities.end())
//			{
//				msmdEntities[msmdName] = std::make_unique<EntityParameterizedDataMetadata>(new EntityParameterizedDataMetadata(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
//				msmdEntities[msmdName]->setMetadataCategory(EntityParameterizedDataMetadata::MetadataCategory::MSMD);
//			}
//			currentMetaData = msmdEntities[msmdName].get();
//			currentMetaData = rmdEntity.get();
//			std::map<std::string, std::list<std::string>> extractedFields = ExtractFieldsFromRange(rangeName);
//			for (auto extractedField : extractedFields)
//			{
//				std::string type = _allRangeEntitiesByName[rangeName]->getSelectedType();
//				if (allFields.find(extractedField.first) != allFields.end())
//				{
//					if (allFields[extractedField.first].first != type)
//					{
//						throw std::exception(("Range Entity " + rangeName + " has an overlap with another rangeselection of a different type.").c_str());
//					}
//					allFields[extractedField.first].second.merge(extractedField.second);
//				}
//				else
//				{
//					allFields[extractedField.first] = std::make_pair<>(type, extractedField.second);
//				}
//			}
//		}
//	}
//
//
//	//
//	//	
//	//	
//	//			
//	//	if (n == 2)
//	//	{
//	//		
//	//	}
//	//	else if (n == 3)
//	//	{
//
//	//	}
//
//	//	for (auto field : extractedFields)
//	//	{
//	//		
//	//	}
//
//	//}
//}
//
//void CollectionAssembler::LoadAllRangeEntitiesByName(std::list<std::pair<UID, UID>> allExistingRanges)
//{
//	UIDList existingRangesIDs;
//	for (auto existingRange : allExistingRanges)
//	{
//		existingRangesIDs.push_back(existingRange.first);
//	}
//	Application::instance()->prefetchDocumentsFromStorage(existingRangesIDs);
//	std::list<std::string> namesOfTableSources;
//
//	ClassFactory classFactory;
//	for (UID existingRange : existingRangesIDs)
//	{
//		auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(existingRange, Application::instance()->getPrefetchedEntityVersion(existingRange), classFactory);
//		std::shared_ptr<EntityTableSelectedRanges> rangeEntity(dynamic_cast<EntityTableSelectedRanges*>(baseEnt));
//		if (rangeEntity == nullptr)
//		{
//			assert(0);
//		}
//		
//		_allRangeEntitiesByName[rangeEntity->getName()]=rangeEntity;
//		namesOfTableSources.push_back(rangeEntity->getTableName());
//	}
//	namesOfTableSources.unique();
//
//	LoadAllTableSourcesByName(namesOfTableSources);
//}
//
//void CollectionAssembler::LoadAllTableSourcesByName(std::list<std::string> tableNames)
//{
//	std::list<std::string> allTables = _modelComponent->getListOfFolderItems(_tableFolder);
//	std::list<ot::EntityInformation> entityInfos;
//	_modelComponent->getEntityInformation(allTables, entityInfos);
//	ClassFactory classFactory;
//
//	for (std::string tableName : tableNames)
//	{
//		bool referencedTableExists = false;
//		for (auto entityInfo : entityInfos)
//		{
//			if (entityInfo.getName() == tableName)
//			{
//				auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
//				std::shared_ptr<EntityParameterizedDataTable> sourceTable(dynamic_cast<EntityParameterizedDataTable*>(baseEnt));
//				if (sourceTable == nullptr)
//				{
//					assert(0);
//				}
//				sourceTable->getTableData(); //Must be done, since the classfactory pointer doesn't exist anymore when the tabledata is actually needed.
//				_allTableEntitiesByName[sourceTable->getName()] = sourceTable;
//				referencedTableExists = true;
//				break;
//			}
//		}
//		if (!referencedTableExists)
//		{
//			throw std::exception(("Table " + tableName + " is being regerenced but does not exist.").c_str());
//		}
//	}
//}
//
//
//
//void CollectionAssembler::FillMetadataWithFields(EntityParameterizedDataMetadata * metaData, std::map<std::string, std::pair<std::string, std::list<std::string>>>& allFields)
//{

//}
