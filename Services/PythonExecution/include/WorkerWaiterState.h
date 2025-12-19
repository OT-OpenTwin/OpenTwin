#pragma once
#include <condition_variable>
#include <mutex>
#include <atomic>

struct WorkerWaiterState {
    std::mutex m_mutex;
    std::condition_variable m_conditionalVariable;
    std::atomic_bool m_isWorkDone = false;
};
