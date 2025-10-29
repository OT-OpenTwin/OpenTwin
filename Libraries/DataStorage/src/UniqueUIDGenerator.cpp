// @otlicense

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
