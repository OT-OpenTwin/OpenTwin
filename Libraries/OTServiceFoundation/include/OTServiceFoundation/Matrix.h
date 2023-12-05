/*****************************************************************//**
 * \file   Matrix.h
 * \brief  Access optimized dense matrix handle.
 * 
 * \author Wagner
 * \date   February 2023
 *********************************************************************/
#pragma once
#include <stdint.h>

template <class T>
class Matrix
{
public:
	enum accessOptimizationDirection {rowWiseAccess, columnWiseAccess};
	Matrix(int64_t rows, int64_t columns, accessOptimizationDirection direction = accessOptimizationDirection::columnWiseAccess);
	Matrix& operator=(const Matrix& other) = delete;
	Matrix(const Matrix& other) = delete;
	~Matrix();

	inline T GetValue(int64_t row, int64_t column) const;
	inline void SetValue(int64_t row, int64_t column, T& value);
	int64_t GetNumberOfRows();
	int64_t GetNumberOfColumns();

private:
	T* _matrix = nullptr;

	int64_t _rows;
	int64_t _columns;
	int64_t _numberOfEntries;

	int64_t _columnStepWidth;
	int64_t _rowStepWidth;

	enum Alignment { CacheLine64 = 64, AVX = 128, AVX2 = 256, AVX512 = 512 };
	int _defaultAlignment = Alignment::CacheLine64;
};

#include "Matrix.hpp"