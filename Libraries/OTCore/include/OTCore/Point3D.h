// @otlicense

//! @file Point3D.h
//! @author Alexander Kuester (alexk95)
//! @date April 2021
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

namespace ot {

	//! @class Point3D
	//! @brief Three dimensional point
	template <class T> class Point3D {
	public:
		//! @brief Constructor
		//! @param _x The initial X value
		//! @param _y The initial Y value
		//! @param _z The initial Z value
		Point3D(T _x, T _y, T _z);

		//! @brief Copy constructor
		//! @param _other The other 3D point
		Point3D(const Point3D<T> & _other);

		Point3D<T> & operator = (const Point3D<T> & _other);
		Point3D<T> operator + (const Point3D<T> & _other) const;
		Point3D<T> operator - (const Point3D<T> & _other) const;
		Point3D<T> operator * (const Point3D<T> & _other) const;
		Point3D<T> operator / (const Point3D<T> & _other) const;
		Point3D<T> & operator += (const Point3D<T> & _other);
		Point3D<T> & operator -= (const Point3D<T> & _other);
		Point3D<T> & operator *= (const Point3D<T> & _other);
		Point3D<T> & operator /= (const Point3D<T> & _other);
		bool operator == (const Point3D<T> & _other);
		bool operator != (const Point3D<T> & _other);

		//! @brief Returns the current X value
		inline T x(void) const { return m_x; }

		//! @brief Returns the current Y value
		inline T y(void) const { return m_y; }

		//! @brief Returns the current Z value
		inline T z(void) const { return m_z; }

		//! @brief Will set the X value
		//! @param _x The X value to set
		void setX(T _x) { m_x = _x; }

		//! @brief Will set the Y value
		//! @param _y The Y value to set
		void setY(T _y) { m_y = _y; }

		//! @brief Will set the Z value
		//! @param _z The Z value to set
		void setZ(T _z) { m_z = _z; }

		//! @brief Will add the provided value to the X value and return the new X value
		//! @param _v The value to add
		T addX(T _v);

		//! @brief Will add the provided value to the Y value and return the new Y value
		//! @param _v The value to add
		T addY(T _v);

		//! @brief Will add the provided value to the Z value and return the new Z value
		//! @param _v The value to add
		T addZ(T _z);

		//! @brief Will subtract the provided value from the X value and return the new X value
		//! @param _v The value to subtract
		T subX(T _v);

		//! @brief Will subtract the provided value from the Y value and return the new Y value
		//! @param _v The value to subtract
		T subY(T _v);

		//! @brief Will subtract the provided value from the Z value and return the new Z value
		//! @param _v The value to subtract
		T subZ(T _v);

	private:
		// Block default constructor
		Point3D() = delete;

		T		m_x;			//! The current X value
		T		m_y;			//! The current Y value
		T		m_z;			//! The current Z value

	}; // class Point3D

	template <class T> Point3D<T>::Point3D(T _x, T _y, T _z) { m_x = _x; m_y = _y; m_z = _z; }

	template <class T> Point3D<T>::Point3D(const Point3D<T> & _other) { m_x = _other.x(); m_y = _other.y(); m_z = _other.z(); }

	template <class T> Point3D<T> & Point3D<T>::operator = (const Point3D<T> & _other) {
		m_x = _other.x(); m_y = _other.y(); m_z = _other.z();
		return *this;
	}

	template <class T> Point3D<T> Point3D<T>::operator + (const Point3D<T> & _other) const {
		Point3D<T> ret(m_x + _other.m_x, m_y + _other.m_y, m_z + _other.m_z);
		return ret;
	}

	template <class T> Point3D<T> Point3D<T>::operator - (const Point3D<T> & _other) const {
		Point3D<T> ret(m_x - _other.m_x, m_y - _other.m_y, m_z - _other.m_z);
		return ret;
	}

	template <class T> Point3D<T> Point3D<T>::operator * (const Point3D<T> & _other) const {
		Point3D<T> ret(m_x * _other.m_x, m_y * _other.m_y, m_z * _other.m_z);
		return ret;
	}

	template <class T> Point3D<T> Point3D<T>::operator / (const Point3D<T> & _other) const {
		T x = m_x;
		T y = m_y;
		T z = m_z;
		if (_other.m_x != 0) { x /= _other.m_x; }
		if (_other.m_y != 0) { y /= _other.m_y; }
		if (_other.m_z != 0) { z /= _other.m_z; }
		Point3D<T> ret(x, y, z);
		return ret;
	}

	template <class T>  Point3D<T> & Point3D<T>::operator += (const Point3D<T> & _other) {
		m_x = m_x + _other.m_x;
		m_y = m_y + _other.m_y;
		m_z = m_z + _other.m_z;
		return *this;
	}

	template <class T>  Point3D<T> & Point3D<T>::operator -= (const Point3D<T> & _other) {
		m_x = m_x - _other.m_x;
		m_y = m_y - _other.m_y;
		m_z = m_z - _other.m_z;
		return *this;
	}

	template <class T>  Point3D<T> & Point3D<T>::operator *= (const Point3D<T> & _other) {
		m_x = m_x * _other.m_x;
		m_y = m_y * _other.m_y;
		m_z = m_z * _other.m_z;
		return *this;
	}

	template <class T>  Point3D<T> & Point3D<T>::operator /= (const Point3D<T> & _other) {
		if (_other.m_x != 0) { m_x /= _other.m_x; }
		if (_other.m_y != 0) { m_y /= _other.m_y; }
		if (_other.m_z != 0) { m_z /= _other.m_z; }
		return *this;
	}

	template <class T> bool Point3D<T>::operator == (const Point3D<T> & _other) {
		if (m_x != _other.x()) { return false; }
		if (m_y != _other.y()) { return false; }
		if (m_z != _other.z()) { return false; }
		return true;
	}

	template <class T> bool Point3D<T>::operator != (const Point3D<T> & _other) { return !(*this == _other); }
	
	template <class T> T Point3D<T>::addX(T _v) { m_x = m_x + _v; return m_x; }

	template <class T> T Point3D<T>::addY(T _v) { m_y = m_y + _v; return m_y; }

	template <class T> T Point3D<T>::addZ(T _v) { m_z = m_z + _v; return m_z; }

	template <class T> T Point3D<T>::subX(T _v) { m_x = m_x - _v; return m_x; }

	template <class T> T Point3D<T>::subY(T _v) { m_y = m_y - _v; return m_y; }

	template <class T> T Point3D<T>::subZ(T _v) { m_z = m_z - _v; return m_z; }

}