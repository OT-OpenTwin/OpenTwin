//! @file RectangleBlockCategoryConfiguration.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/BlockLayerConfiguration.h"
#include "OpenTwinCore/Color.h"

#define OT_RECTANGLEBLOCKLAYERCONFIGURATION_TYPE "RectangleBlockLayer"

namespace ot {

	class Painter2D;

	class BLOCKEDITORAPI_API_EXPORT RectangleBlockLayerConfiguration : public ot::BlockLayerConfiguration {
	public:
		RectangleBlockLayerConfiguration(ot::Painter2D * _backgroundPainter = (ot::Painter2D*)nullptr, const ot::Color& _borderColor = ot::Color(), int _borderWidth = 2, int _cornerRadius = 0);
		virtual ~RectangleBlockLayerConfiguration();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the layer configuration type
		virtual std::string layerType(void) const { return OT_RECTANGLEBLOCKLAYERCONFIGURATION_TYPE; };

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

		RectangleBlockLayerConfiguration(RectangleBlockLayerConfiguration&) = delete;
		RectangleBlockLayerConfiguration& operator = (RectangleBlockLayerConfiguration&) = delete;
	};
}