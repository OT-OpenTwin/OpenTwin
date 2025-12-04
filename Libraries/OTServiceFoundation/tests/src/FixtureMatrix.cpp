// @otlicense
// File: FixtureMatrix.cpp
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

//#include "FixtureMatrix.h"
//#include "OpenTwinFoundation/MatrixFactory.h"
//
//FixtureMatrix::FixtureMatrix()
//{
//	_rows = 5;
//	_columns = 3;
//	_expectedDataInt.reserve(_columns);
//
//	for (int j = 0; j < _columns; j++)
//	{
//		std::vector<int> temp(_rows);
//
//		for (int i = 0; i < _rows; i++)
//		{
//			int index = _rows * i + j * _columns;
//			temp[i] = index;
//		}
//		_expectedDataInt.push_back(temp);
//	}	
//}
//
//Matrix<int>* FixtureMatrix::CreateSquareMatrixRowWiseOptimizedInt()
//{
//	return MatrixFactory<int>::CreateRowWiseOptimized(_rows, _columns, _expectedDataInt);
//}