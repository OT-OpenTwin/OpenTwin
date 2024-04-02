#pragma once

//C++ Header
#include <vector>
#include <map>
#include <string>

class SimulationResults
{
private:
   
    SimulationResults() {}
    static SimulationResults* instance;
    std::map<std::string, std::vector<double>> resultMap;

public:
    
    static SimulationResults* getInstance();
    void addToResultMap(const std::string& key, double value);
    std::map<std::string, std::vector<double>>& getResultMap() {   return resultMap; }
};


