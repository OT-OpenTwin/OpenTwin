// @otlicense
// File: ViewerDebug.h
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

#define OT_VIEWER_MEM_DBG_ENABLED false
#if OT_VIEWER_MEM_DBG_ENABLED==true
#define OT_VIEWER_DBG(___text)                       OT_BASE_DEBUG_LOG("VIEWER", ___text)
#define OT_VIEWER_DBG_PTR(___ptr, ___text)           OT_BASE_DEBUG_PTR("VIEWER", ___ptr, ___text)
#else
#define OT_VIEWER_DBG(___text)
#define OT_VIEWER_DBG_PTR(___ptr, ___text)
#endif

#define OT_VIEWER_VIEWSEL_DBG_ENABLED false
#if OT_VIEWER_VIEWSEL_DBG_ENABLED==true
#define OT_VIEWER_VIEWSEL_DBG(___text)                OT_BASE_DEBUG_LOG("VIEWSEL][VIEWER", ___text)
#define OT_VIEWER_VIEWSEL_DBG_PTR(___ptr, ___text)    OT_BASE_DEBUG_PTR("VIEWSEL][VIEWER", ___ptr, ___text)
#else
#define OT_VIEWER_VIEWSEL_DBG(___text)
#define OT_VIEWER_VIEWSEL_DBG_PTR(___ptr, ___text)
#endif

#define OT_VIEWER_SCENENODE_DBG_ENABLED false
#if OT_VIEWER_SCENENODE_DBG_ENABLED==true
#define OT_VIEWER_SCENENODE_DBG(___text)              OT_BASE_DEBUG_LOG("SCENENODE][VIEWER", ___text)
#define OT_VIEWER_SCENENODE_DBG_PTR(___ptr, ___text)  OT_BASE_DEBUG_PTR("SCENENODE][VIEWER", ___ptr, ___text)
#else
#define OT_VIEWER_SCENENODE_DBG(___text)
#define OT_VIEWER_SCENENODE_DBG_PTR(___ptr, ___text)
#endif

#define OT_VIEWER_VISUALISER_DBG_ENABLED false
#if OT_VIEWER_VISUALISER_DBG_ENABLED==true
#define OT_VIEWER_VISUALISER_DBG(___text)             OT_BASE_DEBUG_LOG("VISUALISER][VIEWER", ___text)
#define OT_VIEWER_VISUALISER_DBG_PTR(___ptr, ___text) OT_BASE_DEBUG_PTR("VISUALISER][VIEWER", ___ptr, ___text)
#else
#define OT_VIEWER_VISUALISER_DBG(___text)
#define OT_VIEWER_VISUALISER_DBG_PTR(___ptr, ___text)
#endif
