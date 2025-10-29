// @otlicense

//! @file Rect.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Size2D.h"
#include "OTCore/Point2D.h"

namespace ot {

	class RectD;
	class RectF;

	//! @class Rect
	//! @brief Rectangle that holds the top left and bottom right corner in int values.
	class OT_CORE_API_EXPORT Rect : public Serializable {
	public:
		Rect();
		Rect(const Point2D& _topLeft, const Point2D& _bottomRight);
		Rect(const Point2D& _topLeft, const Size2D& _size);
		Rect(const Rect& _other);

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

		bool operator = (const Rect& _other) { m_topLeft = _other.getTopLeft(); m_bottomRight = _other.getBottomRight(); return this; };
		bool operator == (const Rect& _other) const { return m_topLeft == _other.getTopLeft() && m_bottomRight == _other.getBottomRight(); };
		bool operator != (const Rect& _other) const { return m_topLeft != _other.getTopLeft() || m_bottomRight != _other.getBottomRight(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setTop(int _top) { m_topLeft.setY(_top); };
		int getTop(void) const { return m_topLeft.y(); };

		void setLeft(int _left) { m_topLeft.setX(_left); };
		int getLeft(void) const { return m_topLeft.x(); };

		void setTopLeft(const Point2D& _topLeft) { m_topLeft = _topLeft; };
		const Point2D& getTopLeft(void) const { return m_topLeft; };

		void setBottom(int _bottom) { m_bottomRight.setY(_bottom); };
		int getBottom(void) const { return m_bottomRight.y(); };

		void setRight(int _right) { m_bottomRight.setX(_right); };
		int getRight(void) const { return m_bottomRight.x(); };

		void setBottomRight(const Point2D& _bottomRight) { m_bottomRight = m_bottomRight; };
		const Point2D& getBottomRight(void) const { return m_bottomRight; };

		void setWidth(int _width) { m_bottomRight = Point2D(m_topLeft.x() + _width, m_topLeft.y()); };
		int getWidth(void) const { return m_bottomRight.x() - m_topLeft.x(); };

		void setHeight(int _height) { m_bottomRight = Point2D(m_topLeft.x(), m_topLeft.y() + _height); };
		int getHeight(void) const { return m_bottomRight.y() - m_topLeft.y(); };

		void setSize(const Size2D& _size) { m_bottomRight = Point2D(m_topLeft.x() + _size.width(), m_topLeft.y() + _size.height()); };
		Size2D getSize(void) const { return Size2D(m_bottomRight.x() - m_topLeft.x(), m_bottomRight.y() - m_topLeft.y()); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Basic calculations

		//! @brief Calculates and returns the union of this Rect and the provided Rect.
		Rect unionWith(const Rect& _other) const;

		//! @brief Calclates and returns the intersection of this Rect and the provided Rect.
		Rect intersectsWith(const Rect& _other) const;

		//! @brief Returns true if top left is (0, 0) and bottom right is (0, 0).
		bool isAllZero(void) const { return m_topLeft.x() == 0 && m_topLeft.y() == 0 && m_bottomRight.x() == 0 && m_bottomRight.y() == 0; };

		//! @brief Returns true if the top left and bottom right points are set correctly.
		//! If the left value is bigger than the right value or the bottom value bigger than the top value false is returned.
		bool isValid(void) const { return (m_topLeft.x() <= m_bottomRight.x()) && (m_topLeft.y() <= m_bottomRight.y()); };

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
	public:
		RectF();
		RectF(const Point2DF& _topLeft, const Point2DF& _bottomRight);
		RectF(const Point2DF& _topLeft, const Size2DF& _size);
		RectF(const RectF& _other);

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

		bool operator = (const RectF& _other) { m_topLeft = _other.getTopLeft(); m_bottomRight = _other.getBottomRight(); return this; };
		bool operator == (const RectF& _other) const { return m_topLeft == _other.getTopLeft() && m_bottomRight == _other.getBottomRight(); };
		bool operator != (const RectF& _other) const { return m_topLeft != _other.getTopLeft() || m_bottomRight != _other.getBottomRight(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setTop(float _top) { m_topLeft.setY(_top); };
		float getTop(void) const { return m_topLeft.y(); };

		void setLeft(float _left) { m_topLeft.setX(_left); };
		float getLeft(void) const { return m_topLeft.x(); };

		void setTopLeft(const Point2DF& _topLeft) { m_topLeft = _topLeft; };
		const Point2DF& getTopLeft(void) const { return m_topLeft; };

		void setBottom(float _bottom) { m_bottomRight.setY(_bottom); };
		float getBottom(void) const { return m_bottomRight.y(); };

		void setRight(float _right) { m_bottomRight.setX(_right); };
		float getRight(void) const { return m_bottomRight.x(); };

		void setBottomRight(const Point2DF& _bottomRight) { m_bottomRight = m_bottomRight; };
		const Point2DF& getBottomRight(void) const { return m_bottomRight; };

		void setWidth(float _width) { m_bottomRight = Point2DF(m_topLeft.x() + _width, m_topLeft.y()); };
		float getWidth(void) const { return m_bottomRight.x() - m_topLeft.x(); };

		void setHeight(float _height) { m_bottomRight = Point2DF(m_topLeft.x(), m_topLeft.y() + _height); };
		float getHeight(void) const { return m_bottomRight.y() - m_topLeft.y(); };

		void setSize(const Size2DF& _size) { m_bottomRight = Point2DF(m_topLeft.x() + _size.width(), m_topLeft.y() + _size.height()); };
		Size2DF getSize(void) const { return Size2DF(m_bottomRight.x() - m_topLeft.x(), m_bottomRight.y() - m_topLeft.y()); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Basic calculations

		//! @brief Calculates and returns the union of this Rect and the provided Rect.
		RectF unionWith(const RectF& _other) const;

		//! @brief Calclates and returns the intersection of this Rect and the provided Rect.
		RectF intersectsWith(const RectF& _other) const;

		//! @brief Returns true if top left is (0, 0) and bottom right is (0, 0).
		bool isAllZero(void) const { return m_topLeft.x() == 0 && m_topLeft.y() == 0 && m_bottomRight.x() == 0 && m_bottomRight.y() == 0; };

		//! @brief Returns true if the top left and bottom right points are set correctly.
		//! If the left value is bigger than the right value or the bottom value bigger than the top value false is returned.
		bool isValid(void) const { return (m_topLeft.x() <= m_bottomRight.x()) && (m_topLeft.y() <= m_bottomRight.y()); };

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
	public:
		RectD();
		RectD(const Point2DD& _topLeft, const Point2DD& _bottomRight);
		RectD(const Point2DD& _topLeft, const Size2DD& _size);
		RectD(const RectD& _other);

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

		bool operator = (const RectD& _other) { m_topLeft = _other.getTopLeft(); m_bottomRight = _other.getBottomRight(); return this; };
		bool operator == (const RectD& _other) const { return m_topLeft == _other.getTopLeft() && m_bottomRight == _other.getBottomRight(); };
		bool operator != (const RectD& _other) const { return m_topLeft != _other.getTopLeft() || m_bottomRight != _other.getBottomRight(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setTop(double _top) { m_topLeft.setY(_top); };
		double getTop(void) const { return m_topLeft.y(); };

		void setLeft(double _left) { m_topLeft.setX(_left); };
		double getLeft(void) const { return m_topLeft.x(); };

		void setTopLeft(const Point2DD& _topLeft) { m_topLeft = _topLeft; };
		const Point2DD& getTopLeft(void) const { return m_topLeft; };

		void setBottom(double _bottom) { m_bottomRight.setY(_bottom); };
		double getBottom(void) const { return m_bottomRight.y(); };

		void setRight(double _right) { m_bottomRight.setX(_right); };
		double getRight(void) const { return m_bottomRight.x(); };

		void setBottomRight(const Point2DD& _bottomRight) { m_bottomRight = m_bottomRight; };
		const Point2DD& getBottomRight(void) const { return m_bottomRight; };

		void setWidth(double _width) { m_bottomRight = Point2DD(m_topLeft.x() + _width, m_topLeft.y()); };
		double getWidth(void) const { return m_bottomRight.x() - m_topLeft.x(); };

		void setHeight(double _height) { m_bottomRight = Point2DD(m_topLeft.x(), m_topLeft.y() + _height); };
		double getHeight(void) const { return m_bottomRight.y() - m_topLeft.y(); };

		void setSize(const Size2DD& _size) { m_bottomRight = Point2DD(m_topLeft.x() + _size.width(), m_topLeft.y() + _size.height()); };
		Size2DD getSize(void) const { return Size2DD(m_bottomRight.x() - m_topLeft.x(), m_bottomRight.y() - m_topLeft.y()); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Basic calculations

		//! @brief Calculates and returns the union of this Rect and the provided Rect.
		RectD unionWith(const RectD& _other) const;

		//! @brief Calclates and returns the intersection of this Rect and the provided Rect.
		RectD intersectsWith(const RectD& _other) const;

		//! @brief Returns true if top left is (0, 0) and bottom right is (0, 0).
		bool isAllZero(void) const { return m_topLeft.x() == 0 && m_topLeft.y() == 0 && m_bottomRight.x() == 0 && m_bottomRight.y() == 0; };

		//! @brief Returns true if the top left and bottom right points are set correctly.
		//! If the left value is bigger than the right value or the bottom value bigger than the top value false is returned.
		bool isValid(void) const { return (m_topLeft.x() <= m_bottomRight.x()) && (m_topLeft.y() <= m_bottomRight.y()); };

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