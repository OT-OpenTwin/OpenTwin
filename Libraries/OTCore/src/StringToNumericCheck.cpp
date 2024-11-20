#include "OTCore/StringToNumericCheck.h"
#include <stdint.h>
#include <charconv>

bool ot::StringToNumericCheck::fitsInInt32(const std::string& str)
{
	int32_t value;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
	return ec == std::errc() && ptr == str.data() + str.size();
}

bool ot::StringToNumericCheck::fitsInInt64(const std::string& str)
{
	int64_t value;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
	return ec == std::errc() && ptr == str.data() + str.size();
}

bool ot::StringToNumericCheck::fitsInFloat(const std::string& str)
{
	float value;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
	bool fits = ec == std::errc() && ptr == str.data() + str.size();
	if (!fits)
	{
		auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value, std::chars_format::scientific);
		fits = ec == std::errc() && ptr == str.data() + str.size();
	}
	return fits;
}

bool ot::StringToNumericCheck::fitsInDouble(const std::string& str)
{
	double value;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
	bool fits = ec == std::errc() && ptr == str.data() + str.size();
	if (!fits)
	{
		auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value, std::chars_format::scientific);
		fits = ec == std::errc() && ptr == str.data() + str.size();
	}
	return fits;
}
