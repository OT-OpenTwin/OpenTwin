// @otlicense

#pragma once
#include <chrono>
#include <string>

class __declspec(dllexport) TimeFormatter
{
public:
	static std::string formatDuration(const std::chrono::steady_clock::time_point _begin, const std::chrono::steady_clock::time_point& _end);
	static const std::string createCurrentDateTimeString();
};
