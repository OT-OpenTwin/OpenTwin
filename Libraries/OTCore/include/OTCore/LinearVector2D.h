//! @file LinearVector2D.h
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTAssert.h"

// std header
#include <vector>

namespace ot {

	//! @class LinearVector2D
	//! @brief A 2D container implemented as a linear vector.
	//! The LinearVector2D provides a dynamically resizable 2D array stored in a single linear vector.
	//! It allows efficient access and modification of elements and supports row and column operations.
	//! 
	//! When comparing to a Vector2D the linear vector is more efficient when the data dimensions are known and do not need to be changed.
	//! @tparam T Type of elements stored in the container.
	template <class T> class LinearVector2D {
	public:

		// Constructor

		//! @brief Default constructor.
		LinearVector2D();

		//! @brief Constructs a 2D vector with given dimensions.
		//! @param _cols Number of columns.
		//! @param _rows Number of rows.
		LinearVector2D(size_t _cols, size_t _rows);

		//! @brief Constructs a 2D vector with given dimensions and default value.
		//! @param _cols Number of columns.
		//! @param _rows Number of rows.
		//! @param _default Default value for all elements.
		LinearVector2D(size_t _cols, size_t _rows, const T& _default);

		//! @brief Constructs a 2D vector from the provided other vector by copying the data.
		//! @param _other Other vector.
		LinearVector2D(const LinearVector2D& _other);

		//! @brief Constructs a 2D vector from the provided other vector by moving the data.
		//! @param _other Other vector.
		LinearVector2D(LinearVector2D&& _other) noexcept;

		//! @brief Destructor.
		virtual ~LinearVector2D() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operator

		//! @brief Assignment operator.
		//! Clears the current data structure and copies the data from the provided vector.
		//! @param _other Other vector.
		//! @return Returns reference to this vector.
		LinearVector2D& operator = (const LinearVector2D& _other);

		//! @brief Move-Assignment operator.
		//! Clears the current data structure by replacing (moving) the data the provided vector.
		//! The provided vector will remain in an undefined state, call clear to ensure a true clear state after the move.
		//! @param _other Other vector.
		//! @return Returns reference to this vector.
		LinearVector2D& operator = (LinearVector2D&& _other) noexcept;

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
		size_t getColumnCount(void) const { return m_cols; };

		//! @brief Current row count.
		size_t getRowCount(void) const { return m_rows; };

		//! @brief Removes all the data and sets the dimensions back to 0, 0.
		//! Complexity: O(r * c) where r = _rows, c = _cols.
		void clear(void);

		//! @brief Resizes the 2D vector.
		//! Complexity: O(r * c) where r = _rows, c = _cols.
		//! @param _cols New number of columns.
		//! @param _rows New number of rows.
		void resize(size_t _cols, size_t _rows) { this->resize(_cols, _rows, static_cast<const T>(0)); };

		//! @brief Resizes the 2D vector with a default value.
		//! Complexity: O(r * c) where r = _rows, c = _cols.
		//! @param _cols New number of columns.
		//! @param _rows New number of rows.
		//! @param _default Default value for new elements.
		void resize(size_t _cols, size_t _rows, const T& _default);

		//! @brief Sets the value of a specific element.
		//! Complexity: O(1).
		//! @param _col Column index.
		//! @param _row Row index.
		//! @param _data Value to set.
		void setData(size_t _col, size_t _row, const T& _data);

		//! @brief Retrieves a reference to a specific element.
		//! Complexity: O(1).
		//! @param _col Column index.
		//! @param _row Row index.
		//! @return Reference to the requested element.
		T& getData(size_t _col, size_t _row);

		//! @brief Retrieves a const reference to a specific element.
		//! Complexity: O(1).
		//! @param _col Column index.
		//! @param _row Row index.
		//! @return Const reference to the requested element.
		const T& getData(size_t _col, size_t _row) const;

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
		//! @param _rows New row count.
		//! @param _cols New column count.
		//! @param _data New data.
		void setData(size_t _rows, size_t _cols, std::vector<T>&& _data);

		//! @brief Retrieves a reference to the internal data.
		//! @return Reference to the data vector.
		std::vector<T>& getData(void) { return m_data; };

		//! @brief Retrieves a const reference to the internal data.
		//! @return Const reference to the data vector.
		const std::vector<T>& getData(void) const { return m_data; };

	private:
		size_t m_rows; //! @brief Number of rows.
		size_t m_cols; //! @brief Number of columns.
		std::vector<T> m_data; //! @brief Linear vector storage.
	};

}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor

