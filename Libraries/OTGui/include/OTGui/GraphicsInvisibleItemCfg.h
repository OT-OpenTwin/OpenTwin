// @otlicense
// File: GraphicsInvisibleItemCfg.h
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
#include "OTGui/PenCfg.h"
#include "OTGui/GraphicsItemCfg.h"

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT GraphicsInvisibleItemCfg : public ot::GraphicsItemCfg {
		OT_DECL_NOMOVE(GraphicsInvisibleItemCfg)
	public:
		static std::string className() { return "GraphicsInvisibleItemCfg"; };

		GraphicsInvisibleItemCfg(const ot::Size2DD& _size = ot::Size2DD());
		GraphicsInvisibleItemCfg(const GraphicsInvisibleItemCfg& _other);
		virtual ~GraphicsInvisibleItemCfg();

		GraphicsInvisibleItemCfg& operator = (const GraphicsInvisibleItemCfg&) = delete;

		//! @brief Creates a copy of this item.
		virtual GraphicsItemCfg* createCopy(void) const override { return new GraphicsInvisibleItemCfg(*this); };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey(void) const override { return GraphicsInvisibleItemCfg::className(); };

		void setSize(const ot::Size2DD& _size) { m_size = _size; };
		const ot::Size2DD& getSize(void) const { return m_size; };

	private:
		ot::Size2DD m_size;
	};

}
