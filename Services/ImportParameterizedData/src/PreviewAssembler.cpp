#include "PreviewAssembler.h"
#include "Application.h"
#include "ClassFactory.h"


std::shared_ptr<EntityParameterizedDataPreviewTable> PreviewAssembler::AssembleTable(ot::UIDList& existingRanges)
{
	LoadSelectedRangesAndTableSources(existingRanges);
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
	std::list<std::string> tableSources;
	for (ot::UID existingRange : existingRanges)
	{
		auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(existingRange, Application::instance()->getPrefetchedEntityVersion(existingRange), Application::instance()->getClassFactory());
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
	for (std::string tableName : tableNames)
	{
		bool referencedTableExists = false;
		for (auto entityInfo : entityInfos)
		{
			
			if (entityInfo.getEntityName() == tableName)
			{
				auto baseEnt =	_modelComponent->readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), Application::instance()->getClassFactory());
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
	
	_numberOfFields = 0;
	for (size_t i = 0; i < _selectedRangeEntities.size(); i++)
	{
		ot::TableRange selectedRange =	_selectedRangeEntities[i]->getSelectedRange();
		if (_selectedRangeEntities[i]->getTableOrientation() == ot::TableHeaderOrientation::horizontal)
		{
			_numberOfFields += selectedRange.getRightColumn() - selectedRange.getLeftColumn() + 1;
		}
		else
		{
			_numberOfFields += selectedRange.getBottomRow() - selectedRange.getTopRow() + 1;
		}
	}
}