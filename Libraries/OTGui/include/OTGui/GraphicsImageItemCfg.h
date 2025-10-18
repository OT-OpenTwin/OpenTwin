//! @file GraphicsImageItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTGui/GraphicsItemCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsImageItemCfg : public ot::GraphicsItemCfg {
		OT_DECL_NOMOVE(GraphicsImageItemCfg)
	public:
		static std::string className() { return "GraphicsImageItemCfg"; };

		GraphicsImageItemCfg();
		GraphicsImageItemCfg(const GraphicsImageItemCfg& _other);
		virtual ~GraphicsImageItemCfg();

		GraphicsImageItemCfg& operator = (const GraphicsImageItemCfg&) = delete;

		//! @brief Creates a copy of this item.
		virtual GraphicsItemCfg* createCopy() const override { return new GraphicsImageItemCfg(*this); };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey() const override { return GraphicsImageItemCfg::className(); };

		void setImageData(const std::vector<char>& _data, ImageFileFormat _fileFormat) { m_imageData = _data; m_imageDataFileType = _fileFormat; };
		const std::vector<char>& getImageData() const { return m_imageData; };
		ImageFileFormat getImageFileFormat() const { return m_imageDataFileType; };

		void setImagePath(const std::string& _path) { m_imageSubPath = _path; };
		const std::string& getImagePath() const { return m_imageSubPath; };

		void setMaintainAspectRatio(bool _active) { m_maintainAspectRatio = _active; };
		bool getMaintainAspectRatio() const { return m_maintainAspectRatio; };

	private:
		ImageFileFormat m_imageDataFileType;
		std::vector<char> m_imageData;
		std::string m_imageSubPath;
		bool m_maintainAspectRatio;
	};

}
