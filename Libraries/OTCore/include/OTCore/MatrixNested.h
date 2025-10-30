// @otlicense
// File: MatrixNested.h
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
#include "OTSystem/OTAssert.h"
#include "OTCore/Matrix.h"

// std header
#include <vector>

namespace ot {

	//! @class MatrixNested
	//! @brief A 2D container implemented as a nested vector container for generic types.
	//! The MatrixNested provides a dynamically resizable 2D array where
	//! the data is stored in a vector of columns and each column vector contains the actual entries.
	//! @tparam T The type of elements stored in the container.
	template <typename T> class MatrixNested : public Matrix<T> {
	public:

		// Constructor

		//! @brief Default constructor.
		MatrixNested();

		//! @brief Constructs a 2D vector with given dimensions, initialized to 0.
		//! @param _dimensions New dimensions.
		MatrixNested(const MatrixIndex& _dimensions);

		//! @brief Constructs a 2D vector with given dimensions and default value.
		//! @param _dimensions New dimensions.
		//! @param _default Default value for initialization.
		MatrixNested(const MatrixIndex& _dimensions, const T& _default);

		//! @brief Constructs a 2D vector from the provided other vector by copying the data.
		//! @param _other Other vector.
		MatrixNested(const MatrixNested& _other);

		//! @brief Constructs a 2D vector from the provided other vector by moving the data.
		//! @param _other Other vector.
		MatrixNested(MatrixNested&& _other);

		//! @brief Destructor.
		virtual ~MatrixNested() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operator

		MatrixNested& operator = (const MatrixNested&);
		MatrixNested& operator = (MatrixNested&&);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data handling
		
		//! @brief Current column count.
		size_t getColumnCount(void) const { return m_size.x; };

		//! @brief Current row count.
		size_t getRowCount(void) const { return m_size.y; };

		//! @brief Removes all the data and sets the dimensions back to 0, 0.
		//! Complexity: O(x * y) where: x = rows; y = columns.
		void clear(void);

		//! @brief Resizes the vector to the given dimensions.
		//! Complexity: O(x * y) where: x = new rows; y = new columns.
		//! @param _x New column count.
		//! @param _y New row count.
		void resize(MatrixIndex::ValueType _x, MatrixIndex::ValueType _y) { this->resize(MatrixIndex({ _x, _y })); };

		//! @brief Resizes the vector to the given dimensions.
		//! Complexity: O(x * y) where: x = new rows; y = new columns.
		//! @param _dimensions New dimensions.
		void resize(const MatrixIndex& _dimensions);

		//! @brief Resizes the vector with a default value.
		//! Complexity: O(x * y) where: x = _rows; y = _cols.
		//! @param _x New column count.
		//! @param _y New row count.
		//! @param _default Default value for new elements.
		void resize(MatrixIndex::ValueType _x, MatrixIndex::ValueType _y, const T& _default) { this->resize(MatrixIndex({ _x, _y }), _default); };

		//! @brief Resizes the vector with a default value.
		//! Complexity: O(x * y) where: x = _rows; y = _cols.
		//! @param _dimensions New dimensions.
		//! @param _default Default value for new elements.
		void resize(const MatrixIndex& _dimensions, const T& _default);

		//! @brief Sets the data at a specific position.
		//! Complexity: O(1).
		//! @param _index Data index.
		//! @param _data Value to set.
		void setData(const MatrixIndex& _index, const T& _data) override;

		//! @brief Sets the data at a specific position.
		//! Complexity: O(1).
		//! @param _index Data index.
		//! @param _data Value to set.
		void setData(const MatrixIndex& _index, T&& _data) override;

		//! @brief Replaces the current data.
		//! Complexity: O(x * y) where: x = rows; y = columns.
		//! @param _dimensions New dimensions.
		//! @param _data New data.
		void setData(const MatrixIndex& _dimensions, const std::vector<std::vector<T>>& _data);

		//! @brief Replaces the current data.
		//! Complexity: O(1) data is moved.
		//! @param _dimensions New dimensions.
		//! @param _data New data.
		void setData(const MatrixIndex& _dimensions, std::vector<std::vector<T>>&& _data);

		//! @brief Retrieves a reference to the data at a specific position.
		//! Complexity: O(1).
		//! @param _index Data index.
		//! @return Reference to the element.
		T& getData(const MatrixIndex& _index) override;

		//! @brief Retrieves a constant reference to the data at a specific position.
		//! Complexity: O(1).
		//! @param _index Data index.
		//! @return Constant reference to the element.
		const T& getData(const MatrixIndex& _index) const override;

		//! @brief Retrieves the modifiable data structure.
		//! @return Reference to the stored data.
		std::vector<std::vector<T>>& getData(void) { return m_data; };

		//! @brief Retrieves the entire data structure.
		//! @return Constant reference to the stored data.
		const std::vector<std::vector<T>>& getData(void) const { return m_data; };

	private:
		MatrixIndex m_size;
		std::vector<std::vector<T>> m_data; //! @brief Row to column vector storage.
	};

}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor

