//! @file Vector2D.h
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTAssert.h"

// std header
#include <vector>

namespace ot {

	//! @class Vector2D
	//! @brief A 2D container implemented as a nested vector container for generic types.
	//! The Vector2D provides a dynamically resizable 2D array where
	//! the data is stored in a vector of columns and each column vector contains the actual entries.
	//! @tparam T The type of elements stored in the container.
	template <typename T>
	class Vector2D {
	public:

		// Constructor

		//! @brief Default constructor.
		Vector2D();

		//! @brief Constructs a 2D vector with given dimensions, initialized to 0.
		//! @param _cols Number of columns.
		//! @param _rows Number of rows.
		Vector2D(size_t _cols, size_t _rows);

		//! @brief Constructs a 2D vector with given dimensions and default value.
		//! @param _cols Number of columns.
		//! @param _rows Number of rows.
		//! @param _default Default value for initialization.
		Vector2D(size_t _cols, size_t _rows, const T& _default);

		//! @brief Constructs a 2D vector from the provided other vector by copying the data.
		//! @param _other Other vector.
		Vector2D(const Vector2D& _other);

		//! @brief Constructs a 2D vector from the provided other vector by moving the data.
		//! @param _other Other vector.
		Vector2D(Vector2D&& _other);

		//! @brief Destructor.
		virtual ~Vector2D() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operator

		Vector2D& operator = (const Vector2D&);
		Vector2D& operator = (Vector2D&&);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Data handling
		
		//! @brief Current column count.
		size_t getColumnCount(void) const { return m_cols; };

		//! @brief Current row count.
		size_t getRowCount(void) const { return m_rows; };

		//! @brief Removes all the data and sets the dimensions back to 0, 0.
		//! Complexity: O(r * c) where r = _rows, c = _cols.
		void clear(void);

		//! @brief Resizes the vector to the given dimensions.
		//! Complexity: O(r * c) where r = _rows and c = _cols.
		//! @param _cols New number of columns.
		//! @param _rows New number of rows .
		void resize(size_t _cols, size_t _rows);

		//! @brief Resizes the vector with a default value.
		//! Complexity: O(r * c) where r = _rows, c = _cols.
		//! @param _cols New number of columns.
		//! @param _rows New number of rows.
		//! @param _default Default value for new elements.
		void resize(size_t _cols, size_t _rows, const T& _default);

		//! @brief Sets the data at a specific position.
		//! Complexity: O(1).
		//! @param _col Column index.
		//! @param _row Row index.
		//! @param _data Value to set.
		void setData(size_t _col, size_t _row, const T& _data);

		//! @brief Retrieves a reference to the data at a specific position.
		//! Complexity: O(1).
		//! @param _col Column index.
		//! @param _row Row index.
		//! @return Reference to the element.
		T& getData(size_t _col, size_t _row);

		//! @brief Retrieves a constant reference to the data at a specific position.
		//! Complexity: O(1).
		//! @param _col Column index.
		//! @param _row Row index.
		//! @return Constant reference to the element.
		const T& getData(size_t _col, size_t _row) const;

		//! @brief Pushes a row to the front, initializing with a default value.
		//! Complexity: O(c) where c = number of columns.
		//! @param _defaultValue Default value for new elements.
		void pushRowFront(const T& _defaultValue);

		//! @brief Pushes a row to the front.
		//! Complexity: O(c) where c = number of columns.
		//! @param _row Row data to insert.
		void pushRowFront(const std::vector<T>& _row);

		//! @brief Pushes a row to the front using move semantics.
		//! Complexity: O(c) where c = number of columns.
		//! @param _row Row data to insert.
		void pushRowFront(std::vector<T>&& _row);

		//! @brief Pushes a row to the back, initializing with a default value.
		//! Complexity: O(c) where c = number of columns.
		//! @param _defaultValue Default value for new elements.
		void pushRowBack(const T& _defaultValue);

		//! @brief Pushes a row to the back.
		//! Complexity: O(c) where c = number of columns.
		//! @param _row Row data to insert.
		void pushRowBack(const std::vector<T>& _row);

		//! @brief Pushes a row to the back using move semantics.
		//! Complexity: O(c) where c = number of columns.
		//! @param _row Row data to insert.
		void pushRowBack(std::vector<T>&& _row);

		//! @brief Pushes a column to the front, initializing with a default value.
		//! Complexity: O(r) where r = number of rows.
		//! @param _defaultValue Default value for new elements.
		void pushColumnFront(const T& _defaultValue);

