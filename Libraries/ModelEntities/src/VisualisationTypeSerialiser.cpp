// @otlicense
// File: VisualisationTypeSerialiser.cpp
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

#include "VisualisationTypeSerialiser.h"
#include "IVisualisationTable.h"
#include "IVisualisationText.h"
#include "IVisualisationImage.h"
#include "OTGui/VisualisationTypes.h"

void VisualisationTypeSerialiser::operator()(EntityBase* _baseEntity, ot::JsonValue& _object, ot::JsonAllocator& _allocator)
{
	ot::VisualisationTypes visTypes;
	
	if (dynamic_cast<IVisualisationTable*>(_baseEntity)) {
		visTypes.addTableVisualisation();
	}
	if (dynamic_cast<IVisualisationText*>(_baseEntity)) {
		visTypes.addTextVisualisation();
	}
	if (dynamic_cast<IVisualisationImage*>(_baseEntity)) {
		visTypes.addImageVisualisation();
	}

	if (visTypes.hasVisualisations()) {
		visTypes.addToJsonObject(_object, _allocator);
	}
}
