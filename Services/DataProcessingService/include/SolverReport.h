// @otlicense
// File: SolverReport.h
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
#include "OTServiceFoundation/UiComponent.h"

class SolverReport
{
public:
	~SolverReport();
	static SolverReport& instance()
	{
		static thread_local SolverReport g_solverReport;
		return g_solverReport;
	}
	void setSolverName(const std::string& _solverName)
	{
		m_solverName = _solverName;
	}
	void addToContent(const std::string& _content)
	{
		m_reportContent += _content;
	}
	void addToContentAndDisplay(const std::string& _content, ot::components::UiComponent* _uiComponent)
	{
		m_reportContent += _content;
		if(_uiComponent != nullptr)
		{
			_uiComponent->displayMessage(_content);
		}
	}
	void clear()
	{
		m_solverName = "";
		m_reportContent = "";
	}

	bool storeReport();
	std::string getSolverName() const
	{
		return m_solverName;
	}
private:
	std::string m_solverName = "";
	std::string m_reportContent = "";
	
	SolverReport()=default;
};
