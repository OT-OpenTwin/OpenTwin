// @otlicense
// File: GraphicsImageItemCfg.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTCore/Color.h"
#include "OTGui/PenCfg.h"
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

		void setBorderPen(const PenFCfg& _pen) { m_borderPen = _pen; };
		const PenFCfg& getBorderPen() const { return m_borderPen; };

	private:
		ImageFileFormat m_imageDataFileType;
		std::vector<char> m_imageData;
		std::string m_imageSubPath;
		bool m_maintainAspectRatio;
		PenFCfg m_borderPen;
	};

}
