// @otlicense
// File: CoreAPIExport.h
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

#ifdef OPENTWINCORE_EXPORTS
//! @brief Dll export
#define OT_CORE_API_EXPORT __declspec(dllexport)
#else
//! @brief Dll import
#define OT_CORE_API_EXPORT __declspec(dllimport)
#endif // OPENTWINCORE_EXPORTS

#define OT_CORE_API_EXPORTONLY __declspec(dllexport)
