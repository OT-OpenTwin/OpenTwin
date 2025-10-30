// @otlicense
// File: Matrix2.h
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

// std header
#include <stdint.h>

namespace ot {

	template <class T>
	class Matrix2 {
	public:
		enum AccessOptimizationDirection { 
			RowWiseAccess,
			ColumnWiseAccess
		};

		Matrix2(int64_t rows, int64_t columns, AccessOptimizationDirection direction = AccessOptimizationDirection::ColumnWiseAccess);
		Matrix2& operator=(const Matrix2& other) = delete;
		Matrix2(const Matrix2& other) = delete;
		~Matrix2();

		inline T getValue(int64_t row, int64_t column) const;
		inline void setValue(int64_t row, int64_t column, T& value);

		int64_t getNumberOfRows(void) const { return m_rows; };
		int64_t getNumberOfColumns(void) const { return m_columns; };

	private:
		T* m_matrix = nullptr;

		int64_t m_rows;
		int64_t m_columns;
		int64_t m_numberOfEntries;

		int64_t m_columnStepWidth;
		int64_t m_rowStepWidth;

		enum Alignment { CacheLine64 = 64, AVX = 128, AVX2 = 256, AVX512 = 512 };
		int m_defaultAlignment = Alignment::CacheLine64;
	};

}

#include "Matrix2.hpp"