// @otlicense

#pragma once
#include <string>
#include <exception>

#include "Options.h"
#include "OptionSettings.h"
class OptionsParameterHandler
{
public:
	virtual OptionsParameterHandler* SetNextHandler(OptionsParameterHandler* nextHandler)
	{
		_nextHandler = nextHandler;
		return this;
	}

	void InterpreteOptionsParameter(const std::string& entry, ts::OptionSettings& options)
	{
		const bool suitableHandlerFound = IndividualInterpretation(entry, options);
		if (!suitableHandlerFound)
		{
			if (_nextHandler == nullptr)
			{
				throw std::exception(std::string("Entry \"" + entry + " \" is not interpretable as content of a .snp options line.").c_str());
			}
			else
			{
				_nextHandler->InterpreteOptionsParameter(entry, options);
			}
		}
	}
protected:
	/// <summary>
	/// Requires a string in lower cases.
	/// </summary>
	/// <param name="entry"></param>
	/// <param name="options"></param>
	/// <returns></returns>
	virtual bool IndividualInterpretation(const std::string& entry, ts::OptionSettings& options) = 0;

private:

	OptionsParameterHandler* _nextHandler = nullptr;
};
