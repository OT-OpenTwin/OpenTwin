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
#include "OTCore/LogDispatcher.h"

#define OT_SELECTION_TEST_LOGS_ENABLED false

#if OT_SELECTION_TEST_LOGS_ENABLED==true
#define OT_SLECTION_TEST_LOG(___text) OT_LOG_T(___text)
#else
#define OT_SLECTION_TEST_LOG(___text)
#endif