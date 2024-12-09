#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// std header
#include <mutex>

class MutexLock : std::lock_guard<std::mutex> {
	OT_DECL_NOCOPY(MutexLock)
public:
	MutexLock();
};
