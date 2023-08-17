#include "PreviewAssemblerRMD.h"

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
				uint32_t selection[4];
				_selectedRangeEntities[i]->getSelectedRange(selection[0], selection[1], selection[2], selection[3], sourceTable);
				auto sourceTableData = sourceTable->getTableData();
				if (_selectedRangeEntities[i]->getSelectEntireRow())
				{
					selection[3] = static_cast<uint32_t>(sourceTableData->getNumberOfColumns())- 1;
					if (sourceTable->getSelectedHeaderOrientation() == EntityParameterizedDataTable::HeaderOrientation::vertical)
					{
						selection[2] = 1;
					}
					else
					{
						selection[2] = 0;
					}
				}
				if (_selectedRangeEntities[i]->getSelectEntireColumn())
				{
					selection[1] = static_cast<uint32_t>(sourceTableData->getNumberOfRows()) - 1;
					if (sourceTable->getSelectedHeaderOrientation() == EntityParameterizedDataTable::HeaderOrientation::horizontal)
					{
						selection[0] = 1;
					}
					else
					{
						selection[0] = 0;
					}
				}

				for (uint32_t column = selection[2]; column <= selection[3]; column++)
				{
					for (uint32_t row = selection[0]; row <= selection[1]; row++)
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

