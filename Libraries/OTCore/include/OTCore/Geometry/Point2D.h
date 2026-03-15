// @otlicense
// File: Point2D.h
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
#include "OTCore/Serializable.h"

namespace ot {

	class Point2D;
	class Point2DF;
	class Point2DD;

	//! @class Point2D
	//! @brief 2D Point with integer values.
	class OT_CORE_API_EXPORT Point2D : public ot::Serializable 
	{
		OT_DECL_DEFCOPY(Point2D)
		OT_DECL_DEFMOVE(Point2D)
	public:
		Point2D() : m_x(0), m_y(0) {};
		Point2D(int _x, int _y) : m_x(_x), m_y(_y) {};
		Point2D(const ConstJsonObject& _jsonObject);
		~Point2D() = default;

		constexpr bool operator == (const Point2D& _other) const { return m_x == _other.m_x && m_y == _other.m_y; };
		constexpr bool operator != (const Point2D& _other) const { return m_x != _other.m_x || m_y != _other.m_y; };
		
		inline Point2D operator + (const Point2D& _other) const { return Point2D(m_x + _other.m_x, m_y + _other.m_y); };
		inline Point2D& operator += (const Point2D& _other) { m_x += _other.m_x; m_y += _other.m_y; return *this; };
		inline Point2D operator - (const Point2D& _other) const { return Point2D(m_x - _other.m_x, m_y -_other.m_y); };
		inline Point2D& operator -= (const Point2D& _other) { m_x -= _other.m_x; m_y -= _other.m_y; return *this; };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the current X value
		constexpr int getX() const { return m_x; };

		//! @brief Returns the current Y value
		constexpr int getY() const { return m_y; };

		//! @brief Set the X value
		//! @param _x The X value to set
		inline void setX(int _x) { m_x = _x; };

		//! @brief Set the Y value
		//! @param _y The Y value to set
		inline void setY(int _y) { m_y = _y; };

		//! @brief Set the X and Y value
		//! @param _x The X value to set
		//! @param _y The y value to set
		inline void set(int _x, int _y) { m_x = _x; m_y = _y; };

		//! @brief Returns a new Point with the lowest coordinates.
		inline Point2D minPoint(const Point2D& _other) const { return ot::Point2D(std::min<int>(m_x, _other.m_x), std::min<int>(m_y, _other.m_y)); }

		//! @brief Returns a new Point with the greatest coordinates.
		inline Point2D maxPoint(const Point2D& _other) const { return ot::Point2D(std::max<int>(m_x, _other.m_x), std::max<int>(m_y, _other.m_y)); }

		//! @brief Moves the x value towards negative infinity bound to the value specified.
		//! If the current value is lower than the bound it will be ignored.
		inline Point2D& moveLeft(int _leftBound) 
		{ 
			m_x = std::min<int>(m_x, _leftBound); 
			return *this; 
		}

		//! @brief Moves the y value towards negative infinity bound to the value specified.
		//! If the current value is lower than the bound it will be ignored.
		inline Point2D& moveTop(int _topBound)
		{ 
			m_y = std::min<int>(m_y, _topBound); 
			return *this; 
		}

		//! @brief Moves the x value towards positive infinity bound to the value specified.
		//! If the current value is greater than the bound it will be ignored.
		inline Point2D& moveRight(int _rightBound)
		{ 
			m_x = std::max<int>(m_x, _rightBound); 
			return *this; 
		}

		//! @brief Moves the y value towards positive infinity bound to the value specified.
		//! If the current value is greater than the bound it will be ignored.
		inline Point2D& moveBottom(int _bottomBount)
		{ 
			m_y = std::max<int>(m_y, _bottomBount); 
			return *this; 
		}

		inline Point2DF toPoint2DF() const;

		inline Point2DD toPoint2DD() const;

	private:
		int	m_x; //! The current X value
		int m_y; //! The current Y value
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @class Point2DF
	//! @brief 2D Point with float values
	class OT_CORE_API_EXPORT Point2DF : public ot::Serializable 
	{
		OT_DECL_DEFCOPY(Point2DF)
		OT_DECL_DEFMOVE(Point2DF)
	public:
		Point2DF() : m_x(0.f), m_y(0.f) {};
		Point2DF(float _x, float _y) : m_x(_x), m_y(_y) {};
		Point2DF(const ConstJsonObject& _jsonObject);
		~Point2DF() = default;

		constexpr bool operator == (const Point2DF& _other) const { return m_x == _other.m_x && m_y == _other.m_y; };
		constexpr bool operator != (const Point2DF& _other) const { return m_x != _other.m_x || m_y != _other.m_y; };

		inline Point2DF operator + (const Point2DF& _other) const { return Point2DF(m_x + _other.m_x, m_y + _other.m_y); };
		inline Point2DF& operator += (const Point2DF& _other) { m_x += _other.m_x; m_y += _other.m_y; return *this; };
		inline Point2DF operator - (const Point2DF& _other) const { return Point2DF(m_x - _other.m_x, m_y - _other.m_y); };
		inline Point2DF& operator -= (const Point2DF& _other) { m_x -= _other.m_x; m_y -= _other.m_y; return *this; };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the current X value
		constexpr float getX() const { return m_x; };

		//! @brief Returns the current Y value
		constexpr float getY() const { return m_y; };

		//! @brief Set the X value
		//! @param _x The X value to set
		inline void setX(float _x) { m_x = _x; };

		//! @brief Set the Y value
		//! @param _y The Y value to set
		inline void setY(float _y) { m_y = _y; };

