// @otlicense
// File: TableIndexSchemata.cpp
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

#pragma once

#include "OTGui/TableIndexSchemata.h"

ot::TableRange ot::TableIndexSchemata::userRangeToMatrixRange(const ot::TableRange& _range, ot::TableCfg::TableHeaderMode _headerOrientation) {
	//First we switch from base-1 index to base -0 index
	int rangeRowBottom = _range.getBottomRow() - 1;
	int rangeRowTop = _range.getTopRow() - 1;
	int rangeColumnLeft = _range.getLeftColumn() - 1;
	int rangeColumnRight = _range.getRightColumn() - 1;

	//Now we take the header in consideration, which is part of the matrix and its row/column index, but the TableConfig handles the header as separate vector
	if (_headerOrientation == ot::TableCfg::TableHeaderMode::Horizontal) {
		rangeRowBottom += 1;
		rangeRowTop += 1;
	}
	else if (_headerOrientation == ot::TableCfg::TableHeaderMode::Vertical) {
		rangeColumnLeft += 1;
		rangeColumnRight += 1;
	}
	ot::TableRange matrixRange(rangeRowTop, rangeColumnLeft, rangeRowBottom, rangeColumnRight);
	return matrixRange;
}
		
ot::TableRange ot::TableIndexSchemata::selectionRangeToMatrixRange(const ot::TableRange& _range, ot::TableCfg::TableHeaderMode _headerOrientation) {
	//Both QTable and matrix work with a base-0 index. Nothing needs to be changed in this regard
	//The QTable handles the header as a separate vector, in the matrix it is simply part of the matrix. Here we need to make an adjustment
	//First we switch from base-1 index to base -0 index
	int rangeRowBottom = _range.getBottomRow();
	int rangeRowTop = _range.getTopRow();
	int rangeColumnLeft = _range.getLeftColumn();
	int rangeColumnRight = _range.getRightColumn();

	//Now we take the header in consideration, which is part of the matrix and its row/column index, but the TableConfig handles the header as separate vector
	if (_headerOrientation == ot::TableCfg::TableHeaderMode::Horizontal) {
		rangeRowBottom += 1;
		rangeRowTop += 1;
	}
	else if (_headerOrientation == ot::TableCfg::TableHeaderMode::Vertical) {
		rangeColumnLeft += 1;
		rangeColumnRight += 1;
	}
	ot::TableRange matrixRange(rangeRowTop, rangeColumnLeft, rangeRowBottom, rangeColumnRight);

	return matrixRange;
}
		
ot::TableRange ot::TableIndexSchemata::selectionRangeToUserRange(const ot::TableRange& _range) {
	//We switch base-0 to base-1 index
	int rangeRowBottom = _range.getBottomRow() + 1;
	int rangeRowTop = _range.getTopRow() + 1;
	int rangeColumnLeft = _range.getLeftColumn() + 1;
	int rangeColumnRight = _range.getRightColumn() + 1;
	ot::TableRange userRange(rangeRowTop, rangeColumnLeft, rangeRowBottom, rangeColumnRight);
	return userRange;
}
		
ot::TableRange ot::TableIndexSchemata::userRangeToSelectionRange(const ot::TableRange& _range) {
	//We switch base-1 to base-0 index
	int rangeRowBottom = _range.getBottomRow() - 1;
	int rangeRowTop = _range.getTopRow() - 1;
	int rangeColumnLeft = _range.getLeftColumn() - 1;
	int rangeColumnRight = _range.getRightColumn() - 1;
	ot::TableRange selectionRange(rangeRowTop, rangeColumnLeft, rangeRowBottom, rangeColumnRight);
	return selectionRange;
}
		
ot::TableRange ot::TableIndexSchemata::matrixToUserRange(const ot::TableRange& _range, ot::TableCfg::TableHeaderMode _headerOrientation) {
	//First we switch from base-0 index to base-1 index
	int rangeRowBottom = _range.getBottomRow() + 1;
	int rangeRowTop = _range.getTopRow() + 1;
	int rangeColumnLeft = _range.getLeftColumn() + 1;
	int rangeColumnRight = _range.getRightColumn() + 1;

	//Now we take the header in consideration, which is part of the matrix and its row/column index, but the TableConfig handles the header as separate vector
	if (_headerOrientation == ot::TableCfg::TableHeaderMode::Horizontal) {
		rangeRowBottom -= 1;
		rangeRowTop -= 1;
	}
	else if (_headerOrientation == ot::TableCfg::TableHeaderMode::Vertical) {
		rangeColumnLeft -= 1;
		rangeColumnRight -= 1;
	}
	ot::TableRange matrixRange(rangeRowTop, rangeColumnLeft, rangeRowBottom, rangeColumnRight);
	return matrixRange;
}