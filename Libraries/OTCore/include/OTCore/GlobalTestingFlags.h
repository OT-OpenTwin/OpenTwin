// @otlicense
// File: GlobalTestingFlags.h
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

#ifndef OT_TESTING_GLOBAL_AllTestsEnabled

//! @def OT_TESTING_GLOBAL_DefaultTestEnabledState
//! @brief Default enabled state (true/false) used for the different tests.
#define OT_TESTING_GLOBAL_AllTestsEnabled false

#endif // !OT_TESTING_GLOBAL_AllTestsEnabled

#if OT_TESTING_GLOBAL_AllTestsEnabled==true

//! @def OT_TESTING_GLOBAL_RuntimeTestingEnabled
//! If enabled any runtime tests are enabled.
//! @warning When deploying check that the value is false or true is clearly wanted.
#define OT_TESTING_GLOBAL_RuntimeTestingEnabled OT_TESTING_GLOBAL_AllTestsEnabled

#else // OT_TESTING_GLOBAL_AllTestsEnabled==true

//! @def OT_TESTING_GLOBAL_RuntimeTestingEnabled
//! If enabled any runtime tests are enabled.
//! @warning When deploying check that the value is false or true is clearly wanted.
#define OT_TESTING_GLOBAL_RuntimeTestingEnabled false

#endif // OT_TESTING_GLOBAL_AllTestsEnabled==true