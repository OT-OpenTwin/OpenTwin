//#pragma once
//#include <vector>
//#include <string>
//#include <memory>
//
//#include <openTwin/components/ModelComponent.h>
//#include "EntityParameterizedDataCategorization.h"
//#include "EntityTableSelectedRanges.h"
//#include "EntityParameterizedDataTable.h"
//#include "EntityParameterizedDataMetadata.h"
//
//using CategorizationVector = std::vector<std::shared_ptr<EntityParameterizedDataCategorization>>;
//using Categorization = std::shared_ptr<EntityParameterizedDataCategorization>;
//using SourceTable = std::shared_ptr<EntityParameterizedDataTable>;
//using SelectedRanges = std::shared_ptr<EntityTableSelectedRanges>;
//
//class CollectionAssembler
//{
//public:
//	CollectionAssembler(ot::components::ModelComponent* modelComponent, std::string tableFolder);
//
//	void CreateCollection(Categorization categorizationRMD, CategorizationVector categorizationMSMDs, CategorizationVector categorizationParameters, CategorizationVector categorizationQuantities, std::list<std::pair<UID, UID>> allExistingRanges);
//
//private:	
//	ot::components::ModelComponent* _modelComponent;
//	std::string _tableFolder;
//
//	std::map<std::string, SelectedRanges> _allRangeEntitiesByName;
//	std::map < std::string, SourceTable> _allTableEntitiesByName;
//
//	void LoadAllRangeEntitiesByName(std::list<std::pair<UID, UID>> allRanges);
//	void LoadAllTableSourcesByName(std::list<std::string> tableNames);
//	std::map<std::string, std::list<std::string>> ExtractFieldsFromRange(std::string rangeName);
//	
//	void FillMetadataWithFields(EntityParameterizedDataMetadata* metaData, std::map<std::string, std::pair<std::string, std::list<std::string>>>& allFields);
//};
//
