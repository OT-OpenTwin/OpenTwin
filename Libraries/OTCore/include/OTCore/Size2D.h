// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"

namespace ot {

	//! @brief 2D Size with integer values
	class OT_CORE_API_EXPORTONLY Size2D : public ot::Serializable {
	public:
		Size2D() : m_w(0), m_h(0) {};
		Size2D(int _w, int _h) : m_w(_w), m_h(_h) {};
		Size2D(const Size2D& _other) : m_w(_other.m_w), m_h(_other.m_h) {};
		virtual ~Size2D() {};

		Size2D& operator = (const Size2D& _other) { m_w = _other.m_w; m_h = _other.m_h; return *this; };
		bool operator == (const Size2D& _other) const { return m_w == _other.m_w && m_h == _other.m_h; };
		bool operator != (const Size2D& _other) const { return m_w != _other.m_w || m_h != _other.m_h; };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns true if the width and height are greater or equal 0
		bool isValid(void) const { return m_w >= 0 && m_h >= 0; };

		//! @brief Set width
		//! @param _w Width to set
		void setWidth(int _w) { m_w = _w; };

		//! @brief Return the width
		int width(void) const { return m_w; };

		//! @brief Set height
		//! @param _h Height to set
		void setHeight(int _h) { m_h = _h; };

		//! @brief Return the height
		int height(void) const { return m_h; };

	private:
		int m_w;
		int m_h;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @brief 2D Size with float values
	class OT_CORE_API_EXPORTONLY Size2DF : public ot::Serializable {
	public:
		Size2DF() : m_w(0.f), m_h(0.f) {};
		Size2DF(float _w, float _h) : m_w(_w), m_h(_h) {};
		Size2DF(const Size2DF& _other) : m_w(_other.m_w), m_h(_other.m_h) {};
		virtual ~Size2DF() {};

		Size2DF& operator = (const Size2DF& _other) { m_w = _other.m_w; m_h = _other.m_h; return *this; };
		bool operator == (const Size2DF& _other) const { return m_w == _other.m_w && m_h == _other.m_h; };
		bool operator != (const Size2DF& _other) const { return m_w != _other.m_w || m_h != _other.m_h; };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns true if the width and height are greater or equal 0
		bool isValid(void) const { return m_w >= 0.f && m_h >= 0.f; };

		//! @brief Set width
		//! @param _w Width to set
		void setWidth(float _w) { m_w = _w; };

		//! @brief Return the width
		float width(void) const { return m_w; };

		//! @brief Set height
		//! @param _h Height to set
		void setHeight(float _h) { m_h = _h; };

		//! @brief Return the height
		float height(void) const { return m_h; };

	private:
		float m_w;
		float m_h;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @brief 2D Size with double values
	class OT_CORE_API_EXPORTONLY Size2DD : public ot::Serializable {
	public:
		Size2DD() : m_w(0.), m_h(0.) {};
		Size2DD(double _w, double _h) : m_w(_w), m_h(_h) {};
		Size2DD(const Size2DD& _other) : m_w(_other.m_w), m_h(_other.m_h) {};
		virtual ~Size2DD() {};

		Size2DD& operator = (const Size2DD& _other) { m_w = _other.m_w; m_h = _other.m_h; return *this; };
		bool operator == (const Size2DD& _other) const { return m_w == _other.m_w && m_h == _other.m_h; };
		bool operator != (const Size2DD& _other) const { return m_w != _other.m_w || m_h != _other.m_h; };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns true if the width and height are greater or equal 0
		bool isValid(void) const { return m_w >= 0. && m_h >= 0.; };

		//! @brief Set width
		//! @param _w Width to set
		void setWidth(double _w) { m_w = _w; };

		//! @brief Return the width
		double width(void) const { return m_w; };

		//! @brief Set height
		//! @param _h Height to set
		void setHeight(double _h) { m_h = _h; };

		//! @brief Return the height
		double height(void) const { return m_h; };

	private:
		double m_w;
		double m_h;
	};

}