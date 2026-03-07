// @otlicense
// File: Margins.h
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
#include "OTGui/GuiTypes.h"

namespace ot {

	// #########################################################################################################################################################################################################

	class OT_GUI_API_EXPORT Margins : public Serializable {
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

		void setLeft(int _margin) { m_left = _margin; };
		int left(void) const { return m_left; };

		void setRight(int _margin) { m_right = _margin; };
		int right(void) const { return m_right; };

		void setTop(int _margin) { m_top = _margin; };
		int top(void) const { return m_top; };

		void setBottom(int _margin) { m_bottom = _margin; };
		int bottom(void) const { return m_bottom; };

	private:
		int m_left;
		int m_top;
		int m_right;
		int m_bottom;
	};

	// #########################################################################################################################################################################################################

	class OT_GUI_API_EXPORT MarginsF : public Serializable {
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

		void setLeft(float _margin) { m_left = _margin; };
		float left(void) const { return m_left; };

		void setRight(float _margin) { m_right = _margin; };
		float right(void) const { return m_right; };

		void setTop(float _margin) { m_top = _margin; };
		float top(void) const { return m_top; };

		void setBottom(float _margin) { m_bottom = _margin; };
		float bottom(void) const { return m_bottom; };

	private:
		float m_left;
		float m_top;
		float m_right;
		float m_bottom;
	};

	// #########################################################################################################################################################################################################

	class OT_GUI_API_EXPORT MarginsD : public Serializable {
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

		void setLeft(double _margin) { m_left = _margin; };
		double left(void) const { return m_left; };

		void setRight(double _margin) { m_right = _margin; };
		double right(void) const { return m_right; };

		void setTop(double _margin) { m_top = _margin; };
		double top(void) const { return m_top; };

		void setBottom(double _margin) { m_bottom = _margin; };
		double bottom(void) const { return m_bottom; };

	private:
		double m_left;
		double m_top;
		double m_right;
		double m_bottom;
	};

}
