// @otlicense

// OpenTwin header
#include "OTCore/DataStruct/GenericDataStructMatrixMerger.h"

// std header
#include <vector>

namespace ot {

	namespace intern {

		class TableColumn
		{
		public:
			TableColumn(const ot::Variable& _header, uint32_t _valueCount)
				: m_header(_header), m_valueCount(_valueCount)
			{
				m_values.resize(_valueCount, Variable());
			};

			void addVariable(const ot::Variable& _value)
			{
				m_values.push_back(_value);
				m_valueCount++;
			}

			void addVariable(ot::Variable&& _value)
			{
				m_values.push_back(std::move(_value));
				m_valueCount++;
			};

			const Variable& getHeader() const { return m_header; };
			std::list<ot::Variable>& getValues() { return m_values; };
			const std::list<ot::Variable>& getValues() const { return m_values; };
			uint32_t getValueCount() const { return m_valueCount; };

		private:
			Variable m_header;
			std::list<ot::Variable> m_values;
			uint32_t m_valueCount = 0;
		};

	}


}

ot::GenericDataStructMatrix ot::GenericDataStructMatrixMerger::tableMerge(const std::list<GenericDataStructMatrix>& _matrices, bool _horizontalHeader)
{
	std::list<intern::TableColumn> finalTable;
	
	uint32_t currentRow = 0;
	
	// For each matrix
	for (const GenericDataStructMatrix& matrix : _matrices)
	{
		auto tableColumns = matrix.toTableColumns(_horizontalHeader);

		uint32_t nextRow = currentRow + 1;

		// For each column
		for (const auto& column : tableColumns)
		{
			// Check if we have values to add
			if (column.second.empty())
			{
				continue;
			}

			nextRow = std::max(nextRow, currentRow + static_cast<uint32_t>(column.second.size()));

			intern::TableColumn* col = nullptr;

			// Find existing column
			for (intern::TableColumn& existingCol : finalTable)
			{
				// Check if name is equal and current value count is equal to current row (no new values added from current matrix)
				if (existingCol.getHeader() == column.first && existingCol.getValueCount() == currentRow)
				{
					col = &existingCol;
					break;
				}
			}
			
			if (col == nullptr)
			{
				// No existing column, create new one
				finalTable.push_back(intern::TableColumn(column.first, currentRow));
				col = &finalTable.back();
			}

			OTAssertNullptr(col);
			OTAssert(col->getValueCount() == currentRow, "Data mismatch");

			// Add values to column
			for (const ot::Variable& value : column.second)
			{
				col->addVariable(value);
			}
		}

		// Expand all existing columns to the new row count if necessary
		for (intern::TableColumn& existingCol : finalTable)
		{
			while (existingCol.getValueCount() < nextRow)
			{
				existingCol.addVariable(Variable());
			}
		}

		currentRow = nextRow;
	}

	uint32_t tableColumnCount = static_cast<uint32_t>(finalTable.size());
	uint32_t tableRowCount = currentRow + 1;

	GenericDataStructMatrix result(_horizontalHeader ? tableRowCount : tableColumnCount, _horizontalHeader ? tableColumnCount : tableRowCount);

	auto headerIt = result.createHeaderIterator(_horizontalHeader);

	for (intern::TableColumn& column : finalTable)
	{
		OTAssert(headerIt.isValid(), "Result iterator is not valid. This should never happen, check the logic of the table merger");
		(*headerIt) = column.getHeader();

		auto dataIt = result.createDataColumnIterator(headerIt.getDataColumnIndex(), _horizontalHeader);
		for (Variable& value : column.getValues())
		{
			OTAssert(dataIt.isValid(), "Result iterator is not valid. This should never happen, check the logic of the table merger");
			(*dataIt) = std::move(value);
			++dataIt;
		}
		OTAssert(!dataIt.isValid(), "Result data iterator is still valid after writing all data. This should never happen, check the logic of the table merger");

		++headerIt;
	}

	return result;
}