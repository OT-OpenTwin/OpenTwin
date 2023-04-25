#pragma once
#include <cassert>
#include <time.h>
#include <mutex>
namespace DataStorageAPI
{
	class UniqueUIDGenerator
	{
	public:
		__declspec(dllexport) UniqueUIDGenerator(unsigned int sessionID, unsigned int serviceID){
			setSessionID(sessionID);
			setServiceID(serviceID);
			timeInSec = getTimeSince2020();
			counter = maxCounter;
		}
		__declspec(dllexport) long long getUID();
		__declspec(dllexport) void setSessionID(unsigned int sessionID);
		__declspec(dllexport) void setServiceID(unsigned int serviceID);
	private:
		static std::mutex m_generationlock;
		__declspec(dllexport) unsigned long long getTimeSince2020();
		static const int counterBits = 14;
		int maxCounter = (1 << counterBits) - 1;
		static const int secondsTill2020 = 1577836800;
		unsigned int counter : counterBits;
		unsigned int serviceID : 8;
		unsigned int sessionID : 6;
		unsigned long long timeInSec;
	};
};
