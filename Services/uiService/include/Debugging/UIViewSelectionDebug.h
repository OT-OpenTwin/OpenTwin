// @otlicense
// File: UITestLogs.h
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
#include "OTViewer/Intern/ViewerDebug.h"

#if OT_VIEWER_VIEWSEL_DBG_ENABLED==true
#define OT_UI_VIEWSEL_DBG(___text)             OT_BASE_DEBUG_LOG("VIEWSEL][UI", ___text)
#define OT_UI_VIEWSEL_DBG_PTR(___ptr, ___text) OT_BASE_DEBUG_PTR("VIEWSEL][UI", ___ptr, ___text)
#else
#define OT_UI_VIEWSEL_DBG(___text)
#define OT_UI_VIEWSEL_DBG_PTR(___ptr, ___text)
#endif