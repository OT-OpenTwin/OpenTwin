#include "MutexLock.h"
#include "MainLockManager.h"

MutexLock::MutexLock() 
	: std::lock_guard<std::mutex>(MainLockManager::getMutex())
{

}
