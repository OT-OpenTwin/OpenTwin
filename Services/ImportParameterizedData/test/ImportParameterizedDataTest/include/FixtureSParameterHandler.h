#pragma once
#include <gtest/gtest.h>

#include "SParameterHandler.h"

class FixtureSParameterHandler: public testing::Test
{
public:
		
	const sp::OptionSettings& GetOptionSettings(const std::string& text) 
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
private:
	SParameterHandler _handler;
};
