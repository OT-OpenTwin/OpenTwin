// @otlicense

#include "gtest/gtest.h"
#include "OTCore/DataStruct/GenericDataStructMatrixMerger.h"

namespace ot {
	namespace intern {
		std::string getText(const std::string& _matrixName, uint32_t _row, uint32_t _column, bool _horizontalHeader)
		{
			if ((_horizontalHeader && _row == 0) || (!_horizontalHeader && _column == 0))
			{
				return "Header " + _matrixName + " " + std::to_string(_row) + ":" + std::to_string(_column);
			}
			else
			{
				return _matrixName + " " + std::to_string(_row) + ":" + std::to_string(_column);
			}
		}

		bool matrixSubsetEqual(const ot::GenericDataStructMatrix& _matrixA, const ot::GenericDataStructMatrix& _matrixB, 
			uint32_t _fromRow, uint32_t _endRow, uint32_t _fromColumn, uint32_t _endColumn, uint32_t _bRowOffset, uint32_t _bColOffset)
		{ 
			for (uint32_t col = _fromColumn; col < _endColumn; col++)
			{
				for (uint32_t row = _fromRow; row < _endRow; row++)
				{
					if (_matrixA.getValue({ row, col }) != _matrixB.getValue({ row + _bRowOffset, col + _bColOffset }))
					{
						return false;
					}
				}
			}

			return true;
		}

		ot::GenericDataStructMatrix createMatrix(const std::string& _name, uint32_t _rows, uint32_t _columns, bool _horizontalHeader)
		{
			ot::GenericDataStructMatrix matrix(_rows, _columns);

			for (uint32_t col = 0; col < _columns; col++)
			{
				for (uint32_t row = 0; row < _rows; row++)
				{
					matrix.setValue({ row, col }, getText(_name, row, col, _horizontalHeader));
				}
			}

			return matrix;
		}
	}
}

TEST(GenericDataStructMatrixMerger, HorizontalTableMerge)
{
	auto matrixA = ot::intern::createMatrix("A", 2, 4, true);
	auto matrixB = ot::intern::createMatrix("B", 2, 2, true);

	auto resultMatrix = ot::GenericDataStructMatrixMerger::tableMerge({ matrixA, matrixB }, true);

	EXPECT_EQ(3, resultMatrix.getNumberOfRows());
	EXPECT_EQ(6, resultMatrix.getNumberOfColumns());

	bool trueA = ot::intern::matrixSubsetEqual(matrixA, resultMatrix, 1, 2, 0, 4, 0, 0);
 	EXPECT_TRUE(trueA);
	bool trueB = ot::intern::matrixSubsetEqual(matrixB, resultMatrix, 1, 2, 0, 2, 1, 4);
	EXPECT_TRUE(trueB);
	
}