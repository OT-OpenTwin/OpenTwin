#pragma once

// OpenTwin header
#include "OpenTwinCore/Serializable.h"
#include "OpenTwinCore/Color.h"
#include "OTBlockEditorAPI/BlockEditorAPIAPIExport.h"
#include "OTBlockEditorAPI/BlockEditorAPITypes.h"

#include <string>
#include <list>

#pragma warning (disable:4251)

namespace ot {

	class BLOCKEDITORAPI_API_EXPORT BlockConnectorConfiguration : public ot::Serializable {
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

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& name(void) const { return m_name; };

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string title(void) const { return m_title; };

		void setTitlePosition(BlockComponentPosition _position) { m_titlePosition = _position; };
		BlockComponentPosition titlePosition(void) const { return m_titlePosition; };

		void addTag(const std::string& _tag);
		void addTags(const std::list<std::string>& _tags);
		void setTags(const std::list<std::string>& _tags);
		const std::list<std::string>& tags(void) const { return m_tags; };

		void setStyle(BlockConnectorStyle _style) { m_style = _style; };
		BlockConnectorStyle style(void) const { return m_style; };

		void setImageSubPath(const std::string& _path) { m_imageSubPath = _path; };
		const std::string& imageSubPath(void) const { return m_imageSubPath; };

		void setFillColor(const ot::Color& _color) { m_fillColor = _color; };
		const ot::Color& fillColor(void) const { return m_fillColor; };

		void setBorderColor(const ot::Color& _color) { m_borderColor = _color; };
		const ot::Color& borderColor(void) const { return m_borderColor; };

	private:
		std::string m_name;
		std::string m_title;
		BlockComponentPosition m_titlePosition;
		std::list<std::string> m_tags;
		BlockConnectorStyle m_style;
		std::string m_imageSubPath;
		ot::Color m_fillColor;
		ot::Color m_borderColor;

		BlockConnectorConfiguration(BlockConnectorConfiguration&) = delete;
		BlockConnectorConfiguration& operator = (BlockConnectorConfiguration&) = delete;
	};

}