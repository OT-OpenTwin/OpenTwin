#pragma once

namespace ot {

	template <class T> class Point2D {
	public:
		//! @brief Constructor
		//! @param _x The initial X value
		//! @param _y The initial Y value
		Point2D(T _x, T _y);

		//! @brief Copy constructor
		//! @param _other The other 2D point
		Point2D(const Point2D<T> & _other);

		Point2D<T> & operator = (const Point2D<T> & _other);
		Point2D<T> operator + (const Point2D<T> & _other);
		Point2D<T> & operator += (const Point2D<T> & _other);
		bool operator == (const Point2D<T> & _other);
		bool operator != (const Point2D<T> & _other);

		//! @brief Returns the current X value
		T x(void) const;

		//! @brief Returns the current Y value
		T y(void) const;

		//! @brief Will set the X value
		//! @param _x The X value to set
		void setX(T _x);

		//! @brief Will set the Y value
		//! @param _y The Y value to set
		void setY(T _y);

		//! @brief Will add the provided value to the X value and return the new X value
		//! @param _v The value to add
		T addX(T _v);

		//! @brief Will add the provided value to the Y value and return the new Y value
		//! @param _v The value to add
		T addY(T _v);

		//! @brief Will subtract the provided value from the X value and return the new X value
		//! @param _v The value to subtract
		T subX(T _v);

		//! @brief Will subtract the provided value from the Y value and return the new Y value
		//! @param _v The value to subtract
		T subY(T _v);

	private:
		// Block default constructor
		Point2D() = delete;

		T		m_x;			//! The current X value
		T		m_y;			//! The current Y value

	};

	template <class T> Point2D<T>::Point2D(T _x, T _y) { m_x = _x; m_y = _y; }

	template <class T> Point2D<T>::Point2D(const Point2D<T> & _other) { m_x = _other.x(); m_y = _other.y(); }

	template <class T> Point2D<T> & Point2D<T>::operator = (const Point2D<T> & _other) {
		m_x = _other.x(); m_y = _other.y();
		return *this;
	}

	template <class T> Point2D<T> Point2D<T>::operator + (const Point2D<T> & _other) {
		Point2D<T> ret(m_x, m_y);
		ret.addX(_other.x());
		ret.addY(_other.y());
		return ret;

	}

	template <class T> Point2D<T> & Point2D<T>::operator += (const Point2D<T> & _other) {
		m_x = m_x + _other.x();
		m_y = m_y + _other.y();
		m_z = m_z + _other.z();
		return *this;
	}

	template <class T> bool Point2D<T>::operator == (const Point2D & _other) {
		if (m_x != _other.x()) { return false; }
		if (m_y != _other.y()) { return false; }
		return true;
	}

	template <class T> bool Point2D<T>::operator != (const Point2D & _other) { return !(*this == _other); }

	template <class T> T Point2D<T>::x(void) const { return m_x; }

	template <class T> T Point2D<T>::y(void) const { return m_y; }

	template <class T> void Point2D<T>::setX(T _x) { m_x = _x; }

	template <class T> void Point2D<T>::setY(T _y) { m_y = _y; }

	template <class T> T Point2D<T>::addX(T _v) { m_x = m_x + v; return m_x; }

	template <class T> T Point2D<T>::addY(T _v) { m_y = m_y + v; return m_y; }

	template <class T> T Point2D<T>::subX(T _v) { m_x = m_x - _v; return m_x; }

	template <class T> T Point2D<T>::subY(T _v) { m_y = m_y - _v; return m_y; }

}