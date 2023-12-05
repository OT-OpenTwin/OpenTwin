#pragma once

#ifdef _PerformanceTest
#include <chrono>
#define TakeTime(x) std::chrono::system_clock::time_point x = std::chrono::system_clock::now()
#define GetInMicroSeconds (t1, t2) std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count())
#else
#define GetInMicroSeconds (t1, t2) {}
#define TakeTime(x) {}

#endif
