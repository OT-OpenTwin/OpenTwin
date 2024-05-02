//! @file Point2D.h
//! @author Alexander Kuester (alexk95)
//! @date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreAPIExport.h"
#include "OTCore/Serializable.h"

namespace ot {

	//! @class Point2D
	//! @brief 2D Point with integer values
	class OT_CORE_API_EXPORT Point2D : public ot::Serializable {
	public:
		Point2D() : m_x(0), m_y(0) {};
		Point2D(int _x, int _y) : m_x(_x), m_y(_y) {};
		Point2D(const Point2D& _other) : m_x(_other.m_x), m_y(_other.m_y) {};
		virtual ~Point2D() {};

		Point2D& operator = (const Point2D& _other) { m_x = _other.m_x; m_y = _other.m_y; return *this; };
		bool operator == (const Point2D& _other) const { return m_x == _other.m_x && m_y == _other.m_y; };
		bool operator != (const Point2D& _other) const { return m_x != _other.m_x || m_y != _other.m_y; };
		
		Point2D operator + (const Point2D& _other) const { return Point2D(m_x + _other.m_x, m_y + _other.m_y); };
		Point2D& operator += (const Point2D& _other) { m_x += _other.m_x; m_y += _other.m_y; return *this; };
		Point2D operator - (const Point2D& _other) const { return Point2D(m_x - _other.m_x, m_y -_other.m_y); };
		Point2D& operator -= (const Point2D& _other) { m_x -= _other.m_x; m_y -= _other.m_y; return *this; };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the current X value
		int x(void) const { return m_x; };

		//! @brief Returns the current Y value
		int y(void) const { return m_y; };

		//! @brief Set the X value
		//! @param _x The X value to set
		void setX(int _x) { m_x = _x; };

		//! @brief Set the Y value
		//! @param _y The Y value to set
		void setY(int _y) { m_y = _y; };

		//! @brief Set the X and Y value
		//! @param _x The X value to set
		//! @param _y The y value to set
		void set(int _x, int _y) { m_x = _x; m_y = _y; };

	private:
		int	m_x; //! The current X value
		int m_y; //! The current Y value
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @class Point2DF
	//! @brief 2D Point with float values
	class OT_CORE_API_EXPORT Point2DF : public ot::Serializable {
	public:
		Point2DF() : m_x(0.f), m_y(0.f) {};
		Point2DF(float _x, float _y) : m_x(_x), m_y(_y) {};
		Point2DF(const Point2DF& _other) : m_x(_other.m_x), m_y(_other.m_y) {};
		virtual ~Point2DF() {};

		Point2DF& operator = (const Point2DF& _other) { m_x = _other.m_x; m_y = _other.m_y; return *this; };
		bool operator == (const Point2DF& _other) const { return m_x == _other.m_x && m_y == _other.m_y; };
		bool operator != (const Point2DF& _other) const { return m_x != _other.m_x || m_y != _other.m_y; };

		Point2DF operator + (const Point2DF& _other) const { return Point2DF(m_x + _other.m_x, m_y + _other.m_y); };
		Point2DF& operator += (const Point2DF& _other) { m_x += _other.m_x; m_y += _other.m_y; return *this; };
		Point2DF operator - (const Point2DF& _other) const { return Point2DF(m_x - _other.m_x, m_y - _other.m_y); };
		Point2DF& operator -= (const Point2DF& _other) { m_x -= _other.m_x; m_y -= _other.m_y; return *this; };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the current X value
		float x(void) const { return m_x; };

		//! @brief Returns the current Y value
		float y(void) const { return m_y; };

		//! @brief Set the X value
		//! @param _x The X value to set
		void setX(float _x) { m_x = _x; };

		//! @brief Set the Y value
		//! @param _y The Y value to set
		void setY(float _y) { m_y = _y; };

		//! @brief Set the X and Y value
		//! @param _x The X value to set
		//! @param _y The y value to set
		void set(float _x, float _y) { m_x = _x; m_y = _y; };

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
	public:
		Point2DD() : m_x(0.), m_y(0.) {};
		Point2DD(double _x, double _y) : m_x(_x), m_y(_y) {};
		Point2DD(const Point2DD& _other) : m_x(_other.m_x), m_y(_other.m_y) {};
		virtual ~Point2DD() {};

		Point2DD& operator = (const Point2DD& _other) { m_x = _other.m_x; m_y = _other.m_y; return *this; };
		bool operator == (const Point2DD& _other) const { return m_x == _other.m_x && m_y == _other.m_y; };
		bool operator != (const Point2DD& _other) const { return m_x != _other.m_x || m_y != _other.m_y; };

		Point2DD operator + (const Point2DD& _other) const { return Point2DD(m_x + _other.m_x, m_y + _other.m_y); };
		Point2DD& operator += (const Point2DD& _other) { m_x += _other.m_x; m_y += _other.m_y; return *this; };
		Point2DD operator - (const Point2DD& _other) const { return Point2DD(m_x - _other.m_x, m_y - _other.m_y); };
		Point2DD& operator -= (const Point2DD& _other) { m_x -= _other.m_x; m_y -= _other.m_y; return *this; };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the current X value
		double x(void) const { return m_x; };

		//! @brief Returns the current Y value
		double y(void) const { return m_y; };

		//! @brief Set the X value
		//! @param _x The X value to set
		void setX(double _x) { m_x = _x; };

		//! @brief Set the Y value
		//! @param _y The Y value to set
		void setY(double _y) { m_y = _y; };

		//! @brief Set the X and Y value
		//! @param _x The X value to set
		//! @param _y The y value to set
		void set(double _x, double _y) { m_x = _x; m_y = _y; };

	private:
		double m_x; //! The current X value
		double m_y; //! The current Y value
	};

}