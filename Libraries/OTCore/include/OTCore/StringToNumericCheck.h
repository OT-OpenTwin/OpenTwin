#pragma once
#include <string>


namespace ot
{
	namespace StringToNumericCheck
	{
		bool fitsInInt32(const std::string& str);
		bool fitsInInt64(const std::string& str);
		bool fitsInFloat(const std::string& str);
		bool fitsInDouble(const std::string& str);
	}
}
