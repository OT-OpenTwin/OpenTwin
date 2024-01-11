#pragma once
#include <string>
#include "OptionSettings.h"

class SParameterHandler
{
public:
	void AnalyseFile(const std::string& fileContent);
	const sp::OptionSettings& getOptionSettings() const { return _optionSettings; }
	const std::string& getComments() const { return _comments; }

private:
	friend class FixtureSParameterHandler;
	std::string _comments = "";
	sp::OptionSettings _optionSettings;
	
	void AnalyseLine(const std::string& content);
	void AnalyseVersionTwoLine(const std::string& content);

	void AnalyseOptionsLine(const std::string& line);
};
