//! @file FillPainter2D.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Painter2D.h"
#include "OpenTwinCore/Color.h"

#define OT_FILLPAINTER2D_TYPE "FillPainter2D"

namespace ot {

	class OT_GUI_API_EXPORT FillPainter2D : public ot::Painter2D {
	public:
		FillPainter2D();
		FillPainter2D(const ot::Color& _color);
		FillPainter2D(const FillPainter2D& _other);
		virtual ~FillPainter2D();

		FillPainter2D& operator = (const FillPainter2D& _other);
		bool operator == (const FillPainter2D& _other) const;
		bool operator != (const FillPainter2D& _other) const;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns a string containing the 2D painter type
		virtual std::string painterType(void) const { return OT_FILLPAINTER2D_TYPE; };

		void setColor(ot::Color& _color) { m_color = _color; };
		const ot::Color& color(void) const { return m_color; };
		
	private:
		ot::Color m_color;
	};
}