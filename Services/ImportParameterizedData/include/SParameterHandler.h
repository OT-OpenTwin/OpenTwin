#pragma once
#include <string>
#include <list>
#include "OptionSettings.h"
#include "PortData.h"

class SParameterHandler
{
public:
	void AnalyseFile(const std::string& fileContent);
	const sp::OptionSettings& getOptionSettings() const { return _optionSettings; }
	const std::string& getComments() const { return _comments; }
	const std::list<sp::PortData>& getPortData() const { return _portData; }

private:
	friend class FixtureSParameterHandler;
	std::string _comments = "";
	sp::OptionSettings _optionSettings;
	std::list<sp::PortData> _portData;

	uint32_t _touchstoneVersion = 1;
	uint32_t _portNumber;

	void AnalyseLine(const std::string& content);
	void AnalyseDataLine(const std::string& content);
	void AnalyseOptionsLine(const std::string& line);
	void AnalyseVersionTwoLine(const std::string& content);

	const std::string CleansOfComments(const std::string& content);
};
