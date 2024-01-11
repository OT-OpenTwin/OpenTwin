#include "SParameterHandler.h"
#include "OptionsParameterHandler.h"
#include "OptionsParameterHandlerFormat.h"
#include "OptionsParameterHandlerFrequency.h"
#include "OptionsParameterHandlerParameter.h"
#include "OptionsParameterHandlerReference.h"

#include "OTCore/EncodingGuesser.h"
#include "OTCore/EncodingConverter_ISO88591ToUTF8.h"
#include "OTCore/EncodingConverter_UTF16ToUTF8.h"

#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>

void SParameterHandler::AnalyseFile(const std::string& fileContent)
{
	std::stringstream stream;
	ot::EncodingGuesser encodingGuesser;
	ot::TextEncoding::EncodingStandard encodingStandard =	encodingGuesser(fileContent.c_str(), fileContent.size());
	if (encodingStandard == ot::TextEncoding::EncodingStandard::ANSI)
	{
		ot::EncodingConverter_ISO88591ToUTF8 converter;
		stream.str(converter(fileContent));
	}
	else if (encodingStandard == ot::TextEncoding::EncodingStandard::UTF16_BEBOM || encodingStandard == ot::TextEncoding::EncodingStandard::UTF16_LEBOM)
	{
		ot::EncodingConverter_UTF16ToUTF8 converter;
		stream.str(converter(encodingStandard,fileContent));
	}
	else
	{
		stream.str(fileContent);
	}
	
	std::string line;
	while (getline(stream, line))
	{
		AnalyseLine(line);
	}
}

void SParameterHandler::AnalyseLine(const std::string& content)
{
	std::string lineWithoutWhitespaces = content;
	lineWithoutWhitespaces.erase(std::remove_if(lineWithoutWhitespaces.begin(), lineWithoutWhitespaces.end(), isspace), lineWithoutWhitespaces.end());
	if (lineWithoutWhitespaces.size() == 0)
	{
		return;
	}
	if (content[0] == '!')
	{
		_comments += content.substr(1, content.size()) + "\n";
	}
	else if (content[0] == '#')
	{
		AnalyseOptionsLine(content);
	}
	else if (content[0] == '[')
	{
		//Version 2.0 not supported yet
		AnalyseVersionTwoLine(content);
	}
	else //Data section
	{
		AnalyseDataLine(content);
	}
		
}

void SParameterHandler::AnalyseDataLine(const std::string& content)
{

	std::string cleansedContent = CleansOfComments(content);
	if (cleansedContent.back() == '\n')
	{
		cleansedContent = cleansedContent.substr(0, cleansedContent.size() - 1);
	}
	if (_touchstoneVersion == 1)
	{
		std::stringstream stream(cleansedContent);
		std::string segment;
		
		while (getline(stream, segment, ' '))
		{
			if (segment == "")
			{
				continue;
			}
			else
			{
				if (_portData.size() == 0 || _portData.back().isFilled())
				{
					_portData.push_back(sp::PortData(_portNumber));
				}
				_portData.back().AddValue(segment);
			}
		}
	}
	else
	{

	}
}

const std::string SParameterHandler::CleansOfComments(const std::string& content)
{
	auto commentCharacterPos = content.find('!');
	if (commentCharacterPos != std::string::npos)
	{
		return content.substr(0, commentCharacterPos);
	}
	else
	{
		return content;
	}
}

void SParameterHandler::AnalyseVersionTwoLine(const std::string& content)
{
	const std::string version = "[Version]";
	const std::string numberOfPorts = "[Number of Ports]";
	const std::string twoPortDataOrder = "[Two-Port Data Order]";
	const std::string numberOfFrequencies = "[Number of Frequencies]";
	const std::string numberOfNoiceFrequencies = "[Number of Noise Frequencies]";
	const std::string reference = "[Reference]";
	const std::string matrixFormat = "[Matrix Format]";
	const std::string networkData= "[Network Data]";
	const std::string mixedModeOrder= "[Mixed-Mode Order]";
	const std::string noiseData= "[Noise Data]";
	const std::string end = "[End]";
	const std::string beginInformation = "[Begin Information]";
	const std::string endInformation = "[End Information]";

}



void SParameterHandler::AnalyseOptionsLine(const std::string& line)
{
	assert(line[0] == '#');
	OptionsParameterHandlerFormat handlerFormat;
	OptionsParameterHandlerFrequency handlerFrequency;
	OptionsParameterHandlerParameter handlerParameter;
	OptionsParameterHandlerReference handlerReference;

	handlerFormat.SetNextHandler(handlerFrequency.SetNextHandler(handlerParameter.SetNextHandler(&handlerReference)));

	std::stringstream ss(line.substr(1,line.size()));
	std::string segment;
	std::string buffered = "";
	while (getline(ss, segment, ' '))
	{
		if (segment == "")
		{
			continue;
		}
		else if (segment == "R")
		{
			buffered = segment;
		}
		else if (buffered != "")
		{
			buffered += " " + segment;
			handlerFormat.InterpreteOptionsParameter(buffered, _optionSettings);
		}
		else
		{
			handlerFormat.InterpreteOptionsParameter(segment, _optionSettings);
		}
	}
}

