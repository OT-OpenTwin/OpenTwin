//! @file GraphicsImageItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"
#include "OTCore/Color.h"

#define OT_SimpleFactoryJsonKeyValue_GraphicsImageItemCfg "OT_GICImage"

namespace ot {

	class OT_GUI_API_EXPORTONLY GraphicsImageItemCfg : public ot::GraphicsItemCfg {
	public:
		GraphicsImageItemCfg(const std::string& _imageSubPath = std::string());
		virtual ~GraphicsImageItemCfg();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string simpleFactoryObjectKey(void) const override { return std::string(OT_SimpleFactoryJsonKeyValue_GraphicsImageItemCfg); };

		void setImagePath(const std::string& _path) { m_imageSubPath = _path; };
		const std::string& imagePath(void) const { return m_imageSubPath; };

		void setMaintainAspectRatio(bool _active) { m_maintainAspectRatio = _active; };
		bool isMaintainAspectRatio(void) const { return m_maintainAspectRatio; };

		void setColorMask(const ot::Color& _color) { m_colorMask = _color; };
		const ot::Color& colorMask(void) const { return m_colorMask; };

	private:
		std::string m_imageSubPath;
		bool m_maintainAspectRatio;
		ot::Color m_colorMask;

		GraphicsImageItemCfg(GraphicsImageItemCfg&) = delete;
		GraphicsImageItemCfg& operator = (GraphicsImageItemCfg&) = delete;
	};

}
