// @otlicense
// File: Painter2DFactory.cpp
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
#include "OTGui/Painter2DFactory.h"

ot::Painter2DFactory& ot::Painter2DFactory::instance(void) {
	static Painter2DFactory g_instance;
	return g_instance;
}

ot::Painter2D* ot::Painter2DFactory::create(const ConstJsonObject& _jsonObject) {
	return Painter2DFactory::instance().createFromJSON(_jsonObject, OT_JSON_MEMBER_Painter2DType);
}