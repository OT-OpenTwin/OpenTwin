// @otlicense
// File: SimulationResults.cpp
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

#include "SimulationResults.h"

SimulationResults* SimulationResults::instance = nullptr;

SimulationResults* SimulationResults::getInstance() {
    
    if (!instance) {
        instance = new SimulationResults();
    }
    return instance;

}

void SimulationResults::addToResultMap(const std::string& key, double value) {

    resultMap[key].push_back(value);
}

void SimulationResults::triggerCallback(std::string messageType, std::string message) {
    
    Q_EMIT callback(messageType, message);
}
