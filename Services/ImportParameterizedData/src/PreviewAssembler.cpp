#include "PreviewAssembler.h"
#include "Application.h"
#include "ClassFactory.h"


std::shared_ptr<EntityParameterizedDataPreviewTable> PreviewAssembler::AssembleTable(ot::UIDList& existingRanges)
{
	LoadSelectedRangesAndTableSources(existingRanges);
	ClassFactory classFactory;
	std::shared_ptr<EntityParameterizedDataPreviewTable> newPreviewTable(new EntityParameterizedDataPreviewTable(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
	std::shared_ptr<EntityResultTableData<std::string>> newPreviewTableData(new EntityResultTableData<std::string>(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
	//CountPreviewFieldsFromSelectedRanges();
	InitiatePreviewTable(newPreviewTableData);
	
	AddFieldsToTable(newPreviewTableData);


	newPreviewTableData->StoreToDataBase();
	newPreviewTable->setTableData(newPreviewTableData);

	return newPreviewTable;
}

void PreviewAssembler::LoadSelectedRangesAndTableSources(ot::UIDList & existingRanges)
{
	Application::instance()->prefetchDocumentsFromStorage(existingRanges);
	ClassFactory classFactory;
	std::list<std::string> tableSources;
	for (ot::UID existingRange : existingRanges)
	{
		auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(existingRange, Application::instance()->getPrefetchedEntityVersion(existingRange), classFactory);
		std::shared_ptr<EntityTableSelectedRanges> rangeEntity(dynamic_cast<EntityTableSelectedRanges*>(baseEnt));
		if (rangeEntity == nullptr)
		{
			assert(0);
		}
		_selectedRangeEntities.push_back(rangeEntity);
		tableSources.push_back(rangeEntity->getTableName());
	}
	tableSources.unique();

	LoadTableSources(tableSources);
}

void PreviewAssembler::LoadTableSources(std::list<std::string>& tableNames)
{
	std::list<std::string> tableSources = _modelComponent->getListOfFolderItems(_tableFolder);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(tableSources, entityInfos);
	ClassFactory classFactory;
	for (std::string tableName : tableNames)
	{
		bool referencedTableExists = false;
		for (auto entityInfo : entityInfos)
		{
			
			if (entityInfo.getName() == tableName)
			{
				auto baseEnt =	_modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
				std::shared_ptr<EntityParameterizedDataTable> sourceTable(dynamic_cast<EntityParameterizedDataTable*>(baseEnt));
				if (sourceTable == nullptr)
				{
					assert(0);
				}
				sourceTable->getTableData();
				_tableSources.push_back(sourceTable);
				referencedTableExists = true;
				break;
			}
		}
		if (!referencedTableExists)
		{
			//TODO: Action if the referred table is not being found.
		}
	}
}

void PreviewAssembler::CountPreviewFieldsFromSelectedRanges()
{
	uint32_t selectedRange[4];
	_numberOfFields = 0;
	for (size_t i = 0; i < _selectedRangeEntities.size(); i++)
	{
		_selectedRangeEntities[i]->getSelectedRange(selectedRange[0], selectedRange[1], selectedRange[2], selectedRange[3]);
		if (_selectedRangeEntities[i]->getTableOrientation() == EntityParameterizedDataTable::GetHeaderOrientation(EntityParameterizedDataTable::HeaderOrientation::horizontal))
		{
			_numberOfFields += selectedRange[3] - selectedRange[2] + 1;
		}
		else
		{
			_numberOfFields += selectedRange[1] - selectedRange[0] + 1;
		}
	}
}