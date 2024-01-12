#pragma once
#include "OptionsParameterHandler.h"
#include "OTCore/StringToVariableConverter.h"

class OptionsParameterHandlerReference : public OptionsParameterHandler
{
public:

private:
	const char _initialCharacter = 'R';
	
	virtual bool IndividualInterpretation(const std::string& entry, ts::OptionSettings& options) override;

};