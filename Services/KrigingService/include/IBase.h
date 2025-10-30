// @otlicense
// File: IBase.h
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
#ifndef IBase
#include <string>
using namespace std;
class IBase
{
protected:
	std::string scriptName;

public:
	virtual int setDataset(std::list<std::vector<double>> d) = 0;
	virtual int train() = 0;
	virtual int predict(std::list<std::vector<double>> d) = 0;
	virtual void getModel() = 0;

	IBase(const std::string scriptName) {
		this->scriptName.assign(scriptName);
	}
};

#endif
