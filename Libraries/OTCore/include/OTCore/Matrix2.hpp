//! @file Matrix2.hpp
//! @brief Access optimized dense matrix handle.
//! @author Wagner
//! @date February 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTCore/Matrix2.h"

// std header
#include <exception>

template<class T>
inline ot::Matrix2<T>::Matrix2(int64_t _rows, int64_t _columns, AccessOptimizationDirection _direction) : m_rows(_rows), m_columns(_columns) {
	if (m_rows == 0 || m_columns == 0) {
		throw std::runtime_error("A matrix creation requires the number of rows and columns to be larger than zero.");
	}
	
	m_numberOfEntries = m_rows * m_columns;
	m_matrix = static_cast<T*>(_aligned_malloc(m_numberOfEntries * sizeof(T), m_defaultAlignment));

	if (_direction == AccessOptimizationDirection::ColumnWiseAccess) {
		m_rowStepWidth = m_columns;
		m_columnStepWidth = 1;
	}
	else {
		OTAssert(_direction == AccessOptimizationDirection::RowWiseAccess, "Unknown access direction");
		m_rowStepWidth = 1;
		m_columnStepWidth = m_rows;
	}
}

template<class T>
inline ot::Matrix2<T>::~Matrix2() {
	_aligned_free(_matrix);
	_matrix = nullptr;
}

template<class T>
inline T ot::Matrix2<T>::getValue(int64_t row, int64_t column) const {
	int64_t index = row * _rowStepWidth + column * _columnStepWidth;
	return _matrix[index];
}

template<class T>
inline void ot::Matrix2<T>::setValue(int64_t row, int64_t column, T& value) {
	int64_t index = row * _rowStepWidth + column * _columnStepWidth;
	_matrix[index] = value;
}
