#pragma once

#include "OTGui/TableCfg.h"
#include "OTGui/TableRange.h"
namespace ot
{
	namespace TableIndexSchemata
	{
		static ot::TableRange userRangeToMatrixRange(const ot::TableRange& _range, ot::TableCfg::TableHeaderMode _headerOrientation)
		{
			//First we switch from base-1 index to base -0 index
			int rangeRowBottom = _range.getBottomRow() - 1;
			int rangeRowTop = _range.getTopRow() - 1;
			int rangeColumnLeft = _range.getLeftColumn() - 1;
			int rangeColumnRight = _range.getRightColumn() - 1;

			//Now we take the header in consideration, which is part of the matrix and its row/column index, but the TableConfig handles the header as separate vector
			if (_headerOrientation == ot::TableCfg::TableHeaderMode::Horizontal)
			{
				rangeRowBottom += 1;
				rangeRowTop += 1;
			}
			else if (_headerOrientation == ot::TableCfg::TableHeaderMode::Vertical)
			{
				rangeColumnLeft += 1;
				rangeColumnRight += 1;
			}
			ot::TableRange matrixRange(rangeRowTop, rangeColumnLeft, rangeRowBottom, rangeColumnRight);
			return matrixRange;
		}
		static ot::TableRange selectionRangeToMatrixRange(const ot::TableRange& _range, ot::TableCfg::TableHeaderMode _headerOrientation)
		{
			//Both QTable and matrix work with a base-0 index. Nothing needs to be changed in this regard
			//The QTable handles the header as a separate vector, in the matrix it is simply part of the matrix. Here we need to make an adjustment
			//First we switch from base-1 index to base -0 index
			int rangeRowBottom = _range.getBottomRow();
			int rangeRowTop = _range.getTopRow();
			int rangeColumnLeft = _range.getLeftColumn();
			int rangeColumnRight = _range.getRightColumn();

			//Now we take the header in consideration, which is part of the matrix and its row/column index, but the TableConfig handles the header as separate vector
			if (_headerOrientation == ot::TableCfg::TableHeaderMode::Horizontal)
			{
				rangeRowBottom += 1;
				rangeRowTop += 1;
			}
			else if (_headerOrientation == ot::TableCfg::TableHeaderMode::Vertical)
			{
				rangeColumnLeft += 1;
				rangeColumnRight += 1;
			}
			ot::TableRange matrixRange(rangeRowTop, rangeColumnLeft, rangeRowBottom, rangeColumnRight);

			return matrixRange;
		}
		static ot::TableRange selectionRangeToUserRange(const ot::TableRange& _range)
		{
			//We switch base-0 to base-1 index
			int rangeRowBottom = _range.getBottomRow() + 1;
			int rangeRowTop = _range.getTopRow() + 1;
			int rangeColumnLeft = _range.getLeftColumn() + 1;
			int rangeColumnRight = _range.getRightColumn() + 1;
			ot::TableRange userRange(rangeRowTop, rangeColumnLeft, rangeRowBottom, rangeColumnRight);
			return userRange;
		}
		static ot::TableRange userRangeToSelectionRange(const ot::TableRange& _range)
		{
			//We switch base-1 to base-0 index
			int rangeRowBottom = _range.getBottomRow() - 1;
			int rangeRowTop = _range.getTopRow() - 1;
			int rangeColumnLeft = _range.getLeftColumn() - 1;
			int rangeColumnRight = _range.getRightColumn() - 1;
			ot::TableRange selectionRange(rangeRowTop, rangeColumnLeft, rangeRowBottom, rangeColumnRight);
			return selectionRange;
		}
		static ot::TableRange matrixToUserRange(const ot::TableRange& _range, ot::TableCfg::TableHeaderMode _headerOrientation)
		{
			//First we switch from base-0 index to base-1 index
			int rangeRowBottom = _range.getBottomRow() + 1;
			int rangeRowTop = _range.getTopRow() + 1;
			int rangeColumnLeft = _range.getLeftColumn() + 1;
			int rangeColumnRight = _range.getRightColumn() + 1;

			//Now we take the header in consideration, which is part of the matrix and its row/column index, but the TableConfig handles the header as separate vector
			if (_headerOrientation == ot::TableCfg::TableHeaderMode::Horizontal)
			{
				rangeRowBottom -= 1;
				rangeRowTop -= 1;
			}
			else if (_headerOrientation == ot::TableCfg::TableHeaderMode::Vertical)
			{
				rangeColumnLeft -= 1;
				rangeColumnRight -= 1;
			}
			ot::TableRange matrixRange(rangeRowTop, rangeColumnLeft, rangeRowBottom, rangeColumnRight);
			return matrixRange;
		}
	}
}