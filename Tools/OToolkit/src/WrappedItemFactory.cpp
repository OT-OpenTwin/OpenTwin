// @otlicense
// File: WrappedItemFactory.cpp
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

// OToolkit header
#include "WrappedItemFactory.h"
#include "GraphicsItemDesignerItemBase.h"

WrappedItemFactory& WrappedItemFactory::instance(void) {
	static WrappedItemFactory g_instance;
	return g_instance;
}

GraphicsItemDesignerItemBase* WrappedItemFactory::createFromConfig(const ot::GraphicsItemCfg* _config) {
	OTAssertNullptr(_config);
	GraphicsItemDesignerItemBase* newItem = WrappedItemFactory::instance().createFromKey(_config->getFactoryKey());
	if (!newItem) {
		OT_LOG_E("Factory failed");
		return newItem;
	}

	newItem->setupDesignerItemFromConfig(_config);

	return newItem;
}