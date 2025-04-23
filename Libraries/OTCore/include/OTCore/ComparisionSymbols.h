#pragma once
#include <string>
#include <list>
namespace ot
{
	struct __declspec(dllexport) ComparisionSymbols
	{
		inline static const std::string g_anyOneOfComparator = "Any of";
		inline static const std::string g_rangeComparator = "Range";
		inline static const std::string g_noneOfComparator = "Not any of";
		inline static const std::list<std::string> g_comparators = { "<", "<=", "=", ">", ">=", "!=", g_anyOneOfComparator, g_noneOfComparator, g_rangeComparator };
	};
}
