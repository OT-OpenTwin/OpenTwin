#include "SimulationResults.h"

SimulationResults* SimulationResults::instance = nullptr;

SimulationResults* SimulationResults::getInstance()
{
    if (!instance) {
        instance = new SimulationResults();
    }
    return instance;
    
}

void SimulationResults::addToResultMap(const std::string& key, double value)
{
    resultMap[key].push_back(value);
}
