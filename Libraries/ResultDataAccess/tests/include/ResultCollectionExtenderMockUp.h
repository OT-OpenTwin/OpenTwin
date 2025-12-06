// @otlicense
// File: ResultCollectionExtenderMockUp.h
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
#include "ResultCollectionExtender.h"
#include <list>
class ResultCollectionExtenderMockUp : public ResultCollectionExtender
{
	friend class FixtureResultCollectionExtender;
	ResultCollectionExtenderMockUp()
	{
		m_indices  = { 31,32,33,34, 35, 36,37,38,39,40 };
	}
private:
	std::list<uint64_t> m_indices;
	uint64_t getLastIndex()
	{
		uint64_t index = m_indices.back();
		m_indices.pop_back();
		return index;
	}
	const uint64_t findNextFreeSeriesIndex() override { return getLastIndex();	};
	const uint64_t findNextFreeQuantityIndex() override { return getLastIndex(); };
	const uint64_t findNextFreeParameterIndex() override { return getLastIndex(); };
};
