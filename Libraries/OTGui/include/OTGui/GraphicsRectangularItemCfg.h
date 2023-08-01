//! @file GraphicsRectangularItemCfg.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"
#include "OpenTwinCore/Color.h"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsRectangularItemCfg : public ot::GraphicsItemCfg {
	public:
		GraphicsRectangularItemCfg(ot::Painter2D * _backgroundPainter = (ot::Painter2D*)nullptr, const ot::Color& _borderColor = ot::Color(), int _borderWidth = 2, int _cornerRadius = 0);
		virtual ~GraphicsRectangularItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		virtual std::string graphicsItemTypeName(void) const { return "GraphicsRectangularItemCfg"; };

		void setCornerRadius(int _px) { m_cornerRadius = _px; };
		int cornerRadius(void) const { return m_cornerRadius; };

		void setBorderColor(const ot::Color& _color) { m_borderColor = _color; };
		const ot::Color& borderColor(void) const { return m_borderColor; };

		void setBorderWidth(int _px) { m_borderWidth = _px; };
		int borderWidth(void) const { return m_borderWidth; };

		void setBackgroundPainer(ot::Painter2D* _painter);
		ot::Painter2D* backgroundPainter(void) { return m_backgroundPainter; };

	private:
		int m_cornerRadius;
		ot::Color m_borderColor;
		int m_borderWidth;
		ot::Painter2D* m_backgroundPainter;

		GraphicsRectangularItemCfg(GraphicsRectangularItemCfg&) = delete;
		GraphicsRectangularItemCfg& operator = (GraphicsRectangularItemCfg&) = delete;
	};

	OT_RegisterSimpleFactoryObject(GraphicsRectangularItemCfg);
}