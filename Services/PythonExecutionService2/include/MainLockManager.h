#pragma once

// Service header
#include "MutexLock.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"

//! @brief The main lock is used to synchronize the qt loop with the main application thread.
class MainLockManager {
	OT_DECL_NOCOPY(MainLockManager)
public:
	static MainLockManager& instance(void);
	static std::mutex& getMutex(void);

private:
	MainLockManager() {};
	~MainLockManager() {};

	std::mutex m_mutex;
};