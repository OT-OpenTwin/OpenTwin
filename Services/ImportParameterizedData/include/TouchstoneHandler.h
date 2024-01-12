#pragma once
#include <string>
#include <list>
#include "OptionSettings.h"
#include "PortData.h"

class TouchstoneHandler
{
public:
	TouchstoneHandler(const std::string& fileName);
	TouchstoneHandler(const TouchstoneHandler& other) = delete;
	TouchstoneHandler(TouchstoneHandler&& other) noexcept;
	TouchstoneHandler& operator=(const TouchstoneHandler& other) = delete;
	TouchstoneHandler& operator=(TouchstoneHandler&& other) noexcept;

	const uint32_t getNumberOfPorts() const { return _portNumber; }
	
	void AnalyseFile(const std::string& fileContent);
	const ts::OptionSettings& getOptionSettings() const { return _optionSettings; }
	const std::string& getComments() const { return _comments; }
	const std::list<ts::PortData>& getPortData() const { return _portData; }

private:
	friend class FixtureTouchstoneHandler;
	std::string _comments = "";
	ts::OptionSettings _optionSettings;
	std::list<ts::PortData> _portData;

	uint32_t _touchstoneVersion = 1;
	uint32_t _portNumber;

	void AnalyseLine(std::string& content);
	void AnalyseDataLine(std::string& content);
	void AnalyseOptionsLine(std::string& line);
	void AnalyseVersionTwoLine(std::string& content);

	void CleansOfComments(std::string& content);
	void CleansOfSpecialCharacter(std::string& content);


};

