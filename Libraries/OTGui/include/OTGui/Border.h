//! @file Border.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCore/Color.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	class OT_GUI_API_EXPORT Border : public ot::Serializable {
	public:
		Border();
		Border(const ot::Color& _color, int _width);
		Border(const ot::Color& _color, int _leftWidth, int _topWidth, int _rightWidth, int _bottomWidth);
		Border(const Border& _other);
		virtual ~Border();

		Border& operator = (const Border& _other);
		bool operator == (const Border& _other) const;
		bool operator != (const Border& _other) const;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		void setColor(const ot::Color& _color) { m_color = _color; };
		const ot::Color& color(void) const { return m_color; };

		void setWidth(int _width);

		inline void setTop(int _w) { m_top = _w; };
		inline int top(void) const { return m_top; };

		inline void setLeft(int _w) { m_left = _w; };
		inline int left(void) const { return m_left; };

		inline void setRight(int _w) { m_right = _w; };
		inline int right(void) const { return m_right; };

		inline void setBottom(int _w) { m_bottom = _w; };
		inline int bottom(void) const { return m_bottom; };

	private:
		ot::Color m_color;
		int m_top;
		int m_right;
		int m_left;
		int m_bottom;
	};

}