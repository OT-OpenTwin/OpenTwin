// @otlicense
// File: InterpreterPathSettings.h
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
#include "OTCore/CoreTypes.h"

class InterpreterPathSettings
{
public:
	InterpreterPathSettings(const std::string& _predefinedEnvironmentName);
	InterpreterPathSettings(ot::UID _manifestUID);
	InterpreterPathSettings() = default;
	~InterpreterPathSettings() = default;
	InterpreterPathSettings(const InterpreterPathSettings& _other) = default;
	InterpreterPathSettings(InterpreterPathSettings&& _other) = default;
	InterpreterPathSettings& operator=(const InterpreterPathSettings& _other) = default;
	InterpreterPathSettings& operator=(InterpreterPathSettings&& _other) = default;

	const std::list<std::string>& getDefaultEnvironments() const
	{
		return m_defaultEnvironments;
	}

	std::string getCustomEnvironmentPath() const
	{
		assert(m_pathsAreSet);
		if (m_customEnvironmentName.empty())
		{
			return "";
		}
		else
		{
			return m_environmentsBase + "\\" + m_customEnvironmentName;
		}
	}
	std::string getHomePath() const
	{
		assert(m_pathsAreSet);
		assert(!m_home.empty());
		return m_home;
	}
	std::string getDllPath() const
	{
		assert(m_pathsAreSet);
		assert(!m_dllPath.empty());
		return m_dllPath;
	}
	std::string getBinPath() const
	{
		assert(m_pathsAreSet);
		assert(!m_binPath.empty());
		return m_binPath;
	}
	std::string getCustomEnvironmentName() const
	{
		assert(m_pathsAreSet);
		return m_customEnvironmentName;
	}
	std::string getEnvironmentsBasePath() const
	{
		assert(m_pathsAreSet);
		assert(!m_environmentsBase.empty());
		return m_environmentsBase;
	}

	std::string getLibPath() const
	{
		assert(m_pathsAreSet);
		assert(!m_libPath.empty());
		return m_libPath;
	}

private:
	std::string m_environmentsBase;
	std::string m_home;
	std::string m_dllPath;
	std::string m_binPath;
	std::string m_customEnvironmentName = ""; //per default core environment only, no custom environment
	std::string m_libPath;
	std::list<std::string> m_defaultEnvironments;

	bool m_pathsAreSet = false;

	void setupBasePaths();
};
