#pragma once
#include <gtest/gtest.h>

#include "TouchstoneHandler.h"

class FixtureTouchstoneHandler : public testing::Test
{
public:
		
	const ts::OptionSettings& AnalyseOptionSettings(const std::string& text) 
	{ 
		_handler.AnalyseLine(text);
		return _handler.getOptionSettings(); 
	}
	
	void AnalyseLine(const std::string& text)
	{
		_handler.AnalyseLine(text);
	}
	
	void AnalyseFile(const std::string& fileContent)
	{
		_handler.AnalyseFile(fileContent);
	}

	const std::string& GetComments()
	{
		return _handler.getComments();
	}

	const std::string CleansLineOfComments(const std::string& line)
	{
		return _handler.CleansOfComments(line);
	}

	void SetNumberOfPorts(uint32_t portNumber)
	{
		_handler._portNumber = portNumber;
	}

	const std::list<ts::PortData>& GetPortData()
	{
		return _handler.getPortData();
	}

	const ts::OptionSettings& GetOptionSettings()
	{
		return _handler.getOptionSettings();
	}

	const std::string& GetFullExampleFourPorts() const { return _exampleFourPorts; };
private:
	TouchstoneHandler _handler;

	const std::string _exampleFourPorts =
		"!4 - port S - parameter data, taken at three frequency points\n"
		"!note that data points need not be aligned\n"
		"# Hz Y RI R 90\n"
		"5.00000 0.60 161.24 0.40 -42.20 0.42 -66.58 0.53 -79.34 !row 1\n"
		"0.40 -42.20 0.60 161.20 0.53 -79.34 0.42 -66.58 !row 2\n"
		"0.42 -66.58 0.53 -79.34 0.60 161.24 0.40 -42.20 !row 3\n"
		"0.53 -79.34 0.42 -66.58 0.40 -42.20 0.60 161.24 !row 4\n"
		"6.00000 0.57 150.37 0.40 -44.34 0.41 -81.24 0.57 -95.77 !row 1\n"
		"0.40 -44.34 0.57 150.37 0.57 -95.77 0.41 -81.24 !row 2\n"
		"0.41 -81.24 0.57 -95.77 0.57 150.37 0.40 -44.34 !row 3\n"
		"0.57 -95.77 0.41 -81.24 0.40 -44.34 0.57 150.37 !row 4\n"
		"7.00000 0.50 136.69 0.45 -46.41 0.37 -99.09 0.62 -114.19 !row 1\n"
		"0.45 -46.41 0.50 136.69 0.62 -114.19 0.37 -99.09 !row 2\n"
		"0.37 -99.09 0.62 -114.19 0.50 136.69 0.45 -46.41 !row 3\n"
		"0.62 -114.19 0.37 -99.09 0.45 -46.41 0.50 136.69 !row 4\n";
};
