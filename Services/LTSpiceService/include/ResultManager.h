#pragma once

#include <list>
#include <string>

#include "OTServiceFoundation/BusinessLogicHandler.h"

#include "ParametricCombination.h"

namespace ltspice
{
	class RawData;
}

class ResultManager : public BusinessLogicHandler
{
public:
	ResultManager(ot::components::ModelComponent* modelComponent);
	virtual ~ResultManager();

	void extractResults(const std::string& ltSpicefileNameBase);

private:
	void storeParametricResults(ltspice::RawData& resultData, std::list<ParametricCombination>& parameterRuns);
	void getParametricCombinations(const std::string& logFileName, std::list<ParametricCombination>& parameterRuns);



};
