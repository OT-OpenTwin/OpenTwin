// @otlicense

//! @file GraphicsPickerItemInfo.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"
#include "OTCore/Serializable.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT GraphicsPickerItemInfo : public Serializable {
		OT_DECL_DEFCOPY(GraphicsPickerItemInfo)
		OT_DECL_DEFMOVE(GraphicsPickerItemInfo)
	public:
		GraphicsPickerItemInfo();
		GraphicsPickerItemInfo(const std::string& _name, const std::string& _title, const std::string& _previewIcon);
		virtual ~GraphicsPickerItemInfo();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& getTitle(void) const { return m_title; };

		void setPreviewIcon(const std::string& _iconPath) { m_previewIcon = _iconPath; };
		const std::string& getPreviewIcon(void) const { return m_previewIcon; };

	private:
		std::string m_name;
		std::string m_title;
		std::string m_previewIcon;
	};

}