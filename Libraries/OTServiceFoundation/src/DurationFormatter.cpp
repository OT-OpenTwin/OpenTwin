#include "OTServiceFoundation/DurationFormatter.h"

std::string DurationFormatter::formatDuration(const std::chrono::steady_clock::time_point _begin, const std::chrono::steady_clock::time_point& _end)
{
	const std::chrono::duration<double, std::milli> duration_ms = (_end - _begin);
	if(duration_ms.count() < 1000.0)
	{
		return std::to_string(static_cast<uint64_t>(duration_ms.count())) + " ms";
	}
	else if(duration_ms.count() < 60000.0)
	{
		const double duration_s = duration_ms.count() / 1000.0;
		return std::to_string(static_cast<uint64_t>(duration_s)) + " s";
	}
	else if(duration_ms.count() < 3600000.0)
	{
		const double duration_min = duration_ms.count() / 60000.0;
		return std::to_string(static_cast<uint64_t>(duration_min)) + " min";
	}
	else
	{
		const double duration_h = duration_ms.count() / 3600000.0;
		return std::to_string(static_cast<uint64_t>(duration_h)) + " h";
	}
	
}
