//! @file Margins.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date July 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	// #########################################################################################################################################################################################################

	class OT_GUI_API_EXPORT Margins : ot::Serializable {

		OT_PROPERTY(int, left, setLeft, left)
		OT_PROPERTY(int, top, setTop, top)
		OT_PROPERTY(int, right, setRight, right)
		OT_PROPERTY(int, bottom, setBottom, bottom)

	public:
		Margins() : m_left(0), m_top(0), m_right(0), m_bottom(0) {};
		Margins(int _left, int _top, int _right, int _bottom) : m_left(_left), m_top(_top), m_right(_right), m_bottom(_bottom) {};
		Margins(const Margins& _other) : m_left(_other.m_left), m_top(_other.m_top), m_right(_other.m_right), m_bottom(_other.m_bottom) {};
		virtual ~Margins() {};
		
		Margins& operator = (const Margins& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		
	};

	// #########################################################################################################################################################################################################

	class OT_GUI_API_EXPORT MarginsF : ot::Serializable {

		OT_PROPERTY(float, left, setLeft, left)
		OT_PROPERTY(float, top, setTop, top)
		OT_PROPERTY(float, right, setRight, right)
		OT_PROPERTY(float, bottom, setBottom, bottom)

	public:
		MarginsF() : m_left(0.f), m_top(0.f), m_right(0.f), m_bottom(0.f) {};
		MarginsF(float _left, float _top, float _right, float _bottom) : m_left(_left), m_top(_top), m_right(_right), m_bottom(_bottom) {};
		MarginsF(const MarginsF& _other) : m_left(_other.m_left), m_top(_other.m_top), m_right(_other.m_right), m_bottom(_other.m_bottom) {};
		virtual ~MarginsF() {};

		MarginsF& operator = (const MarginsF& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;
	};

	// #########################################################################################################################################################################################################

	class OT_GUI_API_EXPORT MarginsD : ot::Serializable {

		OT_PROPERTY(double, left, setLeft, left)
		OT_PROPERTY(double, top, setTop, top)
		OT_PROPERTY(double, right, setRight, right)
		OT_PROPERTY(double, bottom, setBottom, bottom)

	public:
		MarginsD() : m_left(0.), m_top(0.), m_right(0.), m_bottom(0.) {};
		MarginsD(double _left, double _top, double _right, double _bottom) : m_left(_left), m_top(_top), m_right(_right), m_bottom(_bottom) {};
		MarginsD(const MarginsD& _other) : m_left(_other.m_left), m_top(_other.m_top), m_right(_other.m_right), m_bottom(_other.m_bottom) {};
		virtual ~MarginsD() {};

		MarginsD& operator = (const MarginsD& _other);

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;
	};

}
