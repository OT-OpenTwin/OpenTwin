#pragma once
#include <string>


namespace ot
{
	namespace StringToNumericCheck
	{
		bool __declspec(dllexport) fitsInInt32(const std::string& str);
		bool __declspec(dllexport) fitsInInt64(const std::string& str);
		bool __declspec(dllexport) fitsInFloat(const std::string& str);
		bool __declspec(dllexport) fitsInDouble(const std::string& str);
	}
}
