//! @file Border.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/Serializable.h"
#include "OpenTwinCore/Color.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	class OT_GUI_API_EXPORT Border : public ot::Serializable {
	public:
		Border();
		Border(const ot::Color& _color, int _width);
		Border(const ot::Color& _color, int _topWidth, int _leftWidth, int _rightWidth, int _bottomWidth);
		Border(const Border& _other);
		virtual ~Border();

		Border& operator = (const Border& _other);
		bool operator == (const Border& _other) const;
		bool operator != (const Border& _other) const;

		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const override;
		virtual void setFromJsonObject(OT_rJSON_val& _jsonObject) override;

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