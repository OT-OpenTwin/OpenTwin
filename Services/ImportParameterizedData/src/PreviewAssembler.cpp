// @otlicense
// File: PreviewAssembler.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "PreviewAssembler.h"
#include "Application.h"

#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

void PreviewAssembler::LoadSelectedRangesAndTableSources(ot::UIDList & existingRanges)
{
	Application::instance()->prefetchDocumentsFromStorage(existingRanges);
	std::list<std::string> tableSources;
	for (ot::UID existingRange : existingRanges)
	{
		auto baseEnt = ot::EntityAPI::readEntityFromEntityIDandVersion(existingRange, Application::instance()->getPrefetchedEntityVersion(existingRange));
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
	std::list<std::string> tableSources = ot::ModelServiceAPI::getListOfFolderItems(_tableFolder);
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(tableSources, entityInfos);
	for (std::string tableName : tableNames)
	{
		bool referencedTableExists = false;
		for (auto entityInfo : entityInfos)
		{
			
			if (entityInfo.getEntityName() == tableName)
			{
				auto baseEnt = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion());
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
		if (_selectedRangeEntities[i]->getTableHeaderMode() == ot::TableCfg::TableHeaderMode::Horizontal)
		{
			_numberOfFields += selectedRange.getRightColumn() - selectedRange.getLeftColumn() + 1;
		}
		else
		{
			_numberOfFields += selectedRange.getBottomRow() - selectedRange.getTopRow() + 1;
		}
	}
}