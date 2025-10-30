// @otlicense
// File: OptionsParameterHandlerFormat.cpp
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

#include "OptionsParameterHandlerFormat.h"


bool OptionsParameterHandlerFormat::IndividualInterpretation(const std::string& entry, ts::OptionSettings& options)
{
	auto searchResult = _stringToFormat.find(entry);
	if (searchResult != _stringToFormat.end())
	{
		options.setFormat(searchResult->second);
		return true;
	}
	else
	{
		return false;
	}
}
