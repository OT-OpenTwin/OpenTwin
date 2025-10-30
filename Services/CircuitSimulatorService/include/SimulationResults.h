// @otlicense
// File: SimulationResults.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once
//Service Header
#include "BlockEntityHandler.h"


//Qt Header
#include "QtCore/qjsonarray.h"
#include "QtCore/qjsondocument.h"
#include "QtCore/qjsonobject.h"

//Open Twin Header
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "EntityResultText.h"
//C++ Header
#include <vector>
#include <map>
#include <string>
#include <mutex>

class SimulationResults : public BusinessLogicHandler
{

public:
    
    static SimulationResults* getInstance();
    void addToResultMap(const std::string& key, double value);
    std::map<std::string, std::vector<double>>& getResultMap() {   return resultMap; }

    void addResults(const QJsonValue& _result);
    void setSolverInformation(std::string solverName, std::string simulationType, std::string circuitName);

    //Message handling
    void displayMessage(std::string _message);
    void displayError(std::string _message);
    void handleResults(const QJsonValue& _result);
    void handleUnknownMessageType(std::string _message);
    void storeLogDataInResultText();

    //Time Handling
    void handleCircuitExecutionTiming(const QDateTime& _timePoint, std::string timeType);

    //Setter
    void setVecAmount(int amount)
    {
        this->vecAmount = amount;
    }

    //Getter
    int getVecAmount()
    {
        return this->vecAmount;
    }

    void clearUp();
private:

    SimulationResults() { this->vecAmount = 0; }
    static SimulationResults* instance;
    std::map<std::string, std::vector<double>> resultMap;
    int vecAmount;
    BlockEntityHandler m_blockEntityHandler;
    std::string solverName;
    std::string simulationType;
    std::string circuitName;
    std::mutex m_mutex;
    std::string logData;

    std::vector<int> findPercentage(const std::string& input);

};


