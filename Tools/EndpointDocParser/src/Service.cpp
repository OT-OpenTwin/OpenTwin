// @otlicense
// File: Service.cpp
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

// Open Twin header
#include "OTCore/LogDispatcher.h"

// project header
#include "Service.h"

// std header
#include <iostream>

void Service::printService() const {
    OT_LOG_D("Printing Service: " + m_name);
    OT_LOG_D("==================");
    OT_LOG_D("Number of endpoints in service: " + std::to_string(getEndpoints().size()));
    OT_LOG_D("Endpoint actions:");
    for (const Endpoint& endpoint : m_endpoints) {
        OT_LOG_D(endpoint.getAction());
    }
}
