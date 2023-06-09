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
		Border(const Border& _other);
		virtual ~Border();

		Border& operator = (const Border& _other);
		bool operator == (const Border& _other) const;
		bool operator != (const Border& _other) const;

		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const override;
		virtual void setFromJsonObject(OT_rJSON_val& _jsonObject) override;

		void setColor(const ot::Color& _color) { m_color = _color; };
		const ot::Color& color(void) const { return m_color; };

		void setWidth(int _width) { m_width = _width; };
		int width(void) const { return m_width; };

	private:
		ot::Color m_color;
		int m_width;
	};

}