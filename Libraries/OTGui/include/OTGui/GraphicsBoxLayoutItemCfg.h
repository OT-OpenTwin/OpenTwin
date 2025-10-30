// @otlicense
// File: GraphicsBoxLayoutItemCfg.h
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
#include "OTGui/GraphicsLayoutItemCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsBoxLayoutItemCfg : public GraphicsLayoutItemCfg {
		OT_DECL_NOMOVE(GraphicsBoxLayoutItemCfg)
	public:
		static std::string className() { return "GraphicsBoxLayoutItemCfg"; };

		//! @brief Holds the item and its strech factor in the box
		//! If no item is set then its a stretch only
		typedef std::pair<GraphicsItemCfg*, int> itemStrechPair_t;

		GraphicsBoxLayoutItemCfg(Orientation _orientation = Orientation::Horizontal);
		GraphicsBoxLayoutItemCfg(const GraphicsBoxLayoutItemCfg& _other);
		virtual ~GraphicsBoxLayoutItemCfg();

		GraphicsBoxLayoutItemCfg& operator = (const GraphicsBoxLayoutItemCfg&) = delete;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		virtual GraphicsItemCfg* createCopy() const override { return new GraphicsBoxLayoutItemCfg(*this); };
		virtual std::string getFactoryKey() const override { return GraphicsBoxLayoutItemCfg::className(); };

		virtual void addChildItem(GraphicsItemCfg* _item) override;
		void addChildItem(GraphicsItemCfg* _item, int _stretch);
		void addStrech(int _stretch = 1);
		const std::list<itemStrechPair_t>& getItems() const { return m_items; };
		
		void setOrientation(Orientation _orientation) { m_orientation = _orientation; };
		Orientation getOrientation() const { return m_orientation; };

	private:
		void clearItems();

		Orientation m_orientation;
		std::list<itemStrechPair_t> m_items; //! @brief Items and their stretch in the box (nullptr = stretch only)
	};

}