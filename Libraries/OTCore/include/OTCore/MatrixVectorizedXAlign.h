// @otlicense
// File: MatrixVectorizedXAlign.h
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

#define OT_MATRIX_GET_INDEX_XALIGN(___dimensions, ___index) (___index.y * ___dimensions.x) + ___index.x

namespace ot {

	//! @class MatrixVectorizedXAlign
	//! @brief A matrix container implemented as a linear vector.
	//! The MatrixVectorizedXAlign provides a dynamically resizable matrix stored in a single linear vector.
	//! It allows efficient access and modification of elements and supports row and column operations.
	//! 
	//! The data is X aligned so the X values are stored next to each other in the memory.
	//! Therefore row-wise operations are faster than column.wise operations.
	//! @tparam T Type of elements stored in the container.
	template <typename T> class MatrixVectorizedXAlign : public Matrix<T> {
	public:

		// Constructor

		//! @brief Default constructor.
		MatrixVectorizedXAlign();

		//! @brief Constructs a 2D vector with the given dimensions.
		//! @param _dimensions New dimensions.
		MatrixVectorizedXAlign(const MatrixIndex& _dimensions);

		//! @brief Constructs a 2D vector with the given dimensions and default value.
		//! @param _dimensions New dimensions.
		//! @param _default Default value for all elements.
		MatrixVectorizedXAlign(const MatrixIndex& _dimensions, const T& _default);

		//! @brief Constructs a 2D vector with the given gimensions and data.
		//! @param _x X dimensions to set.
		//! @param _y Y dimensions to set.
		//! @param _data Data to set.
		MatrixVectorizedXAlign(MatrixIndex::ValueType _x, MatrixIndex::ValueType _y, std::vector<T>&& _data) : MatrixVectorizedXAlign(MatrixIndex(_x, _y), std::move(_data)) {};

		//! @brief Constructs a 2D vector with the given gimensions and data.
		//! @param _dimensions Dimensions to set.
		//! @param _data Data to set.
		MatrixVectorizedXAlign(MatrixIndex&& _dimensions, std::vector<T>&& _data);

		//! @brief Constructs a 2D vector from the provided other vector by copying the data.
		//! @param _other Other vector.
		MatrixVectorizedXAlign(const MatrixVectorizedXAlign& _other);

		//! @brief Constructs a 2D vector from the provided other vector by moving the data.
		//! @param _other Other vector.
		MatrixVectorizedXAlign(MatrixVectorizedXAlign&& _other) noexcept;

		//! @brief Destructor.
		virtual ~MatrixVectorizedXAlign() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operator

		//! @brief Assignment operator.
		//! Clears the current data structure and copies the data from the provided vector.
		//! @param _other Other vector.
		//! @return Returns reference to this vector.
		MatrixVectorizedXAlign& operator = (const MatrixVectorizedXAlign& _other);

		//! @brief Move-Assignment operator.
		//! Clears the current data structure by replacing (moving) the data the provided vector.
		//! The provided vector will remain in an undefined state, call clear to ensure a true clear state after the move.
		//! @param _other Other vector.
		//! @return Returns reference to this vector.
		MatrixVectorizedXAlign& operator = (MatrixVectorizedXAlign&& _other) noexcept;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Iterator

		using iterator = typename std::vector<T>::iterator;
		using const_iterator = typename std::vector<T>::const_iterator;
		using value_type = T;

		iterator begin() { return m_data.begin(); };
		iterator end() { return m_data.end(); };
		const_iterator begin() const { return m_data.begin(); };
		const_iterator end() const { return m_data.end(); };
		const_iterator cbegin() const { return m_data.cbegin(); };
		const_iterator cend() const { return m_data.cend(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data handling

		//! @brief Current column count.
		size_t getColumnCount(void) const { return m_size.x; };

		//! @brief Current row count.
		size_t getRowCount(void) const { return m_size.y; };

		//! @brief Removes all the data and sets the dimensions back to 0, 0.
		//! Complexity: O(r * c) where r = _rows, c = _cols.
		void clear(void);

		//! @brief Resizes the 2D vector.
		//! Complexity: O(r * c) where r = _rows, c = _cols.
		//! Uses 0 as default.
		//! @param _x New column count.
		//! @param _y New row count.
		void resize(MatrixIndex::ValueType _x, MatrixIndex::ValueType _y) { this->resize(MatrixIndex(_x, _y), static_cast<const T>(0)); };

		//! @brief Resizes the 2D vector.
		//! Complexity: O(r * c) where r = _rows, c = _cols.
		//! Uses 0 as default.
		//! @param _dimensions New dimensions.
		void resize(const MatrixIndex& _dimensions) { this->resize(_dimensions, static_cast<const T>(0)); };

		//! @brief Resizes the 2D vector with a default value.
		//! Complexity: O(r * c) where r = _rows, c = _cols.
		//! @param _x New column count.
		//! @param _y New row count.
		//! @param _default Default value for new elements.
		void resize(MatrixIndex::ValueType _x, MatrixIndex::ValueType _y, const T& _default) { this->resize(MatrixIndex(_x, _y), _default); };

		//! @brief Resizes the 2D vector with a default value.
		//! Complexity: O(r * c) where r = _rows, c = _cols.
		//! @param _dimensions New dimensions.
		//! @param _default Default value for new elements.
		void resize(const MatrixIndex& _dimensions, const T& _default);

		//! @brief Sets the value of a specific element.
		//! Complexity: O(1).
		//! @param _dimensions Data index.
		//! @param _data Value to set.
		void setData(const MatrixIndex& _index, const T& _data) override;

		//! @brief Sets the value of a specific element.
		//! Complexity: O(1).
		//! @param _dimensions Data index.
		//! @param _data Value to set.
		void setData(const MatrixIndex& _index, T&& _data) override;

		//! @brief Retrieves a reference to a specific element.
		//! Complexity: O(1).
		//! @param _dimensions Data index.
		//! @return Reference to the requested element.
		T& getData(const MatrixIndex& _index) override;

		//! @brief Retrieves a const reference to a specific element.
		//! Complexity: O(1).
		//! @param _dimensions Data index.
		//! @return Const reference to the requested element.
		const T& getData(const MatrixIndex& _index) const override;

		//! @brief Inserts a row at the front.
		//! Complexity: O(c) where c = number of columns.
		//! @param _defaultValue Default value for the new row.
		void pushRowFront(const T& _defaultValue = T());

		//! @brief Inserts a row at the front.
		//! Complexity: O(c) where c = number of columns.
		//! @param _row Row data.
		void pushRowFront(const std::vector<T>& _row);

		//! @brief Inserts a row at the front (move version).
		//! Complexity: O(c) where c = number of columns.
		//! @param _row Row data.
		void pushRowFront(std::vector<T>&& _row);

		//! @brief Inserts a row at the back.
		//! Complexity: O(c) where c = number of columns.
		//! @param _defaultValue Default value for the new row.
		void pushRowBack(const T& _defaultValue = T());

		//! @brief Inserts a row at the back.
		//! Complexity: O(c) where c = number of columns.
		//! @param _row Row data.
		void pushRowBack(const std::vector<T>& _row);

		//! @brief Inserts a row at the back (move version).
		//! Complexity: O(c) where c = number of columns.
		//! @param _row Row data.
		void pushRowBack(std::vector<T>&& _row);

		//! @brief Inserts a column at the front.
		//! Complexity: O(r) where r = number of rows.
		//! @param _defaultValue Default value for the new column.
		void pushColumnFront(const T& _defaultValue = T());

		//! @brief Inserts a column at the front.
		//! Complexity: O(r) where r = number of rows.
		//! @param _col Column data.
		void pushColumnFront(const std::vector<T>& _col);

		//! @brief Inserts a column at the front (move version).
		//! Complexity: O(r) where r = number of rows.
		//! @param _col Column data.
		void pushColumnFront(std::vector<T>&& _col);

		//! @brief Inserts a column at the back.
		//! Complexity: O(r) where r = number of rows.
		//! @param _defaultValue Default value for the new column.
		void pushColumnBack(const T& _defaultValue = T());

		//! @brief Inserts a column at the back.
		//! Complexity: O(r) where r = number of rows.
		//! @param _col Column data.
		void pushColumnBack(const std::vector<T>& _col);

		//! @brief Inserts a column at the back (move version).
		//! Complexity: O(r) where r = number of rows.
		//! @param _col Column data.
		void pushColumnBack(std::vector<T>&& _col);

		//! @brief Replaces the current data.
		//! Complexity: O(1) data is moved.
		//! @param _dimensions New dimensions.
		//! @param _data New data.
		void setData(const MatrixIndex& _dimensions, std::vector<T>&& _data);

		//! @brief Retrieves a reference to the internal data.
		//! @return Reference to the data vector.
		std::vector<T>& getData(void) { return m_data; };

		//! @brief Retrieves a const reference to the internal data.
		//! @return Const reference to the data vector.
		const std::vector<T>& getData(void) const { return m_data; };

	private:
		MatrixIndex m_size; //! @brief Dimensions.
		std::vector<T> m_data; //! @brief Y aligned linear vector storage.
	};

}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor

template<typename T>
ot::MatrixVectorizedXAlign<T>::MatrixVectorizedXAlign()
	: m_size(0, 0)
{}

template<typename T>
ot::MatrixVectorizedXAlign<T>::MatrixVectorizedXAlign(const MatrixIndex& _dimensions) :
	MatrixVectorizedXAlign() 
{ 
	this->resize(_dimensions);
}

template<typename T>
ot::MatrixVectorizedXAlign<T>::MatrixVectorizedXAlign(const MatrixIndex& _dimensions, const T& _default) :
	MatrixVectorizedXAlign()
{
	this->resize(_dimensions, _default);
}

template<typename T>
inline ot::MatrixVectorizedXAlign<T>::MatrixVectorizedXAlign(MatrixIndex&& _dimensions, std::vector<T>&& _data) :
	m_size(std::move(_dimensions)), m_data(std::move(_data))
{
	OTAssert(m_data.size() == m_size.x * m_size.y, "Invalid data size");
}

template<typename T>
ot::MatrixVectorizedXAlign<T>::MatrixVectorizedXAlign(const MatrixVectorizedXAlign& _other) :
	m_size(_other.m_size), m_data(_other.m_data)
{}

template<typename T>
ot::MatrixVectorizedXAlign<T>::MatrixVectorizedXAlign(MatrixVectorizedXAlign&& _other) noexcept :
	m_size(std::move(_other.m_size)), std::move(m_data(_other.m_data))
{}

// ###########################################################################################################################################################################################################################################################################################################################

// Operator

template<typename T>
ot::MatrixVectorizedXAlign<T>& ot::MatrixVectorizedXAlign<T>::operator=(const MatrixVectorizedXAlign& _other) {
	if (this != &_other) {
		m_size = _other.m_size;
		m_data = _other.m_data;
	}

	return *this;
}

template<typename T>
ot::MatrixVectorizedXAlign<T>& ot::MatrixVectorizedXAlign<T>::operator=(MatrixVectorizedXAlign&& _other) noexcept {
	if (this != &_other) {
		m_size = std::move(_other.m_size);
		m_data = std::move(_other.m_data);
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data handling

template<typename T>
void ot::MatrixVectorizedXAlign<T>::clear(void) {
	m_size.x = 0;
	m_size.y = 0;
	m_data.clear();
}

template<typename T>
void ot::MatrixVectorizedXAlign<T>::resize(const MatrixIndex& _dimensions, const T& _default) {
	std::vector<T> tmp = std::move(m_data);
	
	m_data.clear();
	m_data.reserve(_dimensions.x * _dimensions.y);

	for (size_t y = 0; y < _dimensions.y; y++) {
		for (size_t x = 0; x < _dimensions.x; x++) {
			if (y < m_size.y && x < _dimensions.x) {
				size_t oldIx = (y * _dimensions.x) + x;
				m_data.push_back(std::move(tmp[oldIx]));
			}
			else {
				m_data.push_back(_default);
			}
		}
	}

	m_size = _dimensions;
}

template<typename T>
inline void ot::MatrixVectorizedXAlign<T>::setData(const MatrixIndex& _index, const T& _data) {
	OTAssert(_index.x < m_size.x, "Column index out of range");
	OTAssert(_index.y < m_size.y, "Row index out of range");
	m_data[OT_MATRIX_GET_INDEX_XALIGN(m_size, _index)] = _data;
}

template<typename T>
inline void ot::MatrixVectorizedXAlign<T>::setData(const MatrixIndex& _index, T&& _data) {
	OTAssert(_index.x < m_size.x, "Column index out of range");
	OTAssert(_index.y < m_size.y, "Row index out of range");
	m_data[OT_MATRIX_GET_INDEX_XALIGN(m_size, _index)] = std::move(_data);
}

template<typename T>
inline T& ot::MatrixVectorizedXAlign<T>::getData(const MatrixIndex& _index) {
	OTAssert(_index.x < m_size.x, "Column index out of range");
	OTAssert(_index.y < m_size.y, "Row index out of range");
	return m_data[OT_MATRIX_GET_INDEX_XALIGN(m_size, _index)];
}

template<typename T>
inline const T& ot::MatrixVectorizedXAlign<T>::getData(const MatrixIndex& _index) const {
	OTAssert(_index.x < m_size.x, "Column index out of range");
	OTAssert(_index.y < m_size.y, "Row index out of range");
	return m_data[OT_MATRIX_GET_INDEX_XALIGN(m_size, _index)];
}

template<typename T>
inline void ot::MatrixVectorizedXAlign<T>::pushRowFront(const T& _defaultValue) {
	m_data.insert(m_data.begin(), m_size.x, _defaultValue);
	m_size.y++;
}

template<typename T>
inline void ot::MatrixVectorizedXAlign<T>::pushRowFront(const std::vector<T>& _row) {
	OTAssert(_row.size() == m_size.x, "Row size mismatch");
	m_data.insert(m_data.begin(), _row.begin(), _row.end());
	m_size.y++;
}

template<typename T>
inline void ot::MatrixVectorizedXAlign<T>::pushRowFront(std::vector<T>&& _row) {
	OTAssert(_row.size() == m_size.x, "Row size mismatch");
	m_data.insert(m_data.begin(), std::make_move_iterator(_row.begin()), std::make_move_iterator(_row.end()));
	m_size.y++;
}

template<typename T>
inline void ot::MatrixVectorizedXAlign<T>::pushRowBack(const T& _defaultValue) {
	m_data.insert(m_data.end(), m_size.x, _defaultValue);
	m_size.y++;
}

template<typename T>
inline void ot::MatrixVectorizedXAlign<T>::pushRowBack(const std::vector<T>& _row) {
	OTAssert(_row.size() == m_size.x, "Row size mismatch");
	m_data.insert(m_data.end(), _row.begin(), _row.end());
	m_size.y++;
}

template<typename T>
inline void ot::MatrixVectorizedXAlign<T>::pushRowBack(std::vector<T>&& _row) {
	OTAssert(_row.size() == m_size.x, "Row size mismatch");
	m_data.insert(m_data.end(), std::make_move_iterator(_row.begin()), std::make_move_iterator(_row.end()));
	m_size.y++;
}

template<typename T>
inline void ot::MatrixVectorizedXAlign<T>::pushColumnFront(const T& _defaultValue) {
	for (size_t r = 0; r < m_size.y; ++r) {
		m_data.insert(m_data.begin() + r * (m_size.x + 1), _defaultValue);
	}
	m_size.x++;
}

template<typename T>
inline void ot::MatrixVectorizedXAlign<T>::pushColumnFront(const std::vector<T>& _col) {
	OTAssert(_col.size() == m_size.y, "Column size mismatch");
	for (size_t r = 0; r < m_size.y; ++r) {
		m_data.insert(m_data.begin() + r * (m_size.x + 1), _col[r]);
	}
	m_size.x++;
}

template<typename T>
inline void ot::MatrixVectorizedXAlign<T>::pushColumnFront(std::vector<T>&& _col) {
	OTAssert(_col.size() == m_size.y, "Column size mismatch");
	for (size_t r = 0; r < m_size.y; ++r) {
		m_data.insert(m_data.begin() + r * (m_size.x + 1), std::move(_col[r]));
	}
	m_size.x++;
}

template<typename T>
inline void ot::MatrixVectorizedXAlign<T>::pushColumnBack(const T& _defaultValue) {
	for (size_t r = 0; r < m_size.y; ++r) {
		m_data.insert(m_data.begin() + (r + 1) * m_size.x + r, _defaultValue);
	}
	m_size.x++;
}

template<typename T>
inline void ot::MatrixVectorizedXAlign<T>::pushColumnBack(const std::vector<T>& _col) {
	OTAssert(_col.size() == m_size.y, "Column size mismatch");
	for (size_t r = 0; r < m_size.y; ++r) {
		m_data.insert(m_data.begin() + (r + 1) * m_size.x + r, _col[r]);
	}
	m_size.x++;
}

template<typename T>
inline void ot::MatrixVectorizedXAlign<T>::pushColumnBack(std::vector<T>&& _col) {
	OTAssert(_col.size() == m_size.y, "Column size mismatch");
	for (size_t r = 0; r < m_size.y; ++r) {
		m_data.insert(m_data.begin() + (r + 1) * m_size.x + r, std::move(_col[r]));
	}
	m_size.x++;
}

template<typename T>
inline void ot::MatrixVectorizedXAlign<T>::setData(const MatrixIndex& _dimensions, std::vector<T>&& _data) {
	m_size = _dimensions;
	m_data = std::move(_data);
}