template<class T>
ot::LinearVector2D<T>::LinearVector2D()
	: m_rows(0), m_cols(0) 
{}

template<class T>
ot::LinearVector2D<T>::LinearVector2D(size_t _cols, size_t _rows) :
	LinearVector2D() 
{ 
	this->resize(_cols, _rows); 
}

template<class T>
ot::LinearVector2D<T>::LinearVector2D(size_t _cols, size_t _rows, const T& _default) :
	LinearVector2D()
{
	this->resize(_cols, _rows, _default);
}

template<class T>
ot::LinearVector2D<T>::LinearVector2D(const LinearVector2D& _other) :
	m_rows(_other.m_rows), m_cols(_other.m_cols), m_data(_other.m_data)
{}

template<class T>
ot::LinearVector2D<T>::LinearVector2D(LinearVector2D&& _other) noexcept :
	m_rows(std::move(_other.m_rows)), std::move(m_cols(_other.m_cols)), std::move(m_data(_other.m_data))
{}

// ###########################################################################################################################################################################################################################################################################################################################

// Operator

template<class T>
ot::LinearVector2D<T>& ot::LinearVector2D<T>::operator=(const LinearVector2D& _other) {
	if (this != &_other) {
		m_rows = _other.m_rows;
		m_cols = _other.m_cols;
		m_data = _other.m_data;
	}

	return *this;
}

