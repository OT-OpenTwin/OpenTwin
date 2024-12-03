#pragma once
#include <list>
#include <string>

struct BatchUpdateInformation
{
	std::list<std::string> m_pythonScriptNames;
	std::list<std::string> m_selectionEntityNames;
};
