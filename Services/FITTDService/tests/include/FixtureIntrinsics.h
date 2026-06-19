// @otlicense
// File: FixtureIntrinsics.h
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
#include "gtest/gtest.h"

class FixtureIntrinsics : public testing::Test
{
	float *_a = nullptr;
	float *_b = nullptr;
	const int _vectorSize = 16;
	const int _alignment = 512;

public:
	FixtureIntrinsics();
	~FixtureIntrinsics();
	float * GetVectorB() const { return _b; };
	float * GetVectorA() const { return _a; };
	const int GetAlignment()const { return _alignment; }
	const int GetVectorSize()const { return _vectorSize; };
};
