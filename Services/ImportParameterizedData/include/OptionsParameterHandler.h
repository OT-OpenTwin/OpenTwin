// @otlicense
// File: OptionsParameterHandler.h
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
#include <string>
#include <exception>

#include "Options.h"
#include "OptionSettings.h"
class OptionsParameterHandler
{
public:
	virtual OptionsParameterHandler* SetNextHandler(OptionsParameterHandler* nextHandler)
	{
		_nextHandler = nextHandler;
		return this;
	}

	void InterpreteOptionsParameter(const std::string& entry, ts::OptionSettings& options)
	{
		const bool suitableHandlerFound = IndividualInterpretation(entry, options);
		if (!suitableHandlerFound)
		{
			if (_nextHandler == nullptr)
			{
				throw std::exception(std::string("Entry \"" + entry + " \" is not interpretable as content of a .snp options line.").c_str());
			}
			else
			{
				_nextHandler->InterpreteOptionsParameter(entry, options);
			}
		}
	}
protected:
	/// <summary>
	/// Requires a string in lower cases.
	/// </summary>
	/// <param name="entry"></param>
	/// <param name="options"></param>
	/// <returns></returns>
	virtual bool IndividualInterpretation(const std::string& entry, ts::OptionSettings& options) = 0;

private:

	OptionsParameterHandler* _nextHandler = nullptr;
};
