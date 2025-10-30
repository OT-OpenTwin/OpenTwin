// @otlicense
// File: UniqueEntityNameCreator.h
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
#include <list>
#include <string>

class UniqueEntityNameCreator
{
public:
	UniqueEntityNameCreator() = default;

	std::string create(const std::string& _entityPath);
	
	void setNameBase(const std::string& _entityShortNameBase)
	{
		m_entityShortNameBase = _entityShortNameBase;
	}

	const std::string& getNameBase() 
	{
		return m_entityShortNameBase;
	}

	void setAlreadyTakenNames(const std::list<std::string>& _alreadyTakenNames);

	void clearAlreadyTakenNames() 
	{ 
		m_alreadyTakenNames.clear(); 
	}

private:
	std::list<std::string> m_alreadyTakenNames;
	std::string m_entityShortNameBase;
};