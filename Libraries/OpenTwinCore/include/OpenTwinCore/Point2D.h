//! @file Point2D.h
//! @author Alexander Kuester (alexk95)
//! @date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/CoreAPIExport.h"
#include "OpenTwinCore/Serializable.h"

namespace ot {

	template <class T> 
	class __declspec(dllexport) Point2DTemplate : public ot::Serializable {
	public:
		Point2DTemplate() : m_x((T)0), m_y((T)0) {};

		//! @brief Constructor
		//! @param _x The initial X value
		//! @param _y The initial Y value
		Point2DTemplate(T _x, T _y) : m_x(_x), m_y(_y) {};

		//! @brief Copy constructor
		//! @param _other The other 2D point
		Point2DTemplate(const Point2DTemplate<T>& _other) : m_x(_other.m_x), m_y(_other.m_y) {};

		virtual ~Point2DTemplate() {};

		Point2DTemplate<T>& operator = (const Point2DTemplate<T>& _other) { m_x = _other.m_x; m_y = _other.m_y; return *this; };
		bool operator == (const Point2DTemplate<T>& _other) const { return m_x == _other.m_x && m_y == _other.m_y; };
		bool operator != (const Point2DTemplate<T>& _other) const { return m_x != _other.m_x || m_y != _other.m_y; };

		//! @brief Returns the current X value
		T x(void) const { return m_x; };

		//! @brief Returns the current Y value
		T y(void) const { return m_y; };

		//! @brief Set the X value
		//! @param _x The X value to set
		void setX(T _x) { m_x = _x; };

		//! @brief Set the Y value
		//! @param _y The Y value to set
		void setY(T _y) { m_y = _y; };

		//! @brief Set the X and Y value
		//! @param _x The X value to set
		//! @param _y The y value to set
		void set(T _x, T _y) { m_x = _x; m_y = _y; };

	protected:
		T		m_x;			//! The current X value
		T		m_y;			//! The current Y value

	};

	//! @brief 2D Point with integer values
	class OT_CORE_API_EXPORT Point2D : public Point2DTemplate<int> {
	public:
		Point2D() : Point2DTemplate(0, 0) {};
		Point2D(int _x, int _y) : Point2DTemplate(_x, _y) {};
		Point2D(const Point2D& _other) : Point2DTemplate(_other) {};
		virtual ~Point2D() {};

		Point2D& operator = (const Point2D& _other) = default;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;
	};

	//! @brief 2D Point with float values
	class OT_CORE_API_EXPORT Point2DF : public Point2DTemplate<float> {
	public:
		Point2DF() : Point2DTemplate(0, 0) {};
		Point2DF(float _x, float _y) : Point2DTemplate(_x, _y) {};
		Point2DF(const Point2DF& _other) : Point2DTemplate(_other) {};
		virtual ~Point2DF() {};

		Point2DF& operator = (const Point2DF& _other) = default;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;
	};

	//! @brief 2D Point with double values
	class OT_CORE_API_EXPORT Point2DD : public Point2DTemplate<double> {
	public:
		Point2DD() : Point2DTemplate(0, 0) {};
		Point2DD(double _x, double _y) : Point2DTemplate(_x, _y) {};
		Point2DD(const Point2DD& _other) : Point2DTemplate(_other) {};
		virtual ~Point2DD() {};

		Point2DD& operator = (const Point2DD& _other) = default;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;
	};

}