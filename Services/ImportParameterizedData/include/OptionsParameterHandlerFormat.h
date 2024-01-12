#pragma once
#include "OptionsParameterHandler.h"
#include <map>

class OptionsParameterHandlerFormat : public OptionsParameterHandler
{
public:
	static std::string ToString(const ts::option::Format& format) 
	{ 
		return _formatToString.find(format)->second;
	}
private:
	const std::map<std::string, ts::option::Format> _stringToFormat = { {"db",ts::option::Format::Decibel_angle}, {"ma", ts::option::Format::magnitude_angle}, {"ri", ts::option::Format::real_imaginary}};
	inline static const std::map<ts::option::Format,std::string> _formatToString = { {ts::option::Format::Decibel_angle,"dB"}, {ts::option::Format::magnitude_angle,"MA"}, {ts::option::Format::real_imaginary, "RI"}};
	
	virtual bool IndividualInterpretation(const std::string& entry, ts::OptionSettings& options) override;
};
