// @otlicense
// File: CoreDebug.h
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
#include "OTCore/Debugging/DebugLogBase.h"

#define OT_CORE_OTOBJECT_DBG_ENABLED false
#if OT_CORE_OTOBJECT_DBG_ENABLED==true
#define OT_CORE_OTOBJECT_DBG(___text)              OT_BASE_DEBUG_LOG("OTOBJECT", ___text)
#define OT_CORE_OTOBJECT_DBG_PTR(___ptr, ___text)  OT_BASE_DEBUG_PTR("OTOBJECT", ___ptr, ___text)
#else
#define OT_CORE_OTOBJECT_DBG(___text)
#define OT_CORE_OTOBJECT_DBG_PTR(___ptr, ___text)
#endif
