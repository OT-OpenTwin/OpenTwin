// @otlicense
// File: PreviewAssemblerRMD.cpp
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

#include "PreviewAssemblerRMD.h"
#include "DataCategorizationHandler.h"
#include "OTGui/TableIndexSchemata.h"

void PreviewAssemblerRMD::InitiatePreviewTable(std::shared_ptr<EntityResultTableData<std::string>> previewTableData)
{
	previewTableData->setNumberOfRows(1);
	previewTableData->setNumberOfColumns(3);
	previewTableData->setValue(0, 0, "Field name");
	previewTableData->setValue(0, 1, "Number of values");
	previewTableData->setValue(0, 2, "Value(s)");
}

void PreviewAssemblerRMD::AddFieldsToTable(std::shared_ptr<EntityResultTableData<std::string>> previewTableData)
{
	std::map<std::string, std::list<std::string>> previewFields = CollectUniqueFields();
	
	uint64_t totalNumberOfRows = previewTableData->getNumberOfRows() + previewFields.size();
	previewTableData->setNumberOfRows(totalNumberOfRows);

	int rowPointer = 1;
	for (auto field : previewFields)
	{
		std::string valuePreview = "";
		auto it = field.second.begin();
		for (uint64_t j = 0; j < field.second.size(); j++)
		{
			if (j == 3)
			{
				valuePreview += ", [...]";
				break;
			}
			if (j != 0)
			{
				valuePreview += ", ";
			}
			valuePreview += *it;
			it++;
		}

		previewTableData->setValue(rowPointer, 0, field.first);
		previewTableData->setValue(rowPointer, 1, std::to_string(field.second.size()));
		previewTableData->setValue(rowPointer, 2, valuePreview);
		rowPointer++;
	}
}

std::map<std::string, std::list<std::string>> PreviewAssemblerRMD::CollectUniqueFields()
{
	std::map<std::string, std::list<std::string>> previewFields;
	for (size_t i = 0; i < _selectedRangeEntities.size(); i++)
	{

		for (auto sourceTable : _tableSources)
		{
			if (sourceTable->getName() == _selectedRangeEntities[i]->getTableName())
			{
				ot::TableRange userRange =  _selectedRangeEntities[i]->getSelectedRange();
				ot::TableCfg::TableHeaderMode headerOrientation = _selectedRangeEntities[i]->getTableHeaderMode();
				ot::TableRange matrixRange = ot::TableIndexSchemata::userRangeToMatrixRange(userRange,headerOrientation);
				uint32_t minColumn = static_cast<uint32_t>(matrixRange.getLeftColumn());
				uint32_t maxColumn = static_cast<uint32_t>(matrixRange.getRightColumn());
				uint32_t minRow = static_cast<uint32_t>(matrixRange.getTopRow());
				uint32_t maxRow = static_cast<uint32_t>(matrixRange.getBottomRow());
				auto sourceTableData = sourceTable->getTableData();

				for (uint32_t column = minColumn; column <= maxColumn; column++)
				{
					for (uint32_t row = minRow; row <= maxRow; row++)
					{
						previewFields[sourceTableData->getValue(0, column)].push_back(sourceTableData->getValue(row, column));
					}
					previewFields[sourceTableData->getValue(0, column)].unique();
				}
			}
		}
	}

	return previewFields;
}