template<typename T>
ot::MatrixNested<T>::MatrixNested() : 
	m_rows(0), m_cols(0)
{}

template<typename T>
ot::MatrixNested<T>::MatrixNested(const MatrixIndex& _dimensions) :
	MatrixNested() 
{
	this->resize(_dimensions);
}

template<typename T>
ot::MatrixNested<T>::MatrixNested(const MatrixIndex& _dimensions, const T& _default) :
	MatrixNested()
{
	this->resize(_dimensions, _default);
}

template<typename T>
ot::MatrixNested<T>::MatrixNested(const MatrixNested& _other) :
	m_size(_other.m_size), m_data(_other.m_data)
{

}

template<typename T>
ot::MatrixNested<T>::MatrixNested(MatrixNested&& _other) :
	m_size(std::move(_other.m_size)), std::move(m_data(_other.m_data))
{

}

// ###########################################################################################################################################################################################################################################################################################################################

// Operator

template<typename T>
ot::MatrixNested<T>& ot::MatrixNested<T>::operator=(const MatrixNested& _other) {
	if (this != &_other) {
		m_size = _other.m_size;
		m_data = _other.m_data;
	}

	return *this;
}

template<typename T>
ot::MatrixNested<T>& ot::MatrixNested<T>::operator=(MatrixNested&& _other) noexcept {
	if (this != &_other) {
		m_size = std::move(_other.m_size);
		m_data = std::move(_other.m_data);
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data handling

template<typename T>
void ot::MatrixNested<T>::clear(void) {
	m_size.x = 0;
	m_size.y = 0;
	m_data.clear();
}

template<typename T>
void ot::MatrixNested<T>::resize(const MatrixIndex& _dimensions) {
	this->resize(_dimensions, static_cast<const T>(0));
}

template<typename T>
void ot::MatrixNested<T>::resize(const MatrixIndex& _dimensions, const T& _default) {
	std::vector<std::vector<T>> tmp = std::move(m_data);
	m_data.clear();

	m_data.reserve(_dimensions.x);

	for (size_t x = 0; x < _dimensions.x; x++) {
		std::vector<T> newColumn;
		newColumn.reserve(_dimensions.y);
		for (size_t y = 0; y < _dimensions.y; y++) {
			if (x < m_size.x && y < m_size.y) {
				newColumn.push_back(std::move(tmp[x][y]));
			}
			else {
				newColumn.push_back(_default);
			}
		}

		m_data.push_back(newColumn);
	}

	m_size = _dimensions;
}

template<typename T>
void ot::MatrixNested<T>::setData(const MatrixIndex& _index, const T& _data) {
	OTAssert(_index.x < m_size.x, "Column index out of range");
	OTAssert(_index.y < m_size.y, "Row index out of range");
	m_data[_index.x][_index.y] = _data;
}

template<typename T>
void ot::MatrixNested<T>::setData(const MatrixIndex& _index, T&& _data) {
	OTAssert(_index.x < m_size.x, "Column index out of range");
	OTAssert(_index.y < m_size.y, "Row index out of range");
	m_data[_index.x][_index.y] = std::move(_data);
}

template<typename T>
void ot::MatrixNested<T>::setData(const ot::MatrixIndex& _dimensions, const std::vector<std::vector<T>>& _data) {
	m_size = _dimensions;
	m_data = _data;
}

template<typename T>
void ot::MatrixNested<T>::setData(const ot::MatrixIndex& _dimensions, std::vector<std::vector<T>>&& _data) {
	m_size = _dimensions;
	m_data = std::move(_data);
}

template<typename T>
T& ot::MatrixNested<T>::getData(const MatrixIndex& _index) {
	OTAssert(_index.x < m_size.x, "Column index out of range");
	OTAssert(_index.y < m_size.y, "Row index out of range");
	return m_data[_index.x][_index.y];
}

template<typename T>
const T& ot::MatrixNested<T>::getData(const MatrixIndex& _index) const {
	OTAssert(_index.x < m_size.x, "Column index out of range");
	OTAssert(_index.y < m_size.y, "Row index out of range");
	return m_data[_index.x][_index.y];
}
