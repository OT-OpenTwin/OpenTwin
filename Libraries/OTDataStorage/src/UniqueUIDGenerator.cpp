// @otlicense
// File: UniqueUIDGenerator.cpp
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

// OpenTwin header
#include "OTSystem/Exception.h"
#include "OTCore/Logging/Logger.h"
#include "OTDataStorage/UniqueUIDGenerator.h"

ot::UniqueUIDGenerator::UniqueUIDGenerator(unsigned int _sessionID, unsigned int _serviceID)
	: m_sessionID(_sessionID), m_serviceID(_serviceID), m_counter(c_maxCounter), m_lastTime(getTimeSince2020())
{
	if (m_sessionID > c_maxSessionID) {
		throw Exception::OutOfBounds("Session ID exceeds maximum value for UID generation");
	}
	if (m_serviceID > c_maxServiceID) {
		throw Exception::OutOfBounds("Service ID exceeds maximum value for UID generation");
	}
}

ot::UID ot::UniqueUIDGenerator::getUID()
{
	std::lock_guard<std::mutex> lock(getGenerationLock());

	std::time_t currentTimeInSec = this->getTimeSince2020();
	
	// Wait 100us if the maximum generation count has been reached
	while ((currentTimeInSec <= m_lastTime) && (m_counter >= c_maxCounter)) {
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		currentTimeInSec = this->getTimeSince2020();
	}

	// If this uid generation is taking place at a later second in time, reset the counter, otherwise increment it
	if (currentTimeInSec > m_lastTime) {
		m_lastTime = currentTimeInSec;
		m_counter = 0;
	}
	else {
		m_counter++;
	}

	//  64bit           36bits                6bits              8bits             14bits
	//  UID     [63..28 CreationTime] [27..22 SessionID] [21..14 ServiceID] [13..0 Counter]
	UID generatedUID = static_cast<UID>(m_lastTime);
	generatedUID <<= c_sessionIDBits;
	generatedUID = generatedUID + m_sessionID;
	generatedUID <<= c_serviceIDBits;
	generatedUID = generatedUID + m_serviceID;
	generatedUID <<= c_counterBits;
	generatedUID = generatedUID + m_counter;

	return generatedUID;
}

std::mutex& ot::UniqueUIDGenerator::getGenerationLock()
{
	static std::mutex g_generationLock;
	return g_generationLock;
}