template<class T>
ot::LinearVector2D<T>& ot::LinearVector2D<T>::operator=(LinearVector2D&& _other) noexcept {
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
void ot::LinearVector2D<T>::clear(void) {
	m_rows = 0;
	m_cols = 0;
	m_data.clear();
}

template<typename T>
void ot::LinearVector2D<T>::resize(size_t _cols, size_t _rows, const T& _default) {
	std::vector<T> tmp = std::move(m_data);
	m_data.clear();

	size_t newSize = _cols * _rows;
	m_data.reserve(newSize);

	for (size_t newR = 0; newR < _rows; newR++) {
		for (size_t newC = 0; newC < _cols; newC++) {
			if (newR < m_rows && newC < m_cols) {
				size_t oldIx = (newR * m_cols) + newC;
				m_data.push_back(std::move(tmp[oldIx]));
			}
			else {
				m_data.push_back(_default);
			}
		}
	}

	m_cols = _cols;
	m_rows = _rows;
}

template<typename T>
inline void ot::LinearVector2D<T>::setData(size_t _col, size_t _row, const T& _data) {
	OTAssert(((_row * m_cols) + _col) < m_data.size(), "Index out of range");
	m_data[(_row * m_cols) + _col] = _data;
}

template<typename T>
inline T& ot::LinearVector2D<T>::getData(size_t _col, size_t _row) {
	OTAssert(((_row * m_cols) + _col) < m_data.size(), "Index out of range");
	return m_data[(_row * m_cols) + _col];
}

template<typename T>
inline const T& ot::LinearVector2D<T>::getData(size_t _col, size_t _row) const {
	OTAssert(((_row * m_cols) + _col) < m_data.size(), "Index out of range");
	return m_data[(_row * m_cols) + _col];
}

template<typename T>
inline void ot::LinearVector2D<T>::pushRowFront(const T& _defaultValue) {
	m_data.insert(m_data.begin(), m_cols, _defaultValue);
	m_rows++;
}

template<typename T>
inline void ot::LinearVector2D<T>::pushRowFront(const std::vector<T>& _row) {
	OTAssert(_row.size() == m_cols, "Row size mismatch");
	m_data.insert(m_data.begin(), _row.begin(), _row.end());
	m_rows++;
}

template<typename T>
inline void ot::LinearVector2D<T>::pushRowFront(std::vector<T>&& _row) {
	OTAssert(_row.size() == m_cols, "Row size mismatch");
	m_data.insert(m_data.begin(), std::make_move_iterator(_row.begin()), std::make_move_iterator(_row.end()));
	m_rows++;
}

template<typename T>
inline void ot::LinearVector2D<T>::pushRowBack(const T& _defaultValue) {
	m_data.insert(m_data.end(), m_cols, _defaultValue);
	m_rows++;
}

template<typename T>
inline void ot::LinearVector2D<T>::pushRowBack(const std::vector<T>& _row) {
	OTAssert(_row.size() == m_cols, "Row size mismatch");
	m_data.insert(m_data.end(), _row.begin(), _row.end());
	m_rows++;
}

template<typename T>
inline void ot::LinearVector2D<T>::pushRowBack(std::vector<T>&& _row) {
	OTAssert(_row.size() == m_cols, "Row size mismatch");
	m_data.insert(m_data.end(), std::make_move_iterator(_row.begin()), std::make_move_iterator(_row.end()));
	m_rows++;
}

template<typename T>
inline void ot::LinearVector2D<T>::pushColumnFront(const T& _defaultValue) {
	for (size_t r = 0; r < m_rows; ++r) {
		m_data.insert(m_data.begin() + r * (m_cols + 1), _defaultValue);
	}
	m_cols++;
}

template<typename T>
inline void ot::LinearVector2D<T>::pushColumnFront(const std::vector<T>& _col) {
	OTAssert(_col.size() == m_rows, "Column size mismatch");
	for (size_t r = 0; r < m_rows; ++r) {
		m_data.insert(m_data.begin() + r * (m_cols + 1), _col[r]);
	}
	m_cols++;
}

template<typename T>
inline void ot::LinearVector2D<T>::pushColumnFront(std::vector<T>&& _col) {
	OTAssert(_col.size() == m_rows, "Column size mismatch");
	for (size_t r = 0; r < m_rows; ++r) {
		m_data.insert(m_data.begin() + r * (m_cols + 1), std::move(_col[r]));
	}
	m_cols++;
}

template<typename T>
inline void ot::LinearVector2D<T>::pushColumnBack(const T& _defaultValue) {
	for (size_t r = 0; r < m_rows; ++r) {
		m_data.insert(m_data.begin() + (r + 1) * m_cols + r, _defaultValue);
	}
	m_cols++;
}

template<typename T>
inline void ot::LinearVector2D<T>::pushColumnBack(const std::vector<T>& _col) {
	OTAssert(_col.size() == m_rows, "Column size mismatch");
	for (size_t r = 0; r < m_rows; ++r) {
		m_data.insert(m_data.begin() + (r + 1) * m_cols + r, _col[r]);
	}
	m_cols++;
}

template<typename T>
inline void ot::LinearVector2D<T>::pushColumnBack(std::vector<T>&& _col) {
	OTAssert(_col.size() == m_rows, "Column size mismatch");
	for (size_t r = 0; r < m_rows; ++r) {
		m_data.insert(m_data.begin() + (r + 1) * m_cols + r, std::move(_col[r]));
	}
	m_cols++;
}

template<class T>
inline void ot::LinearVector2D<T>::setData(size_t _rows, size_t _cols, std::vector<T>&& _data) {
	m_rows = _rows;
	m_cols = _cols;
	m_data = std::move(_data);
}
