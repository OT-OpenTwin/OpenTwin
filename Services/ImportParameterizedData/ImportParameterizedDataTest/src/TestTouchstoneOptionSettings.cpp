// @otlicense
// File: TestTouchstoneOptionSettings.cpp
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

//#include <gtest/gtest.h>
//#include "OptionSettings.h"
//
//TEST(TouchstoneOptionSettings, DefaultValues)
//{
//	ts::OptionSettings settings;
//	EXPECT_EQ(settings.getFormat(), ts::option::Format::magnitude_angle);
//	EXPECT_EQ(settings.getFrequency(), ts::option::Frequency::GHz);
//	EXPECT_EQ(settings.getParameter(), ts::option::Parameter::Scattering);
//	EXPECT_EQ(settings.getReferenceResistance(), ot::Variable(50));
//}
//
//
//TEST(TouchstoneOptionSettings, Equal)
//{
//	ts::OptionSettings settings1(ts::option::Frequency::Hz,ts::option::Format::real_imaginary,ts::option::Parameter::Hybrid_h,ot::Variable(70));
//	ts::OptionSettings settings2(ts::option::Frequency::Hz,ts::option::Format::real_imaginary,ts::option::Parameter::Hybrid_h,ot::Variable(70));
//	EXPECT_EQ(settings1, settings2);
//}
//
//TEST(TouchstoneOptionSettings, NotEqual)
//{
//	ts::OptionSettings settings1(ts::option::Frequency::Hz, ts::option::Format::real_imaginary, ts::option::Parameter::Hybrid_h, ot::Variable(70));
//	ts::OptionSettings settings2;
//	EXPECT_FALSE(settings1 == settings2);
//}