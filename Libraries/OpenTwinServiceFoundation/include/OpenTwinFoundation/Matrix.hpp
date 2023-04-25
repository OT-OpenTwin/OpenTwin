#pragma once
#include "Matrix.h"
#include <exception>

template<class T>
inline Matrix<T>::Matrix(int64_t rows, int64_t columns, accessOptimizationDirection direction) : _rows(rows), _columns(columns)
{
	if (_rows == 0 || _columns == 0)
	{
		throw std::runtime_error("A matrix creation requires the number of rows and columns to be larger than zero.");
	}
	_numberOfEntries = _rows * _columns;
	_matrix = static_cast<T*>(_aligned_malloc(_numberOfEntries * sizeof(T),_defaultAlignment));
	if (direction == accessOptimizationDirection::columnWiseAccess)
	{
		_rowStepWidth = _columns;
		_columnStepWidth = 1;
	}
	else
	{
		_rowStepWidth = 1;
		_columnStepWidth = _rows;
	}
}

template<class T>
inline Matrix<T>::~Matrix()
{
	_aligned_free(_matrix);
	_matrix = nullptr;
}

template<class T>
inline T Matrix<T>::GetValue(int64_t row, int64_t column) const
{
	int64_t index = row * _rowStepWidth + column * _columnStepWidth;
	return _matrix[index];
}

template<class T>
inline void Matrix<T>::SetValue(int64_t row, int64_t column, T& value)
{
	int64_t index = row * _rowStepWidth + column * _columnStepWidth;
	_matrix[index] = value;
}

template<class T>
inline int64_t Matrix<T>::GetNumberOfRows()
{
	return _rows;
}

template<class T>
inline int64_t Matrix<T>::GetNumberOfColumns()
{
	return _columns;
}
