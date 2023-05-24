//! @file Size2D.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/CoreAPIExport.h"
#include "OpenTwinCore/Serializable.h"

namespace ot {

	template <class T>
	class Size2DTemplate : public ot::Serializable {
	public:
		Size2DTemplate() : m_w((T)0), m_h((T)0) {};
		Size2DTemplate(T _w, T _h) : m_w(_w), m_h(_h) {};
		Size2DTemplate(const Size2DTemplate<T>& _other) : m_w(_other.m_w), m_h(_other.m_h) {};
		virtual ~Size2DTemplate() {};

		Size2DTemplate& operator = (const Size2DTemplate<T>& _other) { m_w = _other.m_w; m_h = _other.m_h; return *this; };
		bool operator == (const Size2DTemplate<T>& _other) const { return m_w == _other.m_w && m_h == _other.m_h; };
		bool operator != (const Size2DTemplate<T>& _other) const { return m_w != _other.m_w || m_h != _other.m_h; };

		//! @brief Set width
		//! @param _w Width to set
		void setWidth(T _w) { m_w = _w; };

		//! @brief Return the width
		T width(void) const { return m_w; };

		//! @brief Set height
		//! @param _h Height to set
		void setHeight(T _h) { m_h = _h; };

		//! @brief Return the height
		T height(void) const { return m_h; };

	protected:
		T m_w;
		T m_h;
	};

	//! @brief 2D Size with integer values
	class OT_CORE_API_EXPORT Size2D : public Size2DTemplate<int> {
	public:
		Size2D() : Size2DTemplate(0, 0) {};
		Size2D(int _w, int _h) : Size2DTemplate(_w, _h) {};
		Size2D(const Size2D& _other) : Size2DTemplate(_other) {};
		virtual ~Size2D() {};

		Size2D& operator = (const Size2D& _other) = default;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;
	};

	//! @brief 2D Size with float values
	class OT_CORE_API_EXPORT Size2DF : public Size2DTemplate<float> {
	public:
		Size2DF() : Size2DTemplate(0, 0) {};
		Size2DF(float _w, float _h) : Size2DTemplate(_w, _h) {};
		Size2DF(const Size2DF& _other) : Size2DTemplate(_other) {};
		virtual ~Size2DF() {};

		Size2DF& operator = (const Size2DF& _other) = default;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;
	};

	//! @brief 2D Size with double values
	class OT_CORE_API_EXPORT Size2DD : public Size2DTemplate<double> {
	public:
		Size2DD() : Size2DTemplate(0, 0) {};
		Size2DD(double _w, double _h) : Size2DTemplate(_w, _h) {};
		Size2DD(const Size2DD& _other) : Size2DTemplate(_other) {};
		virtual ~Size2DD() {};

		Size2DD& operator = (const Size2DD& _other) = default;

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