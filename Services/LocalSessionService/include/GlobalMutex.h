#pragma once

// std header
#include <mutex>

#define OT_LSS_GLOBAL_LOCK std::lock_guard<std::mutex> ot_global_mutex_lock(ot::global::mutex);

namespace ot {
	namespace global {
		static std::mutex mutex;
	}
}
