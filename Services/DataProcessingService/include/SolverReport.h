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
