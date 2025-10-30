// @otlicense
// File: GraphicsItemFactory.cpp
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

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsItemFileCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsItemFactory.h"
#include "OTWidgets/GraphicsStackItem.h"
#include "OTWidgets/GraphicsItemLoader.h"

ot::GraphicsItemFactory& ot::GraphicsItemFactory::instance(void) {
	static GraphicsItemFactory g_instance;
	return g_instance;
}

ot::GraphicsItem* ot::GraphicsItemFactory::create(const ConstJsonObject& _configObject, bool _isRoot) {
	// Create configuration
	GraphicsItemCfg* cfg = GraphicsItemCfgFactory::create(_configObject);
	if (!cfg) {
		return nullptr;
	}
	else {
		// Create item
		return GraphicsItemFactory::itemFromConfig(cfg, _isRoot);
	}
}

ot::GraphicsItem* ot::GraphicsItemFactory::itemFromConfig(const ot::GraphicsItemCfg* _configuration, bool _isRoot) {
	OTAssertNullptr(_configuration);

	// If the configuration is a file we need to import the actual configuration for this item.
	if (_configuration->getFactoryKey() == ot::GraphicsItemFileCfg::className()) {
		// Get config and file sub path
		const ot::GraphicsItemFileCfg* actualConfig = dynamic_cast<const ot::GraphicsItemFileCfg*>(_configuration);
		OTAssertNullptr(actualConfig);
		std::string configSubPath = actualConfig->getFile();

		// Import new config
		ot::GraphicsItemCfg* newConfiguration = GraphicsItemLoader::instance().createConfiguration(QString::fromStdString(configSubPath));
		if (!_configuration) {
			return nullptr;
		}

		// Copy config data
		*newConfiguration = *actualConfig;
		_configuration = newConfiguration;
	}

	// Create item
	GraphicsItem* itm = GraphicsItemFactory::instance().createFromKey(_configuration->getFactoryKey());
	if (!itm) return nullptr;
	if (!itm->setupFromConfig(_configuration)) {
		OT_LOG_EA("Setup from configuration failed");
		delete itm;
		return nullptr;
	}

	if (_isRoot) itm->finalizeGraphicsItem();

	return itm;
}
