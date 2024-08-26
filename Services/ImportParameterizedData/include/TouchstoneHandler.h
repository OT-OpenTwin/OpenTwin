#pragma once
#include <string>
#include <list>
#include "OptionSettings.h"
#include "QuantityDescriptionSParameter.h"
#include "DatasetDescription.h"
#include "MetadataParameter.h"

class TouchstoneHandler
{
public:
	TouchstoneHandler(const std::string& _fileName);
	TouchstoneHandler(const TouchstoneHandler& _other) = delete;
	TouchstoneHandler(TouchstoneHandler&& _other) noexcept = default;
	TouchstoneHandler& operator=(const TouchstoneHandler& _other) = delete;
	TouchstoneHandler& operator=(TouchstoneHandler&& _other) noexcept = default;

	static int32_t deriveNumberOfPorts(const std::string& _fileName);
	
	void analyseFile(const std::string& _fileContent, int32_t _numberOfPorts);
	const ts::OptionSettings& getOptionSettings() const { return m_optionSettings; }
	const std::string& getComments() const { return m_comments; }
	QuantityDescriptionSParameter* handOverQuantityDescription() 
	{
		QuantityDescriptionSParameter* temp = m_quantityDescription;
		m_quantityDescription = nullptr;
		return temp;
	}
	MetadataParameter& getMetadataFrequencyParameter() { return m_frequencyParameter; }

private:
	friend class FixtureTouchstoneHandler;

	std::string m_comments = "";
	ts::OptionSettings m_optionSettings;
	
	uint32_t m_touchstoneVersion = 1;
	uint32_t m_portNumber = 0;
	
	QuantityDescriptionSParameter* m_quantityDescription = nullptr;
	MetadataParameter m_frequencyParameter;

	uint32_t m_columnIndex = 0;
	uint32_t m_rowIndex = 0;

	SParameterMatrixHelper* m_firstValues = nullptr;
	SParameterMatrixHelper* m_secondValues = nullptr;

	bool m_firstValueOfTuple = true;

	void analyseLine(std::string& _content);
	void analyseDataLine(std::string& _content);
	void analyseOptionsLine(std::string& _line);
	void analyseVersionTwoLine(std::string& _content);

	void cleansOfComments(std::string& _content);
	void cleansOfSpecialCharacter(std::string& _content);

	//! @brief Currently all segments are turned into double values.
	ot::Variable turnLineSegmentToVariable(const std::string& _segment);
};