		//! @brief Pushes a column to the front.
		//! Complexity: O(r) where r = number of rows.
		//! @param _col Column data to insert.
		void pushColumnFront(const std::vector<T>& _col);

		//! @brief Pushes a column to the front using move semantics.
		//! Complexity: O(r) where r = number of rows.
		//! @param _col Column data to insert.
		void pushColumnFront(std::vector<T>&& _col);

		//! @brief Pushes a column to the back, initializing with a default value.
		//! Complexity: O(r) where r = number of rows.
		//! @param _defaultValue Default value for new elements.
		void pushColumnBack(const T& _defaultValue);

		//! @brief Pushes a column to the back.
		//! Complexity: O(r) where r = number of rows.
		//! @param _col Column data to insert.
		void pushColumnBack(const std::vector<T>& _col);

		//! @brief Pushes a column to the back using move semantics.
		//! Complexity: O(r) where r = number of rows.
		//! @param _col Column data to insert.
		void pushColumnBack(std::vector<T>&& _col);

		//! @brief Replaces the current data.
		//! Complexity: O(1) data is moved.
		//! @param _rows New row count.
		//! @param _cols New column count.
		//! @param _data New data.
		void setData(size_t _rows, size_t _cols, std::vector<std::vector<T>>&& _data);

		//! @brief Retrieves the modifiable data structure.
		//! @return Reference to the stored data.
		std::vector<std::vector<T>>& getData(void) { return m_data; };

		//! @brief Retrieves the entire data structure.
		//! @return Constant reference to the stored data.
		const std::vector<std::vector<T>>& getData(void) const { return m_data; };

	private:
		size_t m_rows; //! @brief Number of rows.
		size_t m_cols; //! @brief Number of columns.
		std::vector<std::vector<T>> m_data; //! @brief Row to column vector storage.
	};

}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor

template<typename T>
ot::Vector2D<T>::Vector2D() : 
	m_rows(0), m_cols(0)
{}

template<typename T>
ot::Vector2D<T>::Vector2D(size_t _cols, size_t _rows) :
	Vector2D() 
{
	this->resize(_cols, _rows);
}

template<typename T>
ot::Vector2D<T>::Vector2D(size_t _cols, size_t _rows, const T& _default) :
	Vector2D()
{
	this->resize(_cols, _rows, _default); 
}

template<typename T>
ot::Vector2D<T>::Vector2D(const Vector2D& _other) :
	m_rows(_other.m_rows), m_cols(_other.m_cols), m_data(_other.m_data)
{

}

template<typename T>
ot::Vector2D<T>::Vector2D(Vector2D&& _other) :
	m_rows(std::move(_other.m_rows)), std::move(m_cols(_other.m_cols)), std::move(m_data(_other.m_data))
{

}

// ###########################################################################################################################################################################################################################################################################################################################

// Operator

template<class T>
ot::Vector2D<T>& ot::Vector2D<T>::operator=(const Vector2D& _other) {
	if (this != &_other) {
		m_rows = _other.m_rows;
		m_cols = _other.m_cols;
		m_data = _other.m_data;
	}

	return *this;
}

