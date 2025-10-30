// @otlicense
// File: Matrix2Factory.hpp
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

// OpenTwin header
#include "OTCore/Matrix2Factory.h"

template<class T>
inline Matrix2<T>* Matrix2Factory<T>::createRowWiseOptimized(int64_t rows, int64_t columns, std::vector<std::vector<T>> columnWiseData) {
	auto temp = new Matrix2<T>(rows, columns, Matrix2<T>::accessOptimizationDirection::rowWiseAccess);
	for (int col = 0; col < columns; col++) {
		for (int row = 0; row < rows; row++) {
			temp->SetValue(row, col, columnWiseData[col][row]);
		}
	}

	return temp;
}

template<class T>
inline Matrix2<T>* Matrix2Factory<T>::createRowWiseOptimizedEmpty(int64_t rows, int64_t columns) {
	return new Matrix2<T>(rows, columns, Matrix2<T>::accessOptimizationDirection::rowWiseAccess);
}

template<class T>
inline Matrix2<T>* Matrix2Factory<T>::createColumnWiseOptimized(int64_t rows, int64_t columns, std::vector<std::vector<T>> columnWiseData) {
	auto temp = new Matrix2<T>(rows, columns, Matrix2<T>::accessOptimizationDirection::columnWiseAccess);
	for (int col = 0; col < columns; col++) {
		for (int row = 0; row < rows; row++) {
			temp->SetValue(row, col, columnWiseData[col][row]);
		}
	}

	return temp;
}

template<class T>
inline Matrix2<T>* Matrix2Factory<T>::createColumnWiseOptimizedEmpty(int64_t rows, int64_t columns) {
	return new Matrix2<T>(rows, columns, Matrix2<T>::accessOptimizationDirection::columnWiseAccess);
}

