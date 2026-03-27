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
		for (const std::pair<ot::Variable, std::vector<ot::Variable>>& column : matrix.toTableColumns())
		{
			nextRow = std::max(nextRow, currentRow + static_cast<uint32_t>(column.second.size()));

			intern::TableColumn* col = nullptr;

			// Find existing column
			for (intern::TableColumn& existingCol : finalTable)
			{
				if (existingCol.getHeader() == column.first && existingCol.getValueCount() == currentRow)
				{
					col = &existingCol;
					break;
				}
			}
			
			if (col == nullptr)
			{
				// No existing column, create new on
				finalTable.push_back(intern::TableColumn(column.first, currentRow));
				col = &finalTable.back();
			}

			OTAssertNullptr(col);

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

	GenericDataStructMatrix result(currentRow + 1, static_cast<uint32_t>(finalTable.size()));
	MatrixEntryIterator<false> resultIt = result.createIterator(_horizontalHeader);

	for (intern::TableColumn& column : finalTable)
	{
		OTAssert(resultIt.isValid(), "Result iterator is not valid. This should never happen, check the logic of the table merger");
		(*resultIt) = column.getHeader();
		++resultIt;
	}
	for (intern::TableColumn& column : finalTable)
	{
		for (Variable& value : column.getValues())
		{
			OTAssert(resultIt.isValid(), "Result iterator is not valid. This should never happen, check the logic of the table merger");
			(*resultIt) = std::move(value);
			++resultIt;
		}
	}

	return result;
}