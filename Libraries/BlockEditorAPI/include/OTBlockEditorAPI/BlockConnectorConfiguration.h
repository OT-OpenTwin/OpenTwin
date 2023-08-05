//! @file BlockConnectorConfiguration.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTBlockEditorAPI/BlockConfigurationGraphicsObject.h"
#include "OTBlockEditorAPI/BlockEditorAPITypes.h"
#include "OpenTwinCore/Color.h"

#include <string>
#include <list>

#pragma warning (disable:4251)

namespace ot {

	class BLOCKEDITORAPI_API_EXPORT BlockConnectorConfiguration : public ot::BlockConfigurationGraphicsObject {
	public:
		BlockConnectorConfiguration();
		virtual ~BlockConnectorConfiguration();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		// ########################################################################################################################################################

		// Setter/Getter

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string title(void) const { return m_title; };

		void setTitleAlignment(Alignment _align) { m_titleAlignment = _align; };
		Alignment titleAlignment(void) const { return m_titleAlignment; };

		void setStyle(BlockConnectorStyle _style) { m_style = _style; };
		BlockConnectorStyle style(void) const { return m_style; };

		void setFillColor(const ot::Color& _color) { m_fillColor = _color; };
		const ot::Color& fillColor(void) const { return m_fillColor; };

		void setBorderColor(const ot::Color& _color) { m_borderColor = _color; };
		const ot::Color& borderColor(void) const { return m_borderColor; };

	private:
		std::string m_title;
		Alignment m_titleAlignment;
		BlockConnectorStyle m_style;
		ot::Color m_fillColor;
		ot::Color m_borderColor;

		BlockConnectorConfiguration(BlockConnectorConfiguration&) = delete;
		BlockConnectorConfiguration& operator = (BlockConnectorConfiguration&) = delete;
	};

}