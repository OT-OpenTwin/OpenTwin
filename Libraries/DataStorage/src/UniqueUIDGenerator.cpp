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

#pragma once
#include "stdafx.h"
#include "..\include\UniqueUIDGenerator.h"

std::mutex DataStorageAPI::UniqueUIDGenerator::m_generationlock;

long long DataStorageAPI::UniqueUIDGenerator::getUID()
{
	m_generationlock.lock();

	unsigned long long currentTimeInSec = this->getTimeSince2020();
	
	//WAIT FOR THE LATER TIME IF MAXIMUM COUNT HAS REACHED 
	while ((currentTimeInSec <= timeInSec) && (counter == maxCounter)) {
		Sleep(100);
		currentTimeInSec = this->getTimeSince2020();
	}
	//IF THE NEW TIME IS LATER THAN PREVIOUS ALLOTTED TIME, RESET COUNTER | ELSE, INCREMENT COUNTER 
	if (currentTimeInSec > timeInSec) {
		timeInSec = currentTimeInSec;
		counter = 0;
	}
	else {
		counter++;
	}
	//64 BITS FOR UID | [36-Creation Time][6-Session ID][8-Service ID][14-Counter]
	unsigned long long generatedUID = timeInSec;
	generatedUID <<= 6;
	generatedUID = generatedUID + sessionID;
	generatedUID <<= 8;
	generatedUID = generatedUID + serviceID;
	generatedUID <<= 14;
	generatedUID = generatedUID + counter;

	m_generationlock.unlock();

	return generatedUID;
}

void DataStorageAPI::UniqueUIDGenerator::setSessionID(unsigned int sessionID)
{
	assert(sessionID < 64);
	this->sessionID = sessionID;
}

void DataStorageAPI::UniqueUIDGenerator::setServiceID(unsigned int serviceID)
{
	assert(serviceID < 256);
	this->serviceID = serviceID;
}

unsigned long long DataStorageAPI::UniqueUIDGenerator::getTimeSince2020()
{
	return time(NULL) - secondsTill2020;
}
