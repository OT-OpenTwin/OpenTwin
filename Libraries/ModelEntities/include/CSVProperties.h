#pragma once
#include <string>
#include <string>

struct CSVProperties
{
	std::string m_rowDelimiter;
	std::string m_columnDelimiter;
	std::string m_decimalDelimiter;
	bool m_evaluateEscapeCharacters;
};
