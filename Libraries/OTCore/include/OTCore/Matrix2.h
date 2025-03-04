//! @file Matrix2.h
//! @brief Access optimized dense matrix handle.
//! @author Wagner
//! @date February 2023
// ###########################################################################################################################################################################################################################################################################################################################

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