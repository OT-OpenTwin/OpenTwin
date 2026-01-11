// @otlicense
// File: GraphicsDecoratedLineItemCfg.h
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
#include "OTGui/Graphics/GraphicsLineItemCfg.h"
#include "OTGui/Graphics/GraphicsDecorationCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsDecoratedLineItemCfg : public GraphicsLineItemCfg {
		OT_DECL_NOMOVE(GraphicsDecoratedLineItemCfg)
	public:
		static std::string className() { return "GraphicsDecoratedLineItemCfg"; };

		GraphicsDecoratedLineItemCfg();
		GraphicsDecoratedLineItemCfg(const GraphicsDecoratedLineItemCfg& _other);
		virtual ~GraphicsDecoratedLineItemCfg();

		GraphicsDecoratedLineItemCfg& operator = (const GraphicsDecoratedLineItemCfg&) = delete;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		//! @brief Creates a copy of this item.
		virtual GraphicsItemCfg* createCopy(void) const override { return new GraphicsDecoratedLineItemCfg(*this); };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey() const override { return GraphicsDecoratedLineItemCfg::className(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setFromDecoration(const GraphicsDecorationCfg& _decoration) { m_fromDecoration = _decoration; };
		void setFromDecoration(GraphicsDecorationCfg&& _decoration) { m_fromDecoration = std::move(_decoration); };
		const GraphicsDecorationCfg& getFromDecoration() const { return m_fromDecoration; };

		void setToDecoration(const GraphicsDecorationCfg& _decoration) { m_toDecoration = _decoration; };
		void setToDecoration(GraphicsDecorationCfg&& _decoration) { m_toDecoration = std::move(_decoration); };
		const GraphicsDecorationCfg& getToDecoration() const { return m_toDecoration; };

	private:
		GraphicsDecorationCfg m_fromDecoration;
		GraphicsDecorationCfg m_toDecoration;
	};

}