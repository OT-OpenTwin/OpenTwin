// @otlicense
// File: GraphicsFlowItem.h
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
#include "OTGui/GraphicsFlowItemBuilder.h"
#include "OTGuiAPI/OTGuiAPIAPIExport.h"

namespace ot {

	class OT_GUIAPI_API_EXPORT GraphicsFlowItem {
		OT_DECL_NOCOPY(GraphicsFlowItem)
	public:
		enum FlowConnectorType {
			Input, // Triangle
			InputSecondary, // Kite
			InputOptional, // Circle
			Output, // Triangle
			OutputSecondary, // Kite
			OutputOptional // Circle
		};

		GraphicsFlowItem();
		GraphicsFlowItem(const std::string& _name);
		GraphicsFlowItem(const std::string& _name, const std::string& _title);
		virtual ~GraphicsFlowItem();

		void setName(const std::string& _name) { m_builder.setName(_name); };
		const std::string& name(void) const { return m_builder.getName(); };

		void setTitle(const std::string& _title) { m_builder.setTitle(_title); };
		const std::string& title(void) const { return m_builder.getTitle(); };

		void setToolTip(const std::string& _toolTip) { m_builder.setToolTip(_toolTip); };

		void setTitleColor(const ot::Color& _color) { m_builder.setTitleBackgroundGradientColor(_color); };

		void setBackgroundImagePath(const std::string& _path) { m_builder.setBackgroundImagePath(_path); };

		void setLeftTitleCornerImagePath(const std::string& _path) { m_builder.setLeftTitleCornerImagePath(_path); };

		void setRightTitleCornerImagePath(const std::string& _path) { m_builder.setRightTitleCornerImagePath(_path); };

		void addConnector(const std::string& _name, FlowConnectorType _type, const std::string& _toolTip = std::string());
		void addConnector(const std::string& _name, const std::string& _title, FlowConnectorType _type, const std::string& _toolTip = std::string());

		//! @brief Creates a GraphicsItemCfg in the "OpenTwin flow block" style that takes the current configuration into account.
		//! The caller takes ownership of the item.
		GraphicsItemCfg* createGraphicsItem(void) const { return m_builder.createGraphicsItem(); };

	private:
		void ini(void);

		GraphicsFlowItemBuilder m_builder;
	};

}