		//! @brief Set the X and Y value
		//! @param _x The X value to set
		//! @param _y The y value to set
		inline void set(float _x, float _y) { m_x = _x; m_y = _y; };

		//! @brief Returns a new Point with the lowest coordinates.
		inline Point2DF minPoint(const Point2DF& _other) const { return ot::Point2DF(std::min<float>(m_x, _other.m_x), std::min<float>(m_y, _other.m_y)); }

		//! @brief Returns a new Point with the greatest coordinates.
		inline Point2DF maxPoint(const Point2DF& _other) const { return ot::Point2DF(std::max<float>(m_x, _other.m_x), std::max<float>(m_y, _other.m_y)); };

		//! @brief Moves the x value towards negative infinity bound to the value specified.
		//! If the current value is lower than the bound it will be ignored.
		inline Point2DF& moveLeft(float _leftBound) { m_x = std::min<float>(m_x, _leftBound); return *this; };

		//! @brief Moves the y value towards negative infinity bound to the value specified.
		//! If the current value is lower than the bound it will be ignored.
		inline Point2DF& moveTop(float _topBound) { m_y = std::min<float>(m_y, _topBound); return *this; };

		//! @brief Moves the x value towards positive infinity bound to the value specified.
		//! If the current value is greater than the bound it will be ignored.
		inline Point2DF& moveRight(float _rightBound) { m_x = std::max<float>(m_x, _rightBound); return *this; };

		//! @brief Moves the y value towards positive infinity bound to the value specified.
		//! If the current value is greater than the bound it will be ignored.
		inline Point2DF& moveBottom(float _bottomBount) { m_y = std::max<float>(m_y, _bottomBount); return *this; };

		inline Point2D toPoint2D() const;

		inline Point2DD toPoint2DD() const;

	private:
		float m_x; //! The current X value
		float m_y; //! The current Y value
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @class Point2DD
	//! @brief 2D Point with double values
	class OT_CORE_API_EXPORT Point2DD : public ot::Serializable {
		OT_DECL_DEFCOPY(Point2DD)
		OT_DECL_DEFMOVE(Point2DD)
	public:
		Point2DD() : m_x(0.), m_y(0.) {};
		Point2DD(double _x, double _y) : m_x(_x), m_y(_y) {};
		Point2DD(const ConstJsonObject& _jsonObject);
		~Point2DD() = default;

		constexpr bool operator == (const Point2DD& _other) const { return m_x == _other.m_x && m_y == _other.m_y; };
		constexpr bool operator != (const Point2DD& _other) const { return m_x != _other.m_x || m_y != _other.m_y; };

		inline Point2DD operator + (const Point2DD& _other) const { return Point2DD(m_x + _other.m_x, m_y + _other.m_y); };
		inline Point2DD& operator += (const Point2DD& _other) { m_x += _other.m_x; m_y += _other.m_y; return *this; };
		inline Point2DD operator - (const Point2DD& _other) const { return Point2DD(m_x - _other.m_x, m_y - _other.m_y); };
		inline Point2DD& operator -= (const Point2DD& _other) { m_x -= _other.m_x; m_y -= _other.m_y; return *this; };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the current X value
		constexpr double getX() const { return m_x; };

		//! @brief Returns the current Y value
		constexpr double getY() const { return m_y; };

		//! @brief Set the X value
		//! @param _x The X value to set
		inline void setX(double _x) { m_x = _x; };

		//! @brief Set the Y value
		//! @param _y The Y value to set
		inline void setY(double _y) { m_y = _y; };

		//! @brief Set the X and Y value
		//! @param _x The X value to set
		//! @param _y The y value to set
		inline void set(double _x, double _y) { m_x = _x; m_y = _y; };

		//! @brief Returns a new Point with the lowest coordinates.
		inline Point2DD minPoint(const Point2DD& _other) const { return ot::Point2DD(std::min<double>(m_x, _other.m_x), std::min<double>(m_y, _other.m_y)); }

		//! @brief Returns a new Point with the greatest coordinates.
		inline Point2DD maxPoint(const Point2DD& _other) const { return ot::Point2DD(std::max<double>(m_x, _other.m_x), std::max<double>(m_y, _other.m_y)); };

		//! @brief Moves the x value towards negative infinity bound to the value specified.
		//! If the current value is lower than the bound it will be ignored.
		inline Point2DD& moveLeft(double _leftBound) { m_x = std::min<double>(m_x, _leftBound); return *this; };

		//! @brief Moves the y value towards negative infinity bound to the value specified.
		//! If the current value is lower than the bound it will be ignored.
		inline Point2DD& moveTop(double _topBound) { m_y = std::min<double>(m_y, _topBound); return *this; };

		//! @brief Moves the x value towards positive infinity bound to the value specified.
		//! If the current value is greater than the bound it will be ignored.
		inline Point2DD& moveRight(double _rightBound) { m_x = std::max<double>(m_x, _rightBound); return *this; };

		//! @brief Moves the y value towards positive infinity bound to the value specified.
		//! If the current value is greater than the bound it will be ignored.
		inline Point2DD& moveBottom(double _bottomBount) { m_y = std::max<double>(m_y, _bottomBount); return *this; };

		inline Point2D toPoint2D() const;

		inline Point2DF toPoint2DF() const;

	private:
		double m_x; //! The current X value
		double m_y; //! The current Y value
	};

}

#include "OTCore/Geometry/Point2D.hpp"