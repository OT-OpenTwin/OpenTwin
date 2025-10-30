// @otlicense
// File: GraphicsItemFactory.h
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
#include "OTCore/FactoryTemplate.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

namespace ot {

	class GraphicsItemCfg;

	//! @class GraphicsItemFactory.
	//! @brief The GraphicsItemFactory is used to create graphics items from their factory name or from a GraphicsItemCfg.
	//! @note Note that items that register at this factory must register with the same key as their configuration equivalents.
	class OT_WIDGETS_API_EXPORT GraphicsItemFactory : public FactoryTemplate<GraphicsItem> {
		OT_DECL_NOCOPY(GraphicsItemFactory)
		OT_DECL_NOMOVE(GraphicsItemFactory)
	public:
		static GraphicsItemFactory& instance(void);

		//! @brief Will create the configuration from the provided json object and then create the item using the created configuration.
		//! @param _configObject The JSON object containing the GraphicsItemCfg.
		static ot::GraphicsItem* create(const ConstJsonObject& _configObject, bool _isRoot = false);

		//! @brief Creates a graphics item from the provided configuration.
		//! The graphics item will be created according to the graphics item cfg name.
		//! If the created item is the root item finalizeGraphicsItem() will be called.
		//! @param _config The graphics item configuration.
		//! @param _isRoot If true, the item will be added to a stack with a highlight item on top. The highlight item will be set to the root item.
		static ot::GraphicsItem* itemFromConfig(const ot::GraphicsItemCfg* _config, bool _isRoot = false);

	private:
		GraphicsItemFactory() {};
		~GraphicsItemFactory() {};
	};

	//! @class GraphicsItemFactoryRegistrar.
	//! @brief The GraphicsItemFactoryRegistrar is used to register a GraphicsItem constructor at the GraphicsItemFactory.
	//! @note Note that the key used to register must be equal to the configraion equivalent (see GraphicsItemCfg).
	template<class T> class GraphicsItemFactoryRegistrar : public FactoryRegistrarTemplate<GraphicsItemFactory, T> {
		OT_DECL_NOCOPY(GraphicsItemFactoryRegistrar)
		OT_DECL_NOMOVE(GraphicsItemFactoryRegistrar)
	public:
		GraphicsItemFactoryRegistrar(const std::string& _key) : FactoryRegistrarTemplate<GraphicsItemFactory, T>(_key) {};
	};

}