template<class T>
ot::Vector2D<T>& ot::Vector2D<T>::operator=(Vector2D&& _other) noexcept {
	if (this != &_other) {
		m_rows = std::move(_other.m_rows);
		m_cols = std::move(_other.m_cols);
		m_data = std::move(_other.m_data);
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data handling

template<typename T>
void ot::Vector2D<T>::clear(void) {
	m_rows = 0;
	m_cols = 0;
	m_data.clear();
}

template<typename T>
void ot::Vector2D<T>::resize(size_t _cols, size_t _rows) {
	this->resize(_cols, _rows, static_cast<const T>(0));
}

template<typename T>
void ot::Vector2D<T>::resize(size_t _cols, size_t _rows, const T& _default) {
	std::vector<std::vector<T>> tmp = std::move(m_data);
	m_data.clear();

	m_data.reserve(_rows);

	for (size_t newR = 0; newR < _cols; newR++) {
		std::vector<T> newRow;
		newRow.reserve(_cols);
		for (size_t newC = 0; newC < _cols; newC++) {
			if (newR < m_rows && newC < m_cols) {
				newRow.push_back(std::move(tmp[newR][newC]));
			}
			else {
				newRow.push_back(_default);
			}
		}

		m_data.push_back(newRow);
	}

	m_cols = _cols;
	m_rows = _rows;
}

template<typename T>
inline void ot::Vector2D<T>::setData(size_t _col, size_t _row, const T& _data) {
	OTAssert(_row < m_data.size(), "Row index out of range");
	OTAssert(_col < m_data[_row].size(), "Column index out of range");
	m_data[_row][_col] = _data;
}

template<typename T>
inline T& ot::Vector2D<T>::getData(size_t _col, size_t _row) {
	OTAssert(_row < m_data.size(), "Row index out of range");
	OTAssert(_col < m_data[_row].size(), "Column index out of range");
	return m_data[_row][_col];
}

template<typename T>
inline const T& ot::Vector2D<T>::getData(size_t _col, size_t _row) const {
	OTAssert(_row < m_data.size(), "Row index out of range");
	OTAssert(_col < m_data[_row].size(), "Column index out of range");
	return m_data[_row][_col];
}

template<typename T>
inline void ot::Vector2D<T>::pushRowFront(const T& _defaultValue) {
	std::vector<T> newRow(m_cols, _defaultValue);
	m_data.insert(m_data.begin(), std::move(newRow));
	m_rows++;
}

template<typename T>
inline void ot::Vector2D<T>::pushRowFront(const std::vector<T>& _row) {
	OTAssert(_row.size() == m_cols, "Row size mismatch");
	m_data.insert(m_data.begin(), _row);
	m_rows++;
}

template<typename T>
inline void ot::Vector2D<T>::pushRowFront(std::vector<T>&& _row) {
	OTAssert(_row.size() == m_cols, "Row size mismatch");
	m_data.insert(m_data.begin(), std::move(_row));
	m_rows++;
}

template<typename T>
inline void ot::Vector2D<T>::pushRowBack(const T& _defaultValue) {
	std::vector<T> newRow(m_cols, _defaultValue);
	m_data.push_back(std::move(newRow));
	m_rows++;
}

template<typename T>
inline void ot::Vector2D<T>::pushRowBack(const std::vector<T>& _row) {
	OTAssert(_row.size() == m_cols, "Row size mismatch");
	m_data.push_back(_row);
	m_rows++;
}

template<typename T>
inline void ot::Vector2D<T>::pushRowBack(std::vector<T>&& _row) {
	OTAssert(_row.size() == m_cols, "Row size mismatch");
	m_data.push_back(std::move(_row));
	m_rows++;
}

template<typename T>
inline void ot::Vector2D<T>::pushColumnFront(const T& _defaultValue) {
	for (auto& row : m_data) {
		row.insert(row.begin(), _defaultValue);
	}
	m_cols++;
}

template<typename T>
inline void ot::Vector2D<T>::pushColumnFront(const std::vector<T>& _col) {
	OTAssert(_col.size() == m_rows, "Column size mismatch");
	for (size_t i = 0; i < m_rows; ++i) {
		m_data[i].insert(m_data[i].begin(), _col[i]);
	}
	m_cols++;
}

template<typename T>
inline void ot::Vector2D<T>::pushColumnFront(std::vector<T>&& _col) {
	OTAssert(_col.size() == m_rows, "Column size mismatch");
	for (size_t i = 0; i < m_rows; ++i) {
		m_data[i].insert(m_data[i].begin(), std::move(_col[i]));
	}
	m_cols++;
}

template<typename T>
inline void ot::Vector2D<T>::pushColumnBack(const T& _defaultValue) {
	for (auto& row : m_data) {
		row.push_back(_defaultValue);
	}
	m_cols++;
}

template<typename T>
inline void ot::Vector2D<T>::pushColumnBack(const std::vector<T>& _col) {
	OTAssert(_col.size() == m_rows, "Column size mismatch");
	for (size_t i = 0; i < m_rows; ++i) {
		m_data[i].push_back(_col[i]);
	}
	m_cols++;
}

template<typename T>
inline void ot::Vector2D<T>::pushColumnBack(std::vector<T>&& _col) {
	OTAssert(_col.size() == m_rows, "Column size mismatch");
	for (size_t i = 0; i < m_rows; ++i) {
		m_data[i].push_back(std::move(_col[i]));
	}
	m_cols++;
}

template<typename T>
inline void ot::Vector2D<T>::setData(size_t _rows, size_t _cols, std::vector<std::vector<T>>&& _data) {
	m_rows = _rows;
	m_cols = _cols;
	m_data = std::move(_data);
}

