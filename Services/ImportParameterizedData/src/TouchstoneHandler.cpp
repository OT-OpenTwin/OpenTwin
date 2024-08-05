#include "TouchstoneHandler.h"
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

TouchstoneHandler::TouchstoneHandler(const std::string& _fileName)
{
	auto posOfFileExtension = _fileName.find_last_of(".") +1;
	const std::string extension = _fileName.substr(posOfFileExtension, _fileName.size());
}

TouchstoneHandler::TouchstoneHandler(TouchstoneHandler&& _other) noexcept
	:m_comments(std::move(_other.m_comments)), m_optionSettings(std::move(_other.m_optionSettings))/*, m_quantityDescription(std::move(_other.m_quantityDescription))*/, m_touchstoneVersion(std::move(_other.m_touchstoneVersion)), m_portNumber(std::move(_other.m_portNumber))
{
}

TouchstoneHandler& TouchstoneHandler::operator=(TouchstoneHandler&& _other) noexcept
{
	m_comments = (std::move(_other.m_comments));
	m_optionSettings = (std::move(_other.m_optionSettings));
	//m_quantityDescription = (std::move(_other.m_quantityDescription));
	m_touchstoneVersion = (std::move(_other.m_touchstoneVersion));
	m_portNumber = (std::move(_other.m_portNumber));
	return *this;
}

int32_t TouchstoneHandler::deriveNumberOfPorts(const std::string& _fileName)
{
	auto posOfFileExtension = _fileName.find_last_of(".") + 1;
	const std::string extension = _fileName.substr(posOfFileExtension, _fileName.size());
	const std::string numberOfPortsAsString = extension.substr(1, extension.size() - 2);
	return std::stoi(numberOfPortsAsString);
}

void TouchstoneHandler::analyseFile(const std::string& _fileContent, int32_t _numberOfPorts)
{
	m_portNumber = _numberOfPorts;
	m_quantityDescription.setNumberOfPorts(m_portNumber);
	std::string::difference_type numberOfRows = std::count(_fileContent.begin(), _fileContent.end(), '\n');
	m_quantityDescription.reserve(numberOfRows);

	std::stringstream stream;
	ot::EncodingGuesser encodingGuesser;
	ot::TextEncoding::EncodingStandard encodingStandard =	encodingGuesser(_fileContent.c_str(), _fileContent.size());
	if (encodingStandard == ot::TextEncoding::EncodingStandard::ANSI)
	{
		ot::EncodingConverter_ISO88591ToUTF8 converter;
		stream.str(converter(_fileContent));
	}
	else if (encodingStandard == ot::TextEncoding::EncodingStandard::UTF16_BEBOM || encodingStandard == ot::TextEncoding::EncodingStandard::UTF16_LEBOM)
	{
		ot::EncodingConverter_UTF16ToUTF8 converter;
		stream.str(converter(encodingStandard,_fileContent));
	}
	else
	{
		stream.str(_fileContent);
	}
	
	std::string line;
	while (getline(stream, line))
	{
		analyseLine(line);
	}
	m_quantityDescription.optimiseMemory();
}

void TouchstoneHandler::analyseLine(std::string& content)
{
	std::string lineWithoutWhitespaces = content;
	lineWithoutWhitespaces.erase(std::remove_if(lineWithoutWhitespaces.begin(), lineWithoutWhitespaces.end(), isspace), lineWithoutWhitespaces.end());
	if (lineWithoutWhitespaces.size() == 0)
	{
		return;
	}
	if (content[0] == '!')
	{
		m_comments += content.substr(1, content.size()) + "\n";
	}
	else if (content[0] == '#')
	{
		analyseOptionsLine(content);
	}
	else if (content[0] == '[')
	{
		//Version 2.0 not supported yet
		analyseVersionTwoLine(content);
	}
	else //Data section
	{
		analyseDataLine(content);
	}
		
}

void TouchstoneHandler::analyseDataLine(std::string& content)
{
	cleansOfComments(content);
	cleansOfSpecialCharacter(content);

	if (m_touchstoneVersion == 1)
	{
		std::stringstream stream(content);
		std::string segment;

		while (getline(stream, segment, ' '))
		{
			if (segment == "")
			{
				continue;
			}
			else
			{
				if (m_firstValues == nullptr)
				{
					m_firstValues = new SParameterMatrixHelper(m_portNumber);
					m_secondValues = new SParameterMatrixHelper(m_portNumber);
					m_frequencyParameter.values.push_back(turnLineSegmentToVariable(segment));
				}
				else
				{
					if (m_firstValueOfTuple)
					{
						m_firstValues->setValue(m_rowIndex, m_columnIndex, turnLineSegmentToVariable(segment));
					}
					else
					{
						m_secondValues->setValue(m_rowIndex, m_columnIndex, turnLineSegmentToVariable(segment));
						if (m_columnIndex == m_portNumber - 1)
						{
							m_columnIndex = 0;
							if (m_rowIndex == m_portNumber - 1)
							{
								m_rowIndex = 0;
								m_quantityDescription.pushBackFirstValue(std::move(*m_firstValues));
								m_firstValues = nullptr;
								m_quantityDescription.pushBackSecondValue(std::move(*m_secondValues));
								m_secondValues = nullptr;
							}
							else
							{
								m_rowIndex++;
							}
						}
						else
						{
							m_columnIndex++;
						}
					}
					m_firstValueOfTuple ^= true; //flip
				}
			}
		}
	}
	else
	{
		assert(false);//not supported yet
	}
}

void TouchstoneHandler::cleansOfComments(std::string& content)
{
	auto commentCharacterPos = content.find('!');
	if (commentCharacterPos != std::string::npos)
	{
		content = content.substr(0, commentCharacterPos);
	}
}

void TouchstoneHandler::cleansOfSpecialCharacter(std::string& content)
{
	content.erase(std::remove(content.begin(), content.end(), '\n'), content.end());
	content.erase(std::remove(content.begin(), content.end(), '\t'), content.end());
	content.erase(std::remove(content.begin(), content.end(), '\v'), content.end());
	content.erase(std::remove(content.begin(), content.end(), '\b'), content.end());
	content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());
	content.erase(std::remove(content.begin(), content.end(), '\f'), content.end());
}

ot::Variable TouchstoneHandler::turnLineSegmentToVariable(const std::string& _segment)
{
	return ot::Variable(std::stoll(_segment));
}

void TouchstoneHandler::analyseVersionTwoLine(std::string& content)
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

void TouchstoneHandler::analyseOptionsLine(std::string& _line)
{
	assert(_line[0] == '#');
	OptionsParameterHandlerFormat handlerFormat;
	OptionsParameterHandlerFrequency handlerFrequency;
	OptionsParameterHandlerParameter handlerParameter;
	OptionsParameterHandlerReference handlerReference;

	handlerFormat.SetNextHandler(handlerFrequency.SetNextHandler(handlerParameter.SetNextHandler(&handlerReference)));

	cleansOfComments(_line);
	cleansOfSpecialCharacter(_line);

	std::stringstream ss(_line.substr(1,_line.size()));
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
			handlerFormat.InterpreteOptionsParameter(buffered, m_optionSettings);
		}
		else
		{
			std::transform(segment.begin(), segment.end(), segment.begin(),
				[](unsigned char c) { return std::tolower(c); });
			handlerFormat.InterpreteOptionsParameter(segment, m_optionSettings);
		}
	}
}

