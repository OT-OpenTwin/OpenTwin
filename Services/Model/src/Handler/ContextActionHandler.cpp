// @otlicense
// File: ContextActionHandler.cpp
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

#include <stdafx.h>

// Service header
#include "Model.h"
#include "Application.h"
#include "Handler/ContextActionHandler.h"

// OpenTwin header
#include "OTGui/Menu/MenuCfg.h"
#include "OTGui/Event/MenuRequestEvent.h"
#include "OTGui/Event/GraphicsMenuRequestData.h"
#include "OTGui/Event/NavigationMenuRequestData.h"
#include "OTGui/Event/PlotMenuRequestData.h"
#include "OTGui/Event/TableMenuRequestData.h"
#include "OTGui/Event/TextEditorMenuRequestData.h"
#include "OTGui/Event/View3DMenuRequestData.h"

using namespace ot;

MenuCfg ContextActionHandler::menuRequested(const MenuRequestEvent& _event)
{
	const MenuRequestData* data = _event.getRequestData();
	if (!data) {
		return MenuCfg();
	}
	
	MenuCfg menu;

	// Get model
	Model* model = Application::instance()->getModel();
	if (!model)
	{
		OT_LOG_E("Model is not available.");
		return menu;
	}

	// Get entity
	EntityBase* entity = model->getEntityByID(data->getEntityID());
	if (!entity)
	{
		OT_LOG_E("Entity not found { \"ID\": " + std::to_string(data->getEntityID()) + " }");
		return menu;
	}

	// Fill menu
	entity->fillContextMenu(data, menu);

	return menu;
}
