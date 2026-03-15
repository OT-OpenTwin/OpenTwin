// @otlicense
// File: Rect.h
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
#include "OTCore/Geometry/Size2D.h"
#include "OTCore/Geometry/Point2D.h"

namespace ot {

	class RectD;
	class RectF;

	//! @class Rect
	//! @brief Rectangle that holds the top left and bottom right corner in int values.
	class OT_CORE_API_EXPORT Rect : public Serializable {
		OT_DECL_DEFCOPY(Rect)
		OT_DECL_DEFMOVE(Rect)
	public:
		Rect();
		Rect(const Point2D& _topLeft, const Point2D& _bottomRight);
		Rect(const Point2D& _topLeft, const Size2D& _size);
		Rect(const ConstJsonObject& _jsonObject);
		~Rect() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class methods

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operators

		constexpr bool operator == (const Rect& _other) const { return m_topLeft == _other.getTopLeft() && m_bottomRight == _other.getBottomRight(); };
		constexpr bool operator != (const Rect& _other) const { return m_topLeft != _other.getTopLeft() || m_bottomRight != _other.getBottomRight(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		inline void setTop(int _top) { m_topLeft.setY(_top); };
		constexpr int getTop(void) const { return m_topLeft.getY(); };

		inline void setLeft(int _left) { m_topLeft.setX(_left); };
		constexpr int getLeft(void) const { return m_topLeft.getX(); };

		inline void setTopLeft(const Point2D& _topLeft) { m_topLeft = _topLeft; };
		constexpr const Point2D& getTopLeft(void) const { return m_topLeft; };

		inline void setBottom(int _bottom) { m_bottomRight.setY(_bottom); };
		constexpr int getBottom(void) const { return m_bottomRight.getY(); };

		inline void setRight(int _right) { m_bottomRight.setX(_right); };
		constexpr int getRight(void) const { return m_bottomRight.getX(); };

		inline void setBottomRight(const Point2D& _bottomRight) { m_bottomRight = m_bottomRight; };
		constexpr const Point2D& getBottomRight(void) const { return m_bottomRight; };

		inline void setWidth(int _width) { m_bottomRight = Point2D(m_topLeft.getX() + _width, m_topLeft.getY()); };
		constexpr int getWidth(void) const { return m_bottomRight.getX() - m_topLeft.getX(); };

		inline void setHeight(int _height) { m_bottomRight = Point2D(m_topLeft.getX(), m_topLeft.getY() + _height); };
		constexpr int getHeight(void) const { return m_bottomRight.getY() - m_topLeft.getY(); };

		inline void setSize(const Size2D& _size) { m_bottomRight = Point2D(m_topLeft.getX() + _size.getWidth(), m_topLeft.getY() + _size.getHeight()); };
		Size2D getSize() const { return Size2D(m_bottomRight.getX() - m_topLeft.getX(), m_bottomRight.getY() - m_topLeft.getY()); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Basic calculations

		//! @brief Calculates and returns the union of this Rect and the provided Rect.
		Rect unionWith(const Rect& _other) const;

		//! @brief Calclates and returns the intersection of this Rect and the provided Rect.
		Rect intersectsWith(const Rect& _other) const;

		//! @brief Returns true if top left is (0, 0) and bottom right is (0, 0).
		bool isAllZero(void) const { return m_topLeft.getX() == 0 && m_topLeft.getY() == 0 && m_bottomRight.getX() == 0 && m_bottomRight.getY() == 0; };

		//! @brief Returns true if the top left and bottom right points are set correctly.
		//! If the left value is bigger than the right value or the bottom value bigger than the top value false is returned.
		bool isValid(void) const { return (m_topLeft.getX() <= m_bottomRight.getX()) && (m_topLeft.getY() <= m_bottomRight.getY()); };

		//! @see void moveTo(const Point2D& _topLeft)
		void moveTo(int _ax, int _ay) { this->moveTo(Point2D(_ax, _ay)); };

		//! @brief Moves the rect to the specified location.
		//! The top left and bottom right will be moved.
		void moveTo(const Point2D& _topLeft);

		//! @see void moveBy(const Point2D& _distance)
		void moveBy(int _dx, int _dy) { this->moveBy(Point2D(_dx, _dy)); };

		//! @brief Moves the rect by the provided distance.
		//! The top left and bottom right will be moved.
		void moveBy(const Point2D& _distance);

		RectF toRectF(void) const;
		RectD toRectD(void) const;

	private:
		Point2D m_topLeft;
		Point2D m_bottomRight;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @class Rect
	//! @brief Rectangle that holds the top left and bottom right corner in float values.
	class OT_CORE_API_EXPORT RectF : public Serializable {
		OT_DECL_DEFCOPY(RectF)
		OT_DECL_DEFMOVE(RectF)
	public:
		RectF();
		RectF(const Point2DF& _topLeft, const Point2DF& _bottomRight);
		RectF(const Point2DF& _topLeft, const Size2DF& _size);
		RectF(const ConstJsonObject& _jsonObject);
		~RectF() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class methods

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operators

		bool operator == (const RectF& _other) const { return m_topLeft == _other.getTopLeft() && m_bottomRight == _other.getBottomRight(); };
		bool operator != (const RectF& _other) const { return m_topLeft != _other.getTopLeft() || m_bottomRight != _other.getBottomRight(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setTop(float _top) { m_topLeft.setY(_top); };
		float getTop(void) const { return m_topLeft.getY(); };

		void setLeft(float _left) { m_topLeft.setX(_left); };
		float getLeft(void) const { return m_topLeft.getX(); };

		void setTopLeft(const Point2DF& _topLeft) { m_topLeft = _topLeft; };
		const Point2DF& getTopLeft(void) const { return m_topLeft; };

		void setBottom(float _bottom) { m_bottomRight.setY(_bottom); };
		float getBottom(void) const { return m_bottomRight.getY(); };

		void setRight(float _right) { m_bottomRight.setX(_right); };
		float getRight(void) const { return m_bottomRight.getX(); };

		void setBottomRight(const Point2DF& _bottomRight) { m_bottomRight = m_bottomRight; };
		const Point2DF& getBottomRight(void) const { return m_bottomRight; };

		void setWidth(float _width) { m_bottomRight = Point2DF(m_topLeft.getX() + _width, m_topLeft.getY()); };
		float getWidth(void) const { return m_bottomRight.getX() - m_topLeft.getX(); };

		void setHeight(float _height) { m_bottomRight = Point2DF(m_topLeft.getX(), m_topLeft.getY() + _height); };
		float getHeight(void) const { return m_bottomRight.getY() - m_topLeft.getY(); };

		void setSize(const Size2DF& _size) { m_bottomRight = Point2DF(m_topLeft.getX() + _size.getWidth(), m_topLeft.getY() + _size.getHeight()); };
		Size2DF getSize(void) const { return Size2DF(m_bottomRight.getX() - m_topLeft.getX(), m_bottomRight.getY() - m_topLeft.getY()); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Basic calculations

		//! @brief Calculates and returns the union of this Rect and the provided Rect.
		RectF unionWith(const RectF& _other) const;

		//! @brief Calclates and returns the intersection of this Rect and the provided Rect.
		RectF intersectsWith(const RectF& _other) const;

		//! @brief Returns true if top left is (0, 0) and bottom right is (0, 0).
		bool isAllZero(void) const { return m_topLeft.getX() == 0 && m_topLeft.getY() == 0 && m_bottomRight.getX() == 0 && m_bottomRight.getY() == 0; };

		//! @brief Returns true if the top left and bottom right points are set correctly.
		//! If the left value is bigger than the right value or the bottom value bigger than the top value false is returned.
		bool isValid(void) const { return (m_topLeft.getX() <= m_bottomRight.getX()) && (m_topLeft.getY() <= m_bottomRight.getY()); };

		//! @see void moveTo(const Point2DF& _topLeft)
		void moveTo(float _ax, float _ay) { this->moveTo(Point2DF(_ax, _ay)); };

		//! @brief Moves the rect to the specified location.
		//! The top left and bottom right will be moved.
		void moveTo(const Point2DF& _topLeft);

		//! @see void moveBy(const Point2DF& _distance)
		void moveBy(float _dx, float _dy) { this->moveBy(Point2DF(_dx, _dy)); };

		//! @brief Moves the rect by the provided distance.
		//! The top left and bottom right will be moved.
		void moveBy(const Point2DF& _distance);

		Rect toRect(void) const;
		RectD toRectD(void) const;

	private:
		Point2DF m_topLeft;
		Point2DF m_bottomRight;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @class RectD
	//! @brief Rectangle that holds the top left and bottom right corner in double values.
	class OT_CORE_API_EXPORT RectD : public Serializable {
		OT_DECL_DEFCOPY(RectD)
		OT_DECL_DEFMOVE(RectD)
	public:
		RectD();
		RectD(const Point2DD& _topLeft, const Point2DD& _bottomRight);
		RectD(const Point2DD& _topLeft, const Size2DD& _size);
		RectD(const ConstJsonObject& _jsonObject);
		~RectD() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class methods

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Operators

		bool operator == (const RectD& _other) const { return m_topLeft == _other.getTopLeft() && m_bottomRight == _other.getBottomRight(); };
		bool operator != (const RectD& _other) const { return m_topLeft != _other.getTopLeft() || m_bottomRight != _other.getBottomRight(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setTop(double _top) { m_topLeft.setY(_top); };
		double getTop(void) const { return m_topLeft.getY(); };

		void setLeft(double _left) { m_topLeft.setX(_left); };
		double getLeft(void) const { return m_topLeft.getX(); };

		void setTopLeft(const Point2DD& _topLeft) { m_topLeft = _topLeft; };
		const Point2DD& getTopLeft(void) const { return m_topLeft; };

		void setBottom(double _bottom) { m_bottomRight.setY(_bottom); };
		double getBottom(void) const { return m_bottomRight.getY(); };

		void setRight(double _right) { m_bottomRight.setX(_right); };
		double getRight(void) const { return m_bottomRight.getX(); };

		void setBottomRight(const Point2DD& _bottomRight) { m_bottomRight = m_bottomRight; };
		const Point2DD& getBottomRight(void) const { return m_bottomRight; };

		void setWidth(double _width) { m_bottomRight = Point2DD(m_topLeft.getX() + _width, m_topLeft.getY()); };
		double getWidth(void) const { return m_bottomRight.getX() - m_topLeft.getX(); };

		void setHeight(double _height) { m_bottomRight = Point2DD(m_topLeft.getX(), m_topLeft.getY() + _height); };
		double getHeight(void) const { return m_bottomRight.getY() - m_topLeft.getY(); };

		void setSize(const Size2DD& _size) { m_bottomRight = Point2DD(m_topLeft.getX() + _size.getWidth(), m_topLeft.getY() + _size.getHeight()); };
		Size2DD getSize(void) const { return Size2DD(m_bottomRight.getX() - m_topLeft.getX(), m_bottomRight.getY() - m_topLeft.getY()); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Basic calculations

		//! @brief Calculates and returns the union of this Rect and the provided Rect.
		RectD unionWith(const RectD& _other) const;

		//! @brief Calclates and returns the intersection of this Rect and the provided Rect.
		RectD intersectsWith(const RectD& _other) const;

		//! @brief Returns true if top left is (0, 0) and bottom right is (0, 0).
		bool isAllZero(void) const { return m_topLeft.getX() == 0 && m_topLeft.getY() == 0 && m_bottomRight.getX() == 0 && m_bottomRight.getY() == 0; };

		//! @brief Returns true if the top left and bottom right points are set correctly.
		//! If the left value is bigger than the right value or the bottom value bigger than the top value false is returned.
		bool isValid(void) const { return (m_topLeft.getX() <= m_bottomRight.getX()) && (m_topLeft.getY() <= m_bottomRight.getY()); };

		//! @see void moveTo(const Point2DD& _topLeft)
		void moveTo(double _ax, double _ay) { this->moveTo(Point2DD(_ax, _ay)); };

		//! @brief Moves the rect to the specified location.
		//! The top left and bottom right will be moved.
		void moveTo(const Point2DD& _topLeft);

		//! @see void moveBy(const Point2DD& _distance)
		void moveBy(double _dx, double _dy) { this->moveBy(Point2DD(_dx, _dy)); };

		//! @brief Moves the rect by the provided distance.
		//! The top left and bottom right will be moved.
		void moveBy(const Point2DD& _distance);

		Rect toRect(void) const;
		RectF toRectF(void) const;

	private:
		Point2DD m_topLeft;
		Point2DD m_bottomRight;
	};

}