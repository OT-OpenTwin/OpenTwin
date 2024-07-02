//! @file GraphicsImageItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTGui/GraphicsItemCfg.h"

#define OT_FactoryKey_GraphicsImageItem "OT_GIImage"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsImageItemCfg : public ot::GraphicsItemCfg {
	public:
		GraphicsImageItemCfg(const std::string& _imageSubPath = std::string());
		virtual ~GraphicsImageItemCfg();

		//! \brief Creates a copy of this item.
		virtual GraphicsItemCfg* createCopy(void) const override;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_GraphicsImageItem); };

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
