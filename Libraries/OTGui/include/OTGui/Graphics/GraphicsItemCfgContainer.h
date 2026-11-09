// @otlicense
// File: GraphicsItemCfgContainer.h
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
#include "OTGui/Graphics/GraphicsItemCfg.h"

namespace ot
{

	class OT_GUI_API_EXPORT GraphicsItemCfgContainer : public Serializable
	{
	public:
		GraphicsItemCfgContainer(GraphicsItemCfg* _item = (GraphicsItemCfg*)nullptr);
		GraphicsItemCfgContainer(const GraphicsItemCfgContainer& _other);
		GraphicsItemCfgContainer(GraphicsItemCfgContainer&& _other) noexcept;
		~GraphicsItemCfgContainer();

		GraphicsItemCfgContainer& operator = (const GraphicsItemCfgContainer& _other);
		GraphicsItemCfgContainer& operator = (GraphicsItemCfgContainer&& _other) noexcept;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		bool isEmpty() const { return m_item == nullptr; };
		void replace(GraphicsItemCfg* _item);
		GraphicsItemCfg* get() { return m_item; };
		const GraphicsItemCfg* get() const { return m_item; };
		GraphicsItemCfg* take() { GraphicsItemCfg* tmp = m_item; m_item = nullptr; return tmp; };

	private:
		GraphicsItemCfg* m_item;
	};

